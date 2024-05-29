// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL
// NDEBUG causes crashes in CGAL sometimes. Anton
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "Polyhedra.hpp"

namespace yade { // Cannot have #include directive inside.

using math::max;
using math::min; // using inside .cpp file is ok.

YADE_PLUGIN(/* self-contained in hpp: */ (Polyhedra)(PolyhedraGeom)(Bo1_Polyhedra_Aabb)(PolyhedraPhys)(PolyhedraMat)(
        Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys)(Ip2_FrictMat_PolyhedraMat_FrictPhys)(Law2_PolyhedraGeom_PolyhedraPhys_Volumetric));

CREATE_LOGGER(Law2_PolyhedraGeom_PolyhedraPhys_Volumetric);
CREATE_LOGGER(Polyhedra);

//*********************************************************************************
/* Polyhedra Constructor */

Polyhedra::Polyhedra(const std::vector<Vector3r>&& V)
{
	createIndex();
	v = V;
	Initialize();
}

Polyhedra::Polyhedra(const Vector3r&& xsize, const int&& xseed)
{
	createIndex();
	seed = xseed;
	size = xsize;
	v.clear();
	Initialize();
}
void Polyhedra::Initialize()
{
	if (init) return;
	bool isRandom = false;

	//get vertices
	int N = (int)v.size();
	if (N == 0) {
		//generate randomly
		while ((int)v.size() < 4)
			GenerateRandomGeometry();
		N        = (int)v.size();
		isRandom = true;
	}

	//compute convex hull of vertices
	std::vector<CGALpoint> points;
	points.resize(v.size());
	for (int i = 0; i < N; i++) {
		points[i] = CGALpoint(v[i][0], v[i][1], v[i][2]);
	}

	CGAL::convex_hull_3(
	        points.begin(),
	        points.end(),
	        P); // i.e. ch_quickhull_polyhedron_3 in convex_hull_3.h see https://doc.cgal.org/4.11.3/Convex_hull_3/group__PkgConvexHull3Functions.html#gadc8318947c2133e56b2e56171b2ecd7d,
	if (int(P.size_of_vertices()) != N)
		LOG_WARN(
		        "Polyhedra surface description downgraded from " << N << " points to " << P.size_of_vertices()
		                                                         << ". Are you sure your vertices input is convex ?")

	//connect triagular facets if possible
	std::transform(P.facets_begin(), P.facets_end(), P.planes_begin(), Plane_equation());
	P = Simplify(P, 1E-9);

	//modify order of v according to CGAl polyhedron
	//int i = 0;
	v.clear();
	for (Polyhedron::Vertex_iterator vIter = P.vertices_begin(); vIter != P.vertices_end(); ++vIter /*, i++*/) {
		v.push_back(Vector3r(vIter->point().x(), vIter->point().y(), vIter->point().z()));
	}

	//list surface triangles for plotting
	faceTri.clear();
	std::transform(P.facets_begin(), P.facets_end(), P.planes_begin(), Plane_equation());
	for (Polyhedron::Facet_iterator fIter = P.facets_begin(); fIter != P.facets_end(); fIter++) {
		Polyhedron::Halfedge_around_facet_circulator hfc0;
		int                                          n = fIter->facet_degree();
		hfc0                                           = fIter->facet_begin();
		int a                                          = std::distance(P.vertices_begin(), hfc0->vertex());
		for (int i = 2; i < n; i++) {
			++hfc0;
			faceTri.push_back(a);
			faceTri.push_back(std::distance(P.vertices_begin(), hfc0->vertex()));
			faceTri.push_back(std::distance(P.vertices_begin(), hfc0->next()->vertex()));
		}
	}

	//compute centroid and volume
	P_volume_centroid(P, &volume, &centroid);
	//check vierd behavior of CGAL in tessalation
	if (isRandom && volume * 1.75 < 4. / 3. * 3.14 * size[0] / 2. * size[1] / 2. * size[2] / 2.) {
		v.clear();
		seed = rand();
		Initialize();
	}
	Vector3r translation((-1) * centroid);

	//set centroid to be [0,0,0]
	for (int i = 0; i < N; i++) {
		v[i] = v[i] - centroid;
	}
	if (isRandom) centroid = Vector3r::Zero();

	Vector3r origin(0, 0, 0);

	//move and rotate also the CGAL structure Polyhedron
	Transformation t_trans(1., 0., 0., translation[0], 0., 1., 0., translation[1], 0., 0., 1., translation[2], 1.);
	std::transform(P.points_begin(), P.points_end(), P.points_begin(), t_trans);

	//compute inertia
	Real     vtet;
	Vector3r ctet;
	Matrix3r Itet1, Itet2;
	Matrix3r inertia_tensor(Matrix3r::Zero());
	for (int i = 0; i < (int)faceTri.size(); i += 3) {
		vtet  = math::abs((origin - v[faceTri[i + 2]]).dot((v[faceTri[i]] - v[faceTri[i + 2]]).cross(v[faceTri[i + 1]] - v[faceTri[i + 2]])) / 6.);
		ctet  = (origin + v[faceTri[i]] + v[faceTri[i + 1]] + v[faceTri[i + 2]]) / 4.;
		Itet1 = TetraInertiaTensor(origin - ctet, v[faceTri[i]] - ctet, v[faceTri[i + 1]] - ctet, v[faceTri[i + 2]] - ctet);
		ctet  = ctet - origin;
		Itet2 << ctet[1] * ctet[1] + ctet[2] * ctet[2], -ctet[0] * ctet[1], -ctet[0] * ctet[2], -ctet[0] * ctet[1],
		        ctet[0] * ctet[0] + ctet[2] * ctet[2], -ctet[2] * ctet[1], -ctet[0] * ctet[2], -ctet[2] * ctet[1],
		        ctet[1] * ctet[1] + ctet[0] * ctet[0];
		inertia_tensor = inertia_tensor + Itet1 + Itet2 * vtet;
	}

	if (math::abs(inertia_tensor(0, 1)) + math::abs(inertia_tensor(0, 2)) + math::abs(inertia_tensor(1, 2)) < 1E-13) {
		// no need to rotate, inertia already diagonal
		orientation = Quaternionr::Identity();
		inertia     = Vector3r(inertia_tensor(0, 0), inertia_tensor(1, 1), inertia_tensor(2, 2));
	} else {
		// calculate eigenvectors of I
		Vector3r rot;
		Matrix3r I_rot(Matrix3r::Zero()), I_new(Matrix3r::Zero());
		matrixEigenDecomposition(inertia_tensor, I_rot, I_new);
		// I_rot = eigenvectors of inertia_tensors in columns
		// I_new = eigenvalues on diagonal
		// set positove direction of vectors - otherwise reloading does not work
		Matrix3r sign(Matrix3r::Zero());
		Real     max_v_signed = I_rot(0, 0);
		Real     max_v        = math::abs(I_rot(0, 0));
		if (max_v < math::abs(I_rot(1, 0))) {
			max_v_signed = I_rot(1, 0);
			max_v        = math::abs(I_rot(1, 0));
		}
		if (max_v < math::abs(I_rot(2, 0))) {
			max_v_signed = I_rot(2, 0);
			max_v        = math::abs(I_rot(2, 0));
		}
		sign(0, 0)   = max_v_signed / max_v;
		max_v_signed = I_rot(0, 1);
		max_v        = math::abs(I_rot(0, 1));
		if (max_v < math::abs(I_rot(1, 1))) {
			max_v_signed = I_rot(1, 1);
			max_v        = math::abs(I_rot(1, 1));
		}
		if (max_v < math::abs(I_rot(2, 1))) {
			max_v_signed = I_rot(2, 1);
			max_v        = math::abs(I_rot(2, 1));
		}
		sign(1, 1) = max_v_signed / max_v;
		sign(2, 2) = 1.;
		I_rot      = I_rot * sign;
		// force the eigenvectors to be right-hand oriented
		Vector3r third = (I_rot.col(0)).cross(I_rot.col(1));
		I_rot(0, 2)    = third[0];
		I_rot(1, 2)    = third[1];
		I_rot(2, 2)    = third[2];

		inertia     = Vector3r(I_new(0, 0), I_new(1, 1), I_new(2, 2));
		orientation = Quaternionr(I_rot);
		//rotate the voronoi cell so that x - is maximal inertia axis and z - is minimal inertia axis
		//orientation.normalize();  //not needed
		for (int i = 0; i < (int)v.size(); i++) {
			v[i] = orientation.conjugate() * v[i];
		}

		//rotate also the CGAL structure Polyhedron
		Matrix3r       rot_mat = (orientation.conjugate()).toRotationMatrix();
		Transformation t_rot(
		        rot_mat(0, 0),
		        rot_mat(0, 1),
		        rot_mat(0, 2),
		        rot_mat(1, 0),
		        rot_mat(1, 1),
		        rot_mat(1, 2),
		        rot_mat(2, 0),
		        rot_mat(2, 1),
		        rot_mat(2, 2),
		        1.);
		std::transform(P.points_begin(), P.points_end(), P.points_begin(), t_rot);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wuse-after-free" // this warning is triggered in line 204
	}
/*
 * Error in this closing bracket is following:

In member function ‘void CGAL::Ref_counted_virtual::remove_reference()’,
    inlined from ‘CGAL::Handle_for_virtual<RefCounted>::~Handle_for_virtual() [with RefCounted = CGAL::Aff_transformation_rep_baseC3<CGAL::ERealHP<1> >]’ at /usr/include/CGAL/Handle_for_virtual.h:83:28,
    inlined from ‘CGAL::Aff_transformationC3<CGAL::ERealHP<1> >::~Aff_transformationC3()’ at /usr/include/CGAL/Cartesian/Aff_transformation_3.h:40:7,
    inlined from ‘CGAL::Aff_transformation_3<CGAL::ERealHP<1> >::~Aff_transformation_3()’ at /usr/include/CGAL/Aff_transformation_3.h:31:7,
    inlined from ‘void yade::Polyhedra::Initialize()’ at /home/ytest/yade/trunk/pkg/polyhedra/Polyhedra_01.cpp:204:2:
 usr/include/CGAL/Handle_for_virtual.h:34:34: error: pointer used after ‘void operator delete(void*, std::size_t)’ [-Werror=use-after-free]
 34 |     void  remove_reference() { --count; }

*/
#pragma GCC diagnostic pop
	//initialization done
	init = 1;
}

void Polyhedra::setVertices(const std::vector<Vector3r>& v2)
{
	init    = false;
	this->v = v2;
	Initialize();
}

void Polyhedra::setVertices4(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2, const Vector3r& v3)
{
	init = false;
	v.resize(4);
	v[0] = v0;
	v[1] = v1;
	v[2] = v2;
	v[3] = v3;
	Initialize();
}

//**************************************************************************
/* Generator of randomly shaped polyhedron based on Voronoi tessellation*/

void Polyhedra::GenerateRandomGeometry()
{
	srand(seed);

	vector<CGALpoint> nuclei;
	nuclei.push_back(CGALpoint(5., 5., 5.));
	CGALpoint    trial;
	unsigned int iter = 0;
	bool         isOK;
	//fill box 5x5x5 with randomly located nuclei with restricted minimal mutual distance 0.75 which gives approximate mean mutual distance 1;
	Real dist_min2 = 0.75 * 0.75;
	while (iter < 500) {
		isOK = true;
		iter++;
		trial = CGALpoint(Real(rand()) / RAND_MAX * 5. + 2.5, Real(rand()) / RAND_MAX * 5. + 2.5, Real(rand()) / RAND_MAX * 5. + 2.5);
		for (int i = 0; i < (int)nuclei.size(); i++) {
			isOK = pow(nuclei[i].x() - trial.x(), 2) + pow(nuclei[i].y() - trial.y(), 2) + pow(nuclei[i].z() - trial.z(), 2) > dist_min2;
			if (!isOK) break;
		}
		if (isOK) {
			iter = 0;
			nuclei.push_back(trial);
		}
	}

	//perform Voronoi tessellation
	nuclei.erase(nuclei.begin());
	Triangulation                dt(nuclei.begin(), nuclei.end());
	Triangulation::Vertex_handle zero_point = dt.insert(CGALpoint(5., 5., 5.));
	v.clear();
	std::vector<Triangulation::Cell_handle> ch_cells;
	dt.incident_cells(zero_point, std::back_inserter(ch_cells));
	for (auto ci = ch_cells.begin(); ci != ch_cells.end(); ++ci) {
		v.push_back(FromCGALPoint(dt.dual(*ci)) - Vector3r(5., 5., 5.));
	}

	//resize and rotate the voronoi cell
	Quaternionr Rot(Real(rand()) / RAND_MAX, Real(rand()) / RAND_MAX, Real(rand()) / RAND_MAX, Real(rand()) / RAND_MAX);
	Rot.normalize();
	for (int i = 0; i < (int)v.size(); i++) {
		v[i] = Rot * (Vector3r(v[i][0] * size[0], v[i][1] * size[1], v[i][2] * size[2]));
	}

	//to avoid patological cases (that should not be present, but CGAL works somehow unpredicable)
	if (v.size() < 8) {
		cout << "wrong " << v.size() << endl;
		v.clear();
		seed = rand();
		GenerateRandomGeometry();
	}
}

//**************************************************************************
/* Get polyhdral surfaces */
vector<vector<int>> Polyhedra::GetSurfaces() const
{
	vector<vector<int>> ret(P.size_of_facets());
	int                 i = 0;
	for (Polyhedron::Facet_const_iterator f = P.facets_begin(); f != P.facets_end(); f++, i++) {
		Polyhedron::Halfedge_around_facet_const_circulator h = f->facet_begin();
		do {
			ret[i].push_back(std::distance(P.vertices_begin(), h->vertex()));
		} while (++h != f->facet_begin());
	}
	return ret;
}

Vector3r Polyhedra::GetCentroid()
{
	Initialize();
	return centroid;
}

Vector3r Polyhedra::GetInertia()
{
	Initialize();
	return inertia;
}

vector<int> Polyhedra::GetSurfaceTriangulation()
{
	Initialize();
	return faceTri;
}

Real Polyhedra::GetVolume()
{
	Initialize();
	return volume;
}

Quaternionr Polyhedra::GetOri()
{
	Initialize();
	return orientation;
}

void Polyhedra::Clear()
{
	v.clear();
	P.clear();
	init = 0;
	size = Vector3r(1., 1., 1.);
	faceTri.clear();
}

bool Polyhedra::IsInitialized() const { return init; }

Polyhedron Polyhedra::GetPolyhedron() const { return P; }

} // namespace yade

#endif // YADE_CGAL

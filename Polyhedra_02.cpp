// © 2013 Jan Elias, http://www.fce.vutbr.cz/STM/elias.j/, elias.j@fce.vutbr.cz
// https://www.vutbr.cz/www_base/gigadisk.php?i=95194aa9a

#ifdef YADE_CGAL
// NDEBUG causes crashes in CGAL sometimes. Anton
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "Polyhedra.hpp"
#ifdef YADE_OPENGL
#include <lib/opengl/OpenGLWrapper.hpp>
#include <preprocessing/dem/Shop.hpp>
#endif

namespace yade { // Cannot have #include directive inside.

using math::max;
using math::min; // using inside .cpp file is ok.

#ifdef YADE_OPENGL
YADE_PLUGIN((Gl1_Polyhedra)(Gl1_PolyhedraGeom)(Gl1_PolyhedraPhys));
#endif

//****************************************************************************************
/* Destructor */

Polyhedra::~Polyhedra() { }

//****************************************************************************************
Real PolyhedraMat::GetStrength() const { return strength; };
Real PolyhedraMat::GetStrengthTau() const { return strengthTau; };
Real PolyhedraMat::GetStrengthSigmaCZ() const { return sigmaCZ; };
Real PolyhedraMat::GetStrengthSigmaCD() const { return sigmaCD; };
int  PolyhedraMat::GetWeiM() const { return Wei_m; };
Real PolyhedraMat::GetWeiS0() const { return Wei_S0; };
Real PolyhedraMat::GetWeiV0() const { return Wei_V0; };
Real PolyhedraMat::GetP() const { return Wei_P; };

//****************************************************************************************
/* Destructor */

PolyhedraGeom::~PolyhedraGeom() { }

//****************************************************************************************
/* AaBb overlap checker  */

void Bo1_Polyhedra_Aabb::go(const shared_ptr<Shape>& ig, shared_ptr<Bound>& bv, const Se3r& se3, const Body*)
{
	Polyhedra* t = static_cast<Polyhedra*>(ig.get());
	if (!t->IsInitialized()) t->Initialize();
	if (!bv) { bv = shared_ptr<Bound>(new Aabb); }
	Aabb* aabb = static_cast<Aabb*>(bv.get());
	//Quaternionr invRot=se3.orientation.conjugate();
	int      N = (int)t->v.size();
	Vector3r v_g, mincoords(0., 0., 0.), maxcoords(0., 0., 0.);
	for (int i = 0; i < N; i++) {
		v_g       = se3.orientation * t->v[i]; // vertices in global coordinates
		mincoords = Vector3r(min(mincoords[0], v_g[0]), min(mincoords[1], v_g[1]), min(mincoords[2], v_g[2]));
		maxcoords = Vector3r(max(maxcoords[0], v_g[0]), max(maxcoords[1], v_g[1]), max(maxcoords[2], v_g[2]));
	}
	if (aabbEnlargeFactor > 0) {
		mincoords *= aabbEnlargeFactor;
		maxcoords *= aabbEnlargeFactor;
	}
	aabb->min = se3.position + mincoords;
	aabb->max = se3.position + maxcoords;
}

//**********************************************************************************
/* Plotting */

#ifdef YADE_OPENGL
bool Gl1_Polyhedra::wire;

void Gl1_Polyhedra::go(const shared_ptr<Shape>& cm, const shared_ptr<State>&, bool wire2, const GLViewInfo&)
{
	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Vector3r(cm->color[0], cm->color[1], cm->color[2]));
	glColor3v(cm->color);
	Polyhedra*  t       = static_cast<Polyhedra*>(cm.get());
	vector<int> faceTri = t->GetSurfaceTriangulation();

	if (wire || wire2) {
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES)
			;
			for (int tri = 0; tri < (int)faceTri.size(); tri += 3) {
				glOneWire(t, faceTri[tri], faceTri[tri + 1]);
				glOneWire(t, faceTri[tri], faceTri[tri + 2]);
				glOneWire(t, faceTri[tri + 1], faceTri[tri + 2]);
			}
		glEnd();
	} else {
		Vector3r centroid = t->GetCentroid();
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glBegin(GL_TRIANGLES)
			;
			for (int tri = 0; tri < (int)faceTri.size(); tri += 3) {
				const auto a = faceTri[tri + 0];
				const auto b = faceTri[tri + 1];
				const auto c = faceTri[tri + 2];

				Vector3r n = (t->v[b] - t->v[a]).cross(t->v[c] - t->v[a]);
				n.normalize();
				Vector3r faceCenter = (t->v[a] + t->v[b] + t->v[c]) / 3.;
				if ((faceCenter - centroid).dot(n) < 0) n = -n;

				glNormal3v(n);
				glVertex3v(t->v[a]);
				glVertex3v(t->v[b]);
				glVertex3v(t->v[c]);
			}
		glEnd();
	}
}

void Gl1_PolyhedraGeom::go(const shared_ptr<IGeom>& ig, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool) { draw(ig); }

void Gl1_PolyhedraGeom::draw(const shared_ptr<IGeom>& /*ig*/) {};

GLUquadric* Gl1_PolyhedraPhys::gluQuadric = NULL;
Real        Gl1_PolyhedraPhys::maxFn;
Real        Gl1_PolyhedraPhys::refRadius;
Real        Gl1_PolyhedraPhys::maxRadius;
int         Gl1_PolyhedraPhys::signFilter;
int         Gl1_PolyhedraPhys::slices;
int         Gl1_PolyhedraPhys::stacks;

void Gl1_PolyhedraPhys::go(
        const shared_ptr<IPhys>& ip, const shared_ptr<Interaction>& i, const shared_ptr<Body>& b1, const shared_ptr<Body>& b2, bool /*wireFrame*/)
{
	if (!gluQuadric) {
		gluQuadric = gluNewQuadric();
		if (!gluQuadric) throw runtime_error("Gl1_PolyhedraPhys::go unable to allocate new GLUquadric object (out of memory?).");
	}
	PolyhedraPhys*    np = static_cast<PolyhedraPhys*>(ip.get());
	shared_ptr<IGeom> ig(i->geom);
	if (!ig) return; // changed meanwhile?
	PolyhedraGeom* geom   = YADE_CAST<PolyhedraGeom*>(ig.get());
	Real           fnNorm = np->normalForce.dot(geom->normal);
	if ((signFilter > 0 && fnNorm < 0) || (signFilter < 0 && fnNorm > 0)) return;
	int fnSign       = fnNorm > 0 ? 1 : -1;
	fnNorm           = math::abs(fnNorm);
	Real radiusScale = 1.;
	maxFn            = max(fnNorm, maxFn);
	Real realMaxRadius;
	if (maxRadius < 0) {
		refRadius     = min(0.03, refRadius);
		realMaxRadius = refRadius;
	} else
		realMaxRadius = maxRadius;
	Real radius = radiusScale * realMaxRadius * (fnNorm / maxFn);
	if (radius <= 0.) radius = 1E-8;
	Vector3r color = Shop::scalarOnColorScale(fnNorm * fnSign, -maxFn, maxFn);

	Vector3r p1 = b1->state->pos, p2 = b2->state->pos;
	Vector3r relPos;
	relPos    = p2 - p1;
	Real dist = relPos.norm();

	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glTranslate(p1[0], p1[1], p1[2]);
	Quaternionr q(Quaternionr().setFromTwoVectors(Vector3r(0, 0, 1), relPos / dist /* normalized */));
	// using Transform with OpenGL: http://eigen.tuxfamily.org/dox/TutorialGeometry.html
	//glMultMatrixd(Eigen::Affine3d(q).data());
	glMultMatrix(Eigen::Transform<Real, 3, Eigen::Affine>(q).data());
	glColor3v(color);
	gluCylinder(gluQuadric, radius, radius, dist, slices, stacks);
	glPopMatrix();
}
#endif

//**********************************************************************************
//!Precompute data needed for rotating tangent vectors attached to the interaction

void PolyhedraGeom::precompute(
        const State& rbp1,
        const State& rbp2,
        const Scene* scene,
        const shared_ptr<Interaction>& /*c*/,
        const Vector3r& currentNormal,
        bool            isNew,
        const Vector3r& shift2)
{
	if (!isNew) {
		orthonormal_axis = normal.cross(currentNormal);
		Real angle       = scene->dt * 0.5 * normal.dot(rbp1.angVel + rbp2.angVel);
		twist_axis       = angle * normal;
	} else
		twist_axis = orthonormal_axis = Vector3r::Zero();
	//Update contact normal
	normal = currentNormal;
	//Precompute shear increment
	Vector3r c1x              = (contactPoint - rbp1.pos);
	Vector3r c2x              = (contactPoint - (rbp2.pos + shift2));
	Vector3r relativeVelocity = (rbp2.vel + rbp2.angVel.cross(c2x)) - (rbp1.vel + rbp1.angVel.cross(c1x));
	//keep the shear part only
	relativeVelocity = relativeVelocity - normal.dot(relativeVelocity) * normal;
	shearInc         = relativeVelocity * scene->dt;
}

//**********************************************************************************
Vector3r& PolyhedraGeom::rotate(Vector3r& shearForce) const
{
	// approximated rotations
	shearForce -= shearForce.cross(orthonormal_axis);
	shearForce -= shearForce.cross(twist_axis);
	//NOTE : make sure it is in the tangent plane? It's never been done before. Is it not adding rounding errors at the same time in fact?...
	shearForce -= normal.dot(shearForce) * normal;
	return shearForce;
}

//**********************************************************************************
/* Material law, physics */

void Ip2_PolyhedraMat_PolyhedraMat_PolyhedraPhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction)
{
	if (interaction->phys) return;
	const shared_ptr<PolyhedraMat>& mat1            = YADE_PTR_CAST<PolyhedraMat>(b1);
	const shared_ptr<PolyhedraMat>& mat2            = YADE_PTR_CAST<PolyhedraMat>(b2);
	interaction->phys                               = shared_ptr<PolyhedraPhys>(new PolyhedraPhys());
	const shared_ptr<PolyhedraPhys>& contactPhysics = YADE_PTR_CAST<PolyhedraPhys>(interaction->phys);
	Real                             Kna            = mat1->young;
	Real                             Knb            = mat2->young;
	Real                             Ksa            = mat1->young * mat1->poisson;
	Real                             Ksb            = mat2->young * mat2->poisson;
	Real                             frictionAngle  = math::min(mat1->frictionAngle, mat2->frictionAngle);
	contactPhysics->tangensOfFrictionAngle          = math::tan(frictionAngle);
	contactPhysics->kn                              = Kna * Knb / (Kna + Knb);
	contactPhysics->ks                              = Ksa * Ksb / (Ksa + Ksb);
};

void Ip2_FrictMat_PolyhedraMat_FrictPhys::go(const shared_ptr<Material>& pp1, const shared_ptr<Material>& pp2, const shared_ptr<Interaction>& interaction)
{
	const shared_ptr<FrictMat>&     mat1 = YADE_PTR_CAST<FrictMat>(pp1);
	const shared_ptr<PolyhedraMat>& mat2 = YADE_PTR_CAST<PolyhedraMat>(pp2);
	Ip2_FrictMat_FrictMat_FrictPhys().go(mat1, mat2, interaction);
}

//**************************************************************************************
Real Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::getPlasticDissipation() const { return (Real)plasticDissipation; }
void Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::initPlasticDissipation(Real initVal)
{
	plasticDissipation.reset();
	plasticDissipation += initVal;
}
Real Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::elasticEnergy()
{
	Real energy = 0;
	FOREACH(const shared_ptr<Interaction>& I, *scene->interactions)
	{
		if (!I->isReal()) continue;
		FrictPhys* phys = dynamic_cast<FrictPhys*>(I->phys.get());
		if (phys) { energy += 0.5 * (phys->normalForce.squaredNorm() / phys->kn + phys->shearForce.squaredNorm() / phys->ks); }
	}
	return energy;
}

//**************************************************************************************
// Apply forces on polyhedrons in collision based on geometric configuration
bool Law2_PolyhedraGeom_PolyhedraPhys_Volumetric::go(shared_ptr<IGeom>& /*ig*/, shared_ptr<IPhys>& /*ip*/, Interaction* I)
{
	if (!I->geom) { return true; }
	const shared_ptr<PolyhedraGeom>& contactGeom(YADE_PTR_DYN_CAST<PolyhedraGeom>(I->geom));
	if (!contactGeom) { return true; }
	const Body::id_t       idA = I->getId1(), idB = I->getId2();
	const shared_ptr<Body>&A = Body::byId(idA), B = Body::byId(idB);

	PolyhedraPhys* phys = dynamic_cast<PolyhedraPhys*>(I->phys.get());

	//erase the interaction when aAbB shows separation, otherwise keep it to be able to store previous separating plane for fast detection of separation
	Vector3r shift2 = scene->cell->hSize * I->cellDist.cast<Real>();
	if (A->bound->min[0] >= B->bound->max[0] + shift2[0] || B->bound->min[0] + shift2[0] >= A->bound->max[0]
	    || A->bound->min[1] >= B->bound->max[1] + shift2[1] || B->bound->min[1] + shift2[1] >= A->bound->max[1]
	    || A->bound->min[2] >= B->bound->max[2] + shift2[2] || B->bound->min[2] + shift2[2] >= A->bound->max[2]) {
		return false;
	}

	//zero penetration depth means no interaction force
	if (!(contactGeom->equivalentPenetrationDepth > 1E-18) || !(contactGeom->penetrationVolume > 0)) {
		phys->normalForce = Vector3r(0., 0., 0.);
		phys->shearForce  = Vector3r(0., 0., 0.);
		return true;
	}

	Real     prop        = math::pow(contactGeom->penetrationVolume, volumePower);
	Vector3r normalForce = contactGeom->normal * prop * phys->kn;

	//shear force: in case the polyhdras are separated and come to contact again, one
	//should not use the previous shear force
	if (contactGeom->isShearNew) shearForce = Vector3r::Zero();
	else
		shearForce = contactGeom->rotate(shearForce);

	const Vector3r& shearDisp = contactGeom->shearInc;
	shearForce -= phys->ks * shearDisp;
	const Real maxFs = normalForce.squaredNorm() * math::pow(phys->tangensOfFrictionAngle, 2);

	if (shearForce.squaredNorm() > maxFs && maxFs) {
		//PFC3d SlipModel, is using friction angle. CoulombCriterion
		Real ratio = sqrt(maxFs) / shearForce.norm();
		if (math::isinf(ratio)) {
			LOG_DEBUG(
			        "shearForce.squaredNorm() > maxFs && maxFs: "
			        << (shearForce.squaredNorm() > maxFs && maxFs)); // the condition should be 1 (we are in this branch), but is actually 0
			LOG_DEBUG("shearForce: " << shearForce);                 // should be (0,0,0)
			ratio = 0;
		}

		//Store prev force for definition of plastic slip
		//Define the plastic work input and increment the total plastic energy dissipated
		const Vector3r trialForce = shearForce;
		shearForce *= ratio;

		if (scene->trackEnergy && traceEnergy) {
			const Real dissip = ((1 / phys->ks) * (trialForce - shearForce)).dot(shearForce);

			if (traceEnergy) plasticDissipation += dissip;
			else if (dissip > 0)
				scene->energy->add(dissip, "plastDissip", plastDissipIx, false);

			// compute elastic energy as well
			scene->energy->add(
			        0.5 * (normalForce.squaredNorm() / phys->kn + shearForce.squaredNorm() / phys->ks), "elastPotential", elastPotentialIx, true);
		}
	} else {
		if (maxFs == 0) shearForce = Vector3r::Zero();
		scene->energy->add(
		        0.5 * (normalForce.squaredNorm() / phys->kn + shearForce.squaredNorm() / phys->ks), "elastPotential", elastPotentialIx, true);
	}
	Vector3r F = -normalForce - shearForce;
	if (contactGeom->equivalentPenetrationDepth != contactGeom->equivalentPenetrationDepth) exit(1);

	scene->forces.addForce(idA, F);
	scene->forces.addForce(idB, -F);
	scene->forces.addTorque(idA, -(A->state->pos - contactGeom->contactPoint).cross(F));
	scene->forces.addTorque(idB, (B->state->pos - contactGeom->contactPoint).cross(F));

	/*
		FILE * fin = fopen("Forces.dat","a");
		fprintf(fin,"************** IDS %d %d **************\n",idA, idB);
		Vector3r T = (B->state->pos-contactGeom->contactPoint).cross(F);
		fprintf(fin,"volume\t%e\n",contactGeom->penetrationVolume);	
		fprintf(fin,"normal_force\t%e\t%e\t%e\n",normalForce[0],normalForce[1],normalForce[2]);	
		fprintf(fin,"shear_force\t%e\t%e\t%e\n",shearForce[0],shearForce[1],shearForce[2]);	
		fprintf(fin,"total_force\t%e\t%e\t%e\n",F[0],F[1],F[2]);		
		fprintf(fin,"torsion\t%e\t%e\t%e\n",T[0],T[1],T[2]);
		fprintf(fin,"A\t%e\t%e\t%e\n",A->state->pos[0],A->state->pos[1],A->state->pos[2]);
		fprintf(fin,"B\t%e\t%e\t%e\n",B->state->pos[0],B->state->pos[1],B->state->pos[2]);
		fprintf(fin,"centroid\t%e\t%e\t%e\n",contactGeom->contactPoint[0],contactGeom->contactPoint[1],contactGeom->contactPoint[2]);
		fclose(fin);
		*/
	//needed to be able to acces interaction forces in other parts of yade
	phys->normalForce = normalForce;
	phys->shearForce  = shearForce;
	return true;
}

} // namespace yade

#endif // YADE_CGAL

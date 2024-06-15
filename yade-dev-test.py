from yade import polyhedra_utils, O
gravel = PolyhedraMat(density = 2600, young = 1E7, poisson = 20000 / 1E7, frictionAngle = 0.5)

list1_polyhedron = [
    [0, 0, 0],
    [1, 0, 0],
    [1, 1, 0],
    [0, 1, 0],
    [0, 0, 1],
    [1, 0, 1],
    [1, 1, 1],
    [0, 1, 1]]

list2_polyhedron = [
    [0.5, 0.5, 1.0],
    [0.5, 0.5, 0.0],
    [0.0, 0.5, 0.5],
    [1.0, 0.5, 0.5],
    [0.5, 1.0, 0.5],
    [0.5, 0.0, 0.5]]

list3_polyhedron = [
    [-0.5, 0.0, 0.809], [0.5, 0.0, 0.809], [0.5, 0.0, -0.809], [-0.5, 0.0, -0.809],
    [0.0, 0.809, 0.5], [0.0, 0.809, -0.5], [0.0, -0.809, 0.5], [0.0, -0.809, -0.5],
    [0.809, 0.5, 0.0], [0.809, -0.5, 0.0], [-0.809, 0.5, 0.0], [-0.809, -0.5, 0.0]]
    
polyhedra_utils.fillHull(list1_polyhedron, gravel, sizemin=[0.25, 0.25, 0.25], sizemax=[0.5, 0.5, 0.5], seed=1)
polyhedra_utils.fillHull(list2_polyhedron, gravel, sizemin=[0.025, 0.025, 0.025], sizemax=[0.05, 0.05, 0.05], seed=2)
polyhedra_utils.fillHull(list3_polyhedron, gravel, sizemin=[0.05, 0.05, 0.05], sizemax=[0.2, 0.2, 0.3], seed=3)

O.bodies.clear()


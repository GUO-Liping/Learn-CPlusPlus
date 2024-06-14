from yade import polyhedra_utils
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
    
polyhedra_utils.fillHull(list2_polyhedron, gravel, sizemin=[0.025, 0.025, 0.025], sizemax=[0.05, 0.05, 0.05], seed=4)



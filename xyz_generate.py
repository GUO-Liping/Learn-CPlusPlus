import numpy as np
import random

# 顶点坐标
p1_xyz = np.array([0,0,0])
p2_xyz = np.array([1,0,0])
p3_xyz = np.array([1,1,0])
p4_xyz = np.array([0,1,0])
p5_xyz = np.array([0,0,1])
p6_xyz = np.array([1,0,1])
p7_xyz = np.array([1,1,1])
p8_xyz = np.array([0,1,1])

p_all_xyz = np.vstack((p1_xyz, p2_xyz, p3_xyz, p4_xyz, p5_xyz, p6_xyz, p7_xyz, p8_xyz))

# 坐标最大范围
x_min = np.min(p_all_xyz[:,0])
x_max = np.max(p_all_xyz[:,0])
y_min = np.min(p_all_xyz[:,1])
y_max = np.max(p_all_xyz[:,1])
z_min = np.min(p_all_xyz[:,2])
z_max = np.max(p_all_xyz[:,2])

# 平面内一点坐标
pc1_xyz = p1_xyz
pc2_xyz = p1_xyz
pc3_xyz = p1_xyz
pc4_xyz = p7_xyz
pc5_xyz = p7_xyz
pc6_xyz = p7_xyz

# 平面法向量——指向多面体外部
nc1_xyz = -np.cross(p2_xyz - p1_xyz, p4_xyz-p1_xyz)
nc2_xyz = -np.cross(p4_xyz - p1_xyz, p5_xyz-p1_xyz)
nc3_xyz = -np.cross(p5_xyz - p1_xyz, p2_xyz-p1_xyz)
nc4_xyz = np.cross(p8_xyz - p7_xyz, p6_xyz-p7_xyz)
nc5_xyz = np.cross(p3_xyz - p7_xyz, p8_xyz-p7_xyz)
nc6_xyz = np.cross(p6_xyz - p7_xyz, p3_xyz-p7_xyz)

# 平面方程
def func_plane(P_xyz, P0_xyz, normal_v):
	x = P_xyz[0]
	y = P_xyz[1]
	z = P_xyz[2]

	x0 = P0_xyz[0]
	y0 = P0_xyz[1]
	z0 = P0_xyz[2]

	nx = normal_v[0]
	ny = normal_v[1]
	nz = normal_v[2]

	return nx*(x-x0) + ny*(y-y0) + nz*(z-z0)

for i in range(100):
	x = x_min + random.random() * (x_max-x_min)
	y = y_min + random.random() * (y_max-y_min)
	z = z_min + random.random() * (z_max-z_min)

	p_xyz = np.array([x, y, z])

	plane1 = func_plane(p_xyz, pc1_xyz, nc1_xyz)
	plane2 = func_plane(p_xyz, pc2_xyz, nc2_xyz)
	plane3 = func_plane(p_xyz, pc3_xyz, nc3_xyz)
	plane4 = func_plane(p_xyz, pc4_xyz, nc4_xyz)
	plane5 = func_plane(p_xyz, pc5_xyz, nc5_xyz)
	plane6 = func_plane(p_xyz, pc6_xyz, nc6_xyz)

	if plane1 < 0 and plane2 < 0 and plane3 < 0 and plane4 < 0 and plane5 < 0 and plane6 < 0:
		print("x=", x, "y=", y, "z=", z)
		break
	else:
		print("没有生成满足条件的点")


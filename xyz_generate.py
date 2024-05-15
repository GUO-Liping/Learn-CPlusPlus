import numpy as np
import random

# 平面点坐标
p1_xyz = np.array([p1_x, p1_y, p1_z])
p2_xyz = np.array([p2_x, p2_y, p2_z])
p3_xyz = np.array([p3_x, p3_y, p3_z])
p4_xyz = np.array([p4_x, p4_y, p4_z])
p5_xyz = np.array([p5_x, p5_y, p5_z])
p6_xyz = np.array([p6_x, p6_y, p6_z])

# 平面法向量——指向多面体外部
n1_xyz = np.array([n1_x, n1_y, n1_z])
n2_xyz = np.array([n2_x, n2_y, n2_z])
n3_xyz = np.array([n3_x, n3_y, n3_z])
n4_xyz = np.array([n4_x, n4_y, n4_z])
n5_xyz = np.array([n5_x, n5_y, n5_z])
n6_xyz = np.array([n6_x, n6_y, n6_z])

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

x_min = 0
x_max = 1
y_min = 0
y_max = 1
z_min = 0
z_max = 1

for i in range(100):
    x = x_min + random.random() * (x_max-x_min)
    y = y_min + random.random() * (y_max-y_min)
    z = z_min + random.random() * (z_max-z_min)

    p_xyz = np.array([x, y, z])

	plane1 = func_plane(p_xyz, p1_xyz, n1_xyz)
	plane2 = func_plane(p_xyz, p2_xyz, n2_xyz)
	plane3 = func_plane(p_xyz, p3_xyz, n3_xyz)
	plane4 = func_plane(p_xyz, p4_xyz, n4_xyz)
	plane5 = func_plane(p_xyz, p5_xyz, n5_xyz)
	plane6 = func_plane(p_xyz, p6_xyz, n6_xyz)
       
    if plane1 < 0 and plane2 < 0 and plane3 < 0 and plane4 < 0 and plane5 < 0 and plane6 < 0:
        print("x=", x, "y=", y, "z=", z)
        break
    else:
        print("没有生成满足条件的点")


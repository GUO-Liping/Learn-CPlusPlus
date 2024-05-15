import numpy as np
import random

p1 = np.array([x1, y1, z1])
p2 = np.array([x2, y2, z2])
p3 = np.array([x3, y3, z3])
p4 = np.array([x4, y4, z4])
p5 = np.array([x5, y5, z5])
p6 = np.array([x6, y6, z6])
p7 = np.array([x7, y7, z7])
p8 = np.array([x8, y8, z8])

fp1 = np.array([xp1, yp1, zp1])
fp2 = np.array([xp2, yp2, zp2])
fp3 = np.array([xp3, yp3, zp3])
fp4 = np.array([xp4, yp4, zp4])
fp5 = np.array([xp5, yp5, zp5])
fp6 = np.array([xp6, yp6, zp6])

fn1 = np.array([xn1, yn1, zn1])
fn2 = np.array([xn2, yn2, zn2])
fn3 = np.array([xn3, yn3, zn3])
fn4 = np.array([xn4, yn4, zn4])
fn5 = np.array([xn5, yn5, zn5])
fn6 = np.array([xn6, yn6, zn6])

def func_plane1(x, y, z):
    return fn1[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)

def func_plane2(x, y, z):
    return fn2[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)

def func_plane1(x, y, z):
    return fn1[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)

def func_plane1(x, y, z):
    return fn1[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)

def func_plane1(x, y, z):
    return fn1[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)

def func_plane1(x, y, z):
    return fn1[0] * (x-x0) + fn1[1] * (y-y0) + fn1[2]*(z-z0)
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
        
    if x + y +z < 1 and x>0 and y>0 and z>0:
        print("x=", x, "y=", y, "z=", z)
        break
    else:
        print("没有生成满足条件的点")


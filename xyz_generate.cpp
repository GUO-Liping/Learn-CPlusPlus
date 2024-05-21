#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <random>
#include <algorithm>

using namespace Eigen;
using namespace std;

typedef Vector3d Vector3r;
typedef Matrix<double, 8, 3> Matrix8x3;

Vector3r cross_product(const Vector3r& v1, const Vector3r& v2) {
    return v1.cross(v2);
}

double func_plane(const Vector3r& P_xyz, const Vector3r& P0_xyz, const Vector3r& normal_v) {
    return normal_v.dot(P_xyz - P0_xyz);
}

int main() {
    // 顶点坐标
    Vector3r p1_xyz(0, 0, 0);
    Vector3r p2_xyz(1, 0, 0);
    Vector3r p3_xyz(1, 1, 0);
    Vector3r p4_xyz(0, 1, 0);
    Vector3r p5_xyz(0, 0, 1);
    Vector3r p6_xyz(1, 0, 1);
    Vector3r p7_xyz(1, 1, 1);
    Vector3r p8_xyz(0, 1, 1);

    Matrix8x3 p_all_xyz;
    p_all_xyz << p1_xyz.transpose(), p2_xyz.transpose(), p3_xyz.transpose(), p4_xyz.transpose(),
                 p5_xyz.transpose(), p6_xyz.transpose(), p7_xyz.transpose(), p8_xyz.transpose();

    // 坐标最大范围
    double x_min = p_all_xyz.col(0).minCoeff();
    double x_max = p_all_xyz.col(0).maxCoeff();
    double y_min = p_all_xyz.col(1).minCoeff();
    double y_max = p_all_xyz.col(1).maxCoeff();
    double z_min = p_all_xyz.col(2).minCoeff();
    double z_max = p_all_xyz.col(2).maxCoeff();

    // 法平面内一点坐标,每三个面有一个交点P1与P7
    Vector3r pc1_xyz = p1_xyz;
    Vector3r pc2_xyz = p1_xyz;
    Vector3r pc3_xyz = p1_xyz;
    Vector3r pc4_xyz = p7_xyz;
    Vector3r pc5_xyz = p7_xyz;
    Vector3r pc6_xyz = p7_xyz;

    // 平面法向量——指向多面体外部
    Vector3r nc1_xyz = -cross_product(p2_xyz - p1_xyz, p4_xyz - p1_xyz);
    Vector3r nc2_xyz = -cross_product(p4_xyz - p1_xyz, p5_xyz - p1_xyz);
    Vector3r nc3_xyz = -cross_product(p5_xyz - p1_xyz, p2_xyz - p1_xyz);
    Vector3r nc4_xyz = cross_product(p8_xyz - p7_xyz, p6_xyz - p7_xyz);
    Vector3r nc5_xyz = cross_product(p3_xyz - p7_xyz, p8_xyz - p7_xyz);
    Vector3r nc6_xyz = cross_product(p6_xyz - p7_xyz, p3_xyz - p7_xyz);

    // 随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < 100; ++i) {
        double x = x_min + dis(gen) * (x_max - x_min);
        double y = y_min + dis(gen) * (y_max - y_min);
        double z = z_min + dis(gen) * (z_max - z_min);

        Vector3r p_xyz(x, y, z);

        double plane1 = func_plane(p_xyz, pc1_xyz, nc1_xyz);
        double plane2 = func_plane(p_xyz, pc2_xyz, nc2_xyz);
        double plane3 = func_plane(p_xyz, pc3_xyz, nc3_xyz);
        double plane4 = func_plane(p_xyz, pc4_xyz, nc4_xyz);
        double plane5 = func_plane(p_xyz, pc5_xyz, nc5_xyz);
        double plane6 = func_plane(p_xyz, pc6_xyz, nc6_xyz);

        if (plane1 < 0 && plane2 < 0 && plane3 < 0 && plane4 < 0 && plane5 < 0 && plane6 < 0) {
            cout << "x = " << x << " y = " << y << " z = " << z << endl;
            break;
        } else {
            cout << "没有生成满足条件的点" << endl;
        }
    }

    return 0;
}

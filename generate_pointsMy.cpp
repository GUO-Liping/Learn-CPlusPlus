#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// 生成 0 到 1 间的随机浮点数
double random01() {
    return static_cast<double>(rand()) / RAND_MAX;
}

// 生成 min 到 max 之间的随机浮点数
double randomInRange(double min, double max) {
    return min + (max - min) * random01();
}

struct Point {
    double x, y, z;
};

// 计算两个向量的叉积
Point crossProduct(const Point& a, const Point& b) {
    Point result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

// 计算两个向量的点积
double dotProduct(const Point& a, const Point& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 生成一个随机点，该点位于指定的六面体内
Point randomPointInHexahedron(const Point& p1, const Point& p2,
                               const Point& p3, const Point& p4,
                               const Point& p5, const Point& p6) {
    // 生成六个平面方程
    Point normal1 = crossProduct({p2.x - p1.x, p2.y - p1.y, p2.z - p1.z},
                                 {p3.x - p1.x, p3.y - p1.y, p3.z - p1.z});
    Point normal2 = crossProduct({p3.x - p2.x, p3.y - p2.y, p3.z - p2.z},
                                 {p4.x - p2.x, p4.y - p2.y, p4.z - p2.z});
    Point normal3 = crossProduct({p4.x - p3.x, p4.y - p3.y, p4.z - p3.z},
                                 {p1.x - p3.x, p1.y - p3.y, p1.z - p3.z});
    Point normal4 = crossProduct({p1.x - p4.x, p1.y - p4.y, p1.z - p4.z},
                                 {p5.x - p4.x, p5.y - p4.y, p5.z - p4.z});
    Point normal5 = crossProduct({p2.x - p1.x, p2.y - p1.y, p2.z - p1.z},
                                 {p5.x - p1.x, p5.y - p1.y, p5.z - p1.z});
    Point normal6 = crossProduct({p3.x - p2.x, p3.y - p2.y, p3.z - p2.z},
                                 {p6.x - p2.x, p6.y - p2.y, p6.z - p2.z});

    // 生成随机点在六面体内部

    
    
    Point random_point;
    random_point.x = p1.x + s * (p2.x - p1.x) + t * (p3.x - p1.x) + u * (p4.x - p1.x) + v * (p5.x - p1.x) + w * (p6.x - p1.x);
    random_point.y = p1.y + s * (p2.y - p1.y) + t * (p3.y - p1.y) + u * (p4.y - p1.y) + v * (p5.y - p1.y) + w * (p6.y - p1.y);
    random_point.z = p1.z + s * (p2.z - p1.z) + t * (p3.z - p1.z) + u * (p4.z - p1.z) + v * (p5.z - p1.z) + w * (p6.z - p1.z);

    return random_point;
}

int main() {
    // 设置随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));

    // 定义六面体的六个顶点
    Point p1 = {1.0, 2.0, 3.0};
    Point p2 = {4.0, 2.5, 2.0};
    Point p3 = {3.0, 6.0, 4.0};
    Point p4 = {0.5, 5.0, 3.5};
    Point p5 = {2.0, 2.0, 0.0};
    Point p6 = {4.0, 4.0, 1.0};

    // 生成一个随机点
    Point random_point = randomPointInHexahedron(p1, p2, p3, p4, p5, p6);

    // 输出随机点的坐标
    std::cout << "Random Point: (" << random_point.x << ", " << random_point.y << ", " << random_point.z << ")\n";

    return 0;
}

#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
    // void print() const { 这一行代码定义了一个常量成员函数 print，它用于打印 Point 对象的成员变量 x 和 y，并且保证在调用这个函数时不会修改对象的状态
    // 关键字 const 表示这个成员函数是一个常量成员函数
    void print() const {
        cout << "Point(" << x << ", " << y << ")" << endl;
    }
};

int main() {
    Point p = {1, 2}; // 创建一个 Point 对象
    Point* ptr = &p; // 创建一个指向 Point 对象的指针
    Point& ref = p; // 创建一个 Point 对象的引用

    // 使用指针和引用修改 Point 对象的成员
    ptr->x = 10; // 通过指针修改 x 成员
    ref.y = 20; // 通过引用修改 y 成员

    // 打印结果
    p.print(); // 输出 Point(10, 20)

    return 0;
}

#include <iostream>
#include "Circle.h"
#include "Circle.cpp"
#include <windows.h>  //解决打开Circle_main.exe中文乱码的问题

using namespace std;

int main()
{
  SetConsoleOutputCP(65001);  //解决打开Circle_main.exe中文乱码的问题
  double radius;
  double density;
  cout << "请输入半径的值，单位：m，数据类型：double： " ;
  cin >> radius;
  cout << "请输入密度的值，单位kg/m2，数据类型：double： ";
  cin >> density;
  Circle c(radius, density);
  cout << "Perimeter of the Circle = " << c.Perimeter() << endl;
  cout << "Area of the Circle = " << c.Area() << endl;
  cout << "Mass of the Circle = " << c.Mass() << endl;
  system("pause");
  return 1;
}

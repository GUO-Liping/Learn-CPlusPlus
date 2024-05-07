# ifndef CIRCLE_H
# define CIRCLE_H

// code
class Circle
{
  private:
    double radius;
    double density;
  public:
    Circle(double r, double d); //构造函数
    double Perimeter();
    double Area();
    double Mass();
};

# endif
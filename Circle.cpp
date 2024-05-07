# include "Circle.h"
# include <cmath>

Circle::Circle(double r, double d)
{
  radius = r;
  density = d;
}

double Circle::Perimeter()
{
  return M_PI*2*radius;
}

double Circle::Area()
{
  return M_PI*radius*radius;
}

double Circle::Mass()
{
  return M_PI*radius*radius*density;
}
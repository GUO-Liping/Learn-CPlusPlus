# include <iostream>
# include <Eigen/Dense>
# include <stdlib.h>

using namespace std;
using namespace Eigen;
 
int main()
{
  MatrixXd m0(2,2);
  m0(0,0) = 3;
  m0(1,0) = 2.5;
  m0(0,1) = -1;
  m0(1,1) = m0(1,0) + m0(0,1);

  MatrixXd m1 = MatrixXd::Zero(2,2);
  MatrixXd m2 = MatrixXd::Identity(2,2);
  MatrixXd m3 = MatrixXd::Ones(3,3);
  MatrixXd m4 = MatrixXd::Random(3,3);
  MatrixXd m5 = MatrixXd::Constant(3,3,3.5);
  MatrixXd m6 = Matrix3d::Constant(3.6);

  MatrixXd mat_add = m0 + m1;
  MatrixXd mat_min = m2 - m1;
  MatrixXd mat_mul_num = m3 * 2;
  MatrixXd mat_div_num = m3 / 2;
  MatrixXd mat_mul = m3 * m4;
  MatrixXd mat_rev = m2.inverse();


  cout << "m0+m1=" << endl << m0+m1 << endl;
  cout << "m2-m1=" << endl << m2-m1 << endl;
  cout << "m3*2=" << endl << m3*2 << endl;
  cout << "m3/2=" << endl << m3/2 << endl;
  cout << "m3*m4=" << endl << m3*m4 << endl;
  cout << "m4+m5=" << endl << m4+m5 << endl;
  cout << "m6=" << endl << m6 << endl;

  MatrixXd Mat1(3,3);
  Mat1 << 1,0,0,0,2,0,0,0,3;
  Mat1*Mat1.inverse();
  cout << "Mat1=\n" << Mat1 << endl;
  cout << "Mat1转置矩阵: \n" << Mat1.transpose() << endl;
  cout << "Mat1伴随矩阵: \n" << Mat1 .adjoint() << endl;
  cout << "Mat1逆矩阵: \n" << Mat1.inverse() << endl;
  cout << "Mat1与Mat1逆矩阵乘积： \n" << Mat1*Mat1.inverse() << endl;
  cout << "Mat1行列式: \n" << Mat1.determinant() << endl;
  SelfAdjointEigenSolver<Matrix3d>eigensover(Mat1);
  if (eigensover.info() != Success) abort();
  cout << "Mat1特征值: \n" << eigensover.eigenvalues() << endl;
  cout << "Mat1特征向量: \n" << eigensover. eigenvectors() << endl;

  // system("pause");

}
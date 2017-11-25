#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>
using Eigen::MatrixXd;
int main(){
  int depth=2, rows=3, cols=3;
  //三次元配列作りたい時
  MatrixXd *Sigma;
  Sigma=new MatrixXd[depth];
  for(int i=0;i<depth;i++){
    Sigma[i].resize(rows,cols);
    MatrixXd rand=MatrixXd::Random(rows,cols);
    Sigma[i]=rand;
    std::cout<<Sigma[i]<<std::endl;
  }
  //逆行列
  MatrixXd Sigma_inv=Sigma[0].inverse();
  std::cout<<"inverse:\n"<<Sigma_inv<<std::endl;
  //行列式
  double det=Sigma[1].determinant();
  std::cout<<"determinant:\n"<<det<<std::endl;
  delete[] Sigma;
}


#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/LU>

using namespace Eigen;

#ifndef _GMM_H
#define _GMM_H

class GMM{
 private:
  MatrixXd Data, Mu;
  MatrixXd LargeP, SmallP, Tmp_SmallP;
  MatrixXd *Sigma;
  MatrixXi Crisp, Correct;
  MatrixXi CTable;
  VectorXi InitializeC;
  VectorXd Pi;
 public:
  //コンストラクタ
  GMM(int ,int, int);
  //デストラクタ
  ~GMM();
  //データ点の数を返す
  int data_number();
  //データの次元数を返す
  int dimension();
  //混合数を返す
  int centers_number();
  //データ読み込み
  void input_data(const char *);
  //正解読み込み
  void input_correct(const char *);
  //出力
  void output(int);
  //分類結果表
  void set_ctable();
  //精度
  void precision();
  //再現率
  void recall();
  //F-measure
  void fmeasure();
  //ユークリッド距離の計算
  bool dissimilarities(int, MatrixXd &);
  //k-means++っぽく平均初期化
  void initialize_mu(int);
  //混合率初期化
  void initialize_pi();
  //分散共分散行列初期化
  void initialize_sigma();
  //Mステップ
  //混合率更新式
  void revise_pi();
  //平均更新式
  void revise_mu();
  //分散共分散行列更新式
  void revise_sigma();
  //Eステップ
  //混合正規分布密度関数を計算
  bool revise_small_p();
  //P(omega|x;theta)更新式
  void revise_large_p();
  //crip
  void set_crisp();
};

#endif

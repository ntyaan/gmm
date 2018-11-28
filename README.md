# [C++]混合正規分布[Eigen3]

## はじめに

混合正規分布を実装するにあたって分散共分散行列の行列式と逆行列を計算するのが面倒くさい．そこでC++の行列ライブラリであるEigen3の力を借りて実装してみた．

Eigenのページ([http://eigen.tuxfamily.org/index.php?title=Main_Page](http://eigen.tuxfamily.org/index.php?title=Main_Page))から最新バージョンをDLして解凍し，Eigenフォルダを/user/local/include/にコピーして準備完了．


```bash
~$ tar xf eigen-eigen-なんたら.tar.bz2
~$ cd eigen-eigen-なんたら/
~$ sudo cp -r Eigen/ /usr/local/include/
```

逆行列は関数inverse，行列式は関数determinantで求まる．

```cpp
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
```

## 混合正規分布

続・わかりやすいパターン認識 教師なし学習入門を参考にした．


導出は正直理解できないところがあったけど最終的な答えである更新式さえ分かればなんとかなりそう．

クラス

$\omega_i$

の確率密度関数

$p\left(\boldsymbol{x_k} | \omega_{i}; \boldsymbol{\mu_i},\boldsymbol{\Sigma_i}\right) = \frac{1}{(2\pi)^{d/2}|\boldsymbol{\Sigma_i}|^{1/2}}\exp\left(-\frac{1}{2}(\boldsymbol{x_k}-\boldsymbol{\mu_i})^{\mathrm{T}}\boldsymbol{\Sigma^{-1}}(\boldsymbol{x_k}-\boldsymbol{\mu_i})\right)$

### EMアルゴリズム

Step1: 事前確率

$\pi_i$，

平均ベクトル

$\boldsymbol{\mu_i}$，

分散共分散行列

$\boldsymbol{\Sigma_i}$

の初期値を与える．

Step2:

$P\left(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i}\right)$

を求める．

$P\left(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i}\right)=\frac{\pi_i・p\left(\boldsymbol{x_k} | \omega_{i}; \boldsymbol{\mu_i},\boldsymbol{\Sigma_i}\right)}{\sum_{j=0}^C \pi_j・p\left(\boldsymbol{x_k} | \omega_{j}; \boldsymbol{\mu_j},\boldsymbol{\Sigma_j}\right)}$

step3:

$\pi_i$，$\boldsymbol{\mu_i}$，$\boldsymbol{\Sigma_i}$

を更新し，新しい

$\hat{\pi_i}$，

$\hat{\boldsymbol{\mu_i}}$，

$\hat{\boldsymbol{\Sigma_i}}$

を求める．

$\hat{\pi_i}=\frac{\sum_{k=1}^N P(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i})}{N}$

$\hat{\boldsymbol{\mu_i}}=\frac{\sum_{k=1}^N P(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i})\boldsymbol{x_k}}{\sum_{k=1}^NP(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i})}$

$\hat{\boldsymbol{\Sigma_i}}=\frac{\sum_{k=1}^N P(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i})(\boldsymbol{x_k}-\boldsymbol{\mu_i})(\boldsymbol{x_k}-\boldsymbol{\mu_i})^{\mathrm{T}}}{\sum_{k=1}^N P(\omega_i|\boldsymbol{x_k};\boldsymbol{\mu_i},\boldsymbol{\Sigma_i})}$

Step4:

ここで$\pi_i=\hat{\pi_i}$，

$\boldsymbol{\mu_i}=\hat{\boldsymbol{\mu_i}}$，

$\boldsymbol{\Sigma_i}=\hat{\boldsymbol{\Sigma_i}}$

と設定する．対数尤度

$\log{p(\boldsymbol{x};\boldsymbol{\mu},\boldsymbol{\Sigma})}$

を求め，増分が設定した閾値以下なら終了，そうでなければStep2へ戻る．

### コード

- [gmm.h](https://github.com/ntyaan/gmm/blob/master/gmm.h)
- [gmm.cxx](https://github.com/ntyaan/gmm/blob/master/gmm.cxx)
- [main.cxx](https://github.com/ntyaan/gmm/blob/master/main.cxx)
- [Makefile](https://github.com/ntyaan/gmm/blob/master/Makefile)


## 人工データ

混合数3，次元数2，データ点数各クラス{150, 50, 300}の合計500のデータを平均{{0.0, 0.0}, {0.0, -3.5}, {-2.5, -2.5}}，標準偏差{{1.0, 1.0}, {0.35, 0.40}, {0.5, 0.5}}とし生成した([normal_distribution.txt](https://github.com/ntyaan/gmm/blob/master/normal_distribution.txt))．

データ点からランダムに3つ選びその値を初期平均ベクトルへ，また分散共分散行列には単位行列を初期値として与える．初期混合率$\pi_i$には1.0/混合数を与えた．
うまい初期値だと下のようになる．

![test.gif](https://qiita-image-store.s3.amazonaws.com/0/134663/a77d7dbd-f847-f0ec-6f7b-527704bdc5bf.gif)


## iris dataset 

[https://archive.ics.uci.edu/ml/datasets/iris](https://archive.ics.uci.edu/ml/datasets/iris)のiris.dataを読み込めるように変えた[iris.txt](https://github.com/ntyaan/gmm/blob/master/iris.txt)を用いて，上と同様に初期値を決めて試した．

うまい初期値が選ばれるとほぼ間違いなく分類できる．

|| Iris-setosa  | Iris-versicolor | Iris-virginica | Sum　|
|:---|:---|:---|:----|:---|
|**Cluster1**| 50 | 0  | 0 | 50 |
|**Cluster2**|  0 | 45 | 0 | 45 | 
|**Cluster3**|  0 |  5 | 50| 55 |
|**Sum**| 50 | 50 | 50| 150 | 

リンク:[https://github.com/ntyaan/gmm](https://github.com/ntyaan/gmm)

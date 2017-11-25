#include "gmm.h"

GMM::GMM(int data_number,
	 int dimension,
	 int centers_number){
  try{
    Data.resize(data_number, dimension);
    Mu.resize(centers_number, dimension);
    LargeP.resize(centers_number, data_number);
    SmallP.resize(centers_number, data_number);
    Tmp_SmallP.resize(centers_number, data_number);
    for(int i=0;i<centers_number;i++)
      for(int k=0;k<data_number;k++)
	SmallP(i,k)=DBL_MAX;
    Sigma=new MatrixXd[centers_number];
    for(int i=0;i<centers_number;i++)
      Sigma[i].resize(dimension, dimension);
    Crisp.resize(centers_number,data_number);
    Correct.resize(centers_number,data_number);
    CTable.resize(centers_number+1,centers_number+1);
    InitializeC.resize(centers_number);
    Pi.resize(centers_number);
  }
  catch(std::bad_alloc){
    std::cerr<< "out of memory"<<std::endl;
    throw;
  }
}

GMM::~GMM(){
  delete []Sigma;
}

int GMM::data_number(){
  return Data.rows();
}

int GMM::dimension(){
  return Data.cols();
}

int GMM::centers_number(){
  return Mu.rows();
}

void GMM::input_data(const char *filename){
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int k=0;k<data_number();k++){
    for(int ell=0;ell<dimension();ell++){
      ifs>>Data(k, ell);
    }
  }
  ifs.close();
  return;
}

void GMM::input_correct(const char *filename){
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<centers_number();i++)
    for(int k=0;k<data_number();k++)
      ifs>>Correct(i, k);
  ifs.close();
  return;
}

void GMM::output(int index){
  std::ofstream ofs("result.txt");
  for(int k=0;k<data_number();k++){
    for(int ell=0;ell<dimension();ell++)
      ofs<<Data(k,ell)<<"\t";
    for(int i=0;i<centers_number();i++)
      ofs<<LargeP(i,k)<<"\t";
    ofs<<std::endl;
  }
  ofs.close();
  std::ofstream ofs_centers("centers.txt");
  for(int i=0;i<centers_number();i++){
    for(int ell=0;ell<dimension();ell++)
      ofs_centers<<Mu(i,ell)<<"\t";
    ofs_centers<<std::endl;
  }
  ofs_centers.close();
  const std::string str="gnuplot -e \"ite='gif/"
    +std::to_string(index)+"'\" plt_sample_result.plt";
  sleep(1);
  int sys=system(str.c_str());
  if(sys)
    std::cerr<<"output error"<<std::endl;
  return;
}
  
void GMM::set_ctable(void){
  MatrixXi Tmp=Crisp*Correct.transpose();
  for(int i=0;i<Tmp.rows();i++)
    for(int j=0;j<Tmp.cols();j++)
      CTable(i,j)=Tmp(i,j);
  for(int i=0;i<CTable.rows()-1;i++){
    CTable(i,CTable.cols()-1)=0.0;
    for(int j=0;j<CTable.cols()-1;j++){
      CTable(i,CTable.cols()-1)+=CTable(i,j);
    }
  }
  for(int j=0;j<CTable.cols()-1;j++){
    CTable(CTable.rows()-1,j)=0.0;
    for(int i=0;i<CTable.rows()-1;i++){
      CTable(CTable.rows()-1,j)+=CTable(i,j);
    }
  }
  CTable(CTable.rows()-1,CTable.cols()-1)=data_number();
  std::cout<<CTable<<std::endl;
  return;
}

void GMM::precision(void){
  double numerator=0.0;
  std::cout<<"Precision = TP/(TP+FP)"<<std::endl;
  for(int i=0;i<centers_number();i++){
    double result=(double)CTable(i,i)/CTable(i,centers_number());
    std::cout<<CTable(i,i)<<"/"<<CTable(i,centers_number())<<"="
	     <<result<<std::endl;
    numerator+=CTable(i,i);
  }
  std::cout<<"all:\t\t"<<numerator<<"/"<<data_number()<<" = "
	   <<numerator/data_number()<<std::endl;
  return;
}

void GMM::recall(void){
  double numerator=0.0;
  std::cout<<"Recall = TP/(TP+FN)"<<std::endl;
  for(int i=0;i<centers_number();i++){
    double result=(double)CTable(i,i)/CTable(centers_number(),i);
    std::cout<<CTable(i,i)<<"/"<<CTable(centers_number(),i)<<" = "
	     <<result<<std::endl;
    numerator+=CTable(i,i);
  }
  std::cout<<"all:\t\t"<<numerator<<"/"<<data_number()<<" = "
	   <<numerator/data_number()<<std::endl;
  return;
}

void GMM::fmeasure(void){
  std::cout<<"F-measure = (2*Recall*Pricision)/(Recall+Pricision)"
	   <<std::endl;
  for(int i=0;i<centers_number();i++){
    double recall=(double)CTable(i,i)/CTable(centers_number(),i);
    double pricision=(double)CTable(i,i)/CTable(i,centers_number());
    std::cout<<"(2*"<<recall<<"*"<<pricision
	     <<")/("<<recall<<"+"<<pricision
	     <<") = "<<(2*recall*pricision)/(recall+pricision)
	     <<std::endl;
  }
  return;
}

bool GMM::dissimilarities(int index, MatrixXd &mat){
  for(int i=0;i<index+1;i++){
    for(int k=0;k<data_number();k++){
      mat(i,k)=0.0;
      for(int ell=0;ell<dimension();ell++)
	mat(i,k)+=pow(Data(k,ell)-Mu(i,ell),2.0);
      if(std::isnan(mat(i,k)))
	return false;
    }
  }
  return true;
}

void GMM::initialize_mu(int random_index){
  std::mt19937_64 mt;
  mt.seed(random_index);
  //データ番号をランダムに発生
  std::uniform_int_distribution<> randNum(0,data_number()-1);
  //0~1の実数を発生
  std::uniform_real_distribution<> rand01(0.0,1.0);
  //step1:ランダムに一つデータを選ぶ
  InitializeC(0)=randNum(mt);
  //step1で選ばれたデータを第一クラスタ中心とする
  for(int ell=0;ell<dimension();ell++)
    Mu(0,ell)=Data(InitializeC(0),ell);
  //ユークリッド距離
  MatrixXd Dissimilarities(centers_number(),data_number());
  int i=1;
  while(1){
    //step:2
    //データ間非類似度を計算
    if(dissimilarities(i-1, Dissimilarities));
    else{
      std::cerr<<"initialize_mu error"<<std::endl;
      std::exit(1);
    }
    VectorXd Tmp_Vector(data_number());
    for(int k=0;k<data_number();k++){
      //一番近い中心までの距離が最大になるデータが選ばれるよう
      double min=DBL_MAX;
      for(int j=0;j<i;j++){
	if(min>Dissimilarities(j, k))
	  min=Dissimilarities(j, k);	      
      }
      Tmp_Vector(k)=min;
    }    
    //各非類似度を0~1の線で表す用
    //例：Tmp_Sim[0]=0
    //    Tmp_Sim[1]=Tmp_Vector[0]
    //    Tmp_Sim[2]=Tmp_Vector[2]-[1]
    //    Tmp_Sim[k]=Tmp_Vector[k]-[k-1]
    VectorXd Tmp_Similarities(data_number()+1);
    double Sum=0.0,tmp=0.0;
    Tmp_Similarities[0]=0.0;
    //合計を計算
    for(int k=0;k<data_number();k++)
      Sum+=Tmp_Vector(k);
    for(int k=1;k<data_number()+1;k++){
      tmp+=Tmp_Vector(k-1)/Sum;//非類似度を0~1で表現
      Tmp_Similarities(k)=tmp;
    }
    double rnd01=rand01(mt);
    for(int k=0;k<data_number();k++){
      //0~1をランダムで選び、選ばれたデータを中心へ
      if(Tmp_Similarities(k)<rnd01&&Tmp_Similarities(k+1)>=rnd01)
	InitializeC(i)=k;
    }
    for(int ell=0;ell<dimension();ell++)
      Mu(i, ell)=Data(InitializeC[i], ell);
    i++;
    if(i==centers_number())break;
  }
#ifdef SAMPLE
  std::ofstream ofs("centers.txt");
  for(int i=0;i<centers_number();i++){
    for(int ell=0;ell<dimension();ell++){
      ofs<<Data(InitializeC(i),ell)<<"\t";
    }
    ofs<<std::endl;
  }
  ofs.close();
  const std::string str="gnuplot -e \"ite='gif/'\" plt_sample1.plt";
  sleep(1);
  int sys=system(str.c_str());
  if(sys)
    std::cerr<<"output error"<<std::endl;
#endif
  return;
}

void GMM::initialize_pi(){
  for(int i=0;i<centers_number();i++)
    Pi(i)=1.0/centers_number();
  return;
}

void GMM::initialize_sigma(){
  for(int i=0;i<centers_number();i++)
    for(int ell=0;ell<dimension();ell++)
      Sigma[i](ell,ell)=1.0; 
  return;
}

void GMM::revise_pi(){
  for(int i=0;i<centers_number();i++){
    double numerator=0.0;
    for(int k=0;k<data_number();k++)
      numerator+=LargeP(i,k);
    Pi(i)=numerator/data_number();
#ifdef DEBUG
    if(std::isnan(Pi(i))){
      std::cerr<<"Pi"<<std::endl;
      std::exit(1);
    }
#endif
  }
  return;
}

void GMM::revise_mu(){
  for(int i=0;i<centers_number();i++){
    for(int ell=0;ell<dimension();ell++){
      double numerator=0.0;
      double denominator=0.0;
      for(int k=0;k<data_number();k++){
	numerator+=LargeP(i,k)*Data(k,ell);
	denominator+=LargeP(i,k);
      }
      Mu(i,ell)=numerator/denominator;
#ifdef DEBUG
      if(std::isnan(Mu(i,ell))){
	std::cerr<<"Mu"<<std::endl;
	std::exit(1);
      }
#endif
    }
  }
  return;
}

void GMM::revise_sigma(){
  for(int i=0;i<centers_number();i++){
    for(int ell_a=0;ell_a<dimension();ell_a++){
      for(int ell_b=0;ell_b<dimension();ell_b++){
	double numerator=0.0;
	double denominator=0.0;
	for(int k=0;k<data_number();k++){
	  numerator+=LargeP(i,k)
	    *(Data(k,ell_a)-Mu(i,ell_a))*(Data(k,ell_b)-Mu(i,ell_b));
	  denominator+=LargeP(i,k);
	}
	Sigma[i](ell_a,ell_b)=numerator/denominator;
#ifdef DEBUG
	if(std::isnan(Sigma[i](ell_a,ell_b))){
	  std::cerr<<"Sigma"<<std::endl;
	  std::exit(1);
	}
#endif
      }
    }
  }
  return;
}

bool GMM::revise_small_p(){
  Tmp_SmallP=SmallP;
  double threshold=0.0;
  for(int i=0;i<centers_number();i++){
    MatrixXd Sigma_inverse=Sigma[i].inverse();
    for(int k=0;k<data_number();k++){
      double sum=0.0;
      for(int ell_a=0;ell_a<dimension();ell_a++){
	for(int ell_b=0;ell_b<dimension();ell_b++){
	  sum+=(Data(k,ell_a)-Mu(i,ell_a))
	    *Sigma_inverse(ell_a,ell_b)
	    *(Data(k,ell_b)-Mu(i,ell_b));
	}
      }
      double det=Sigma[i].determinant();
      SmallP(i,k)=exp(-0.5*sum)/(pow(2.0*M_PI,dimension()/2.0)
				 *pow(det,0.5));
      threshold+=fabs(log(SmallP(i,k))-log(Tmp_SmallP(i,k)));
#ifdef DEBUG
      if(std::isnan(SmallP(i,k))){
	std::cerr<<"SmallP:"<<det<<std::endl;
	std::cout<<Sigma_inverse<<std::endl;
	std::exit(1);
      }
#endif
    }
  }
  if(threshold<=1.0e-10)
    return true;
  return false;
}

void GMM::revise_large_p(){
  for(int k=0;k<data_number();k++){
    double denominator=0.0;
    for(int j=0;j<centers_number();j++)
      denominator+=Pi(j)*SmallP(j,k);
    for(int i=0;i<centers_number();i++){
      LargeP(i,k)=(Pi(i)*SmallP(i,k))/denominator;
#ifdef DEBUG
      if(std::isnan(LargeP(i,k))){
	std::cerr<<"LargeP"<<std::endl;
	std::exit(1);
      }
#endif
    }
  }
  return;
}

void GMM::set_crisp(void){
  for(int k=0;k<data_number();k++){
    for(int i=0;i<centers_number();i++)
      Crisp(i,k)=0.0;
    double max=-DBL_MAX;
    int max_index=-1;
    for(int i=0;i<centers_number();i++){
      if(LargeP(i,k)>max){
	max=LargeP(i,k);
	max_index=i;
      }
    }
    Crisp(max_index,k)=1.0;
  }
  return;
}

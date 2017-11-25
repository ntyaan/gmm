#include <iostream>
#include <random>
#include <cstdlib>
#include <cstring>
#include <fstream>

constexpr int CLASS=3;
constexpr int Num=500;
constexpr int d[3]={150,50,300};

int main(){
  std::random_device seed_gen;
  std::default_random_engine engine(seed_gen());
  std::normal_distribution<> dist(0.0, 1.0);
  std::normal_distribution<> dist2_1(0.0, 0.35);
  std::normal_distribution<> dist2_2(-3.5, 0.40);
  std::normal_distribution<> dist3(-2.5, 0.50);

  std::ofstream file("normal_distribution.txt");
  for (std::size_t n = 0; n < d[0]; ++n) {
    double result1 = dist(engine);
    double result2 = dist(engine);
    file << result1<<"\t"<<result2<<std::endl;
  }
  for (std::size_t n = 0; n < d[1]; ++n) {
    double result3 = dist2_1(engine);
    double result4 = dist2_2(engine);
    file << result3<<"\t"<<result4<<std::endl;
  }
  
  for (std::size_t n = 0; n < d[2]; ++n) {
    double result5 = dist3(engine);
    double result6 = dist3(engine);
    file << result5<<"\t"<<result6<<std::endl;
  }
  file.close();
  const std::string str="gnuplot -e \"ite='gif/'\" plt_sample0.plt";
  int sys=system(str.c_str());
  if(sys)
    std::cerr<<"output error"<<std::endl;
  
  std::ofstream ofs("normal_distribution_correct.txt");
  int tmp=d[0];
  for(int i=0;i<CLASS;i++){
    if(i==0){
      for(int j=0;j<d[i];j++){
	ofs<<1<<" ";
      }
      for(int j=0;j<Num-d[i];j++){
	ofs<<0<<" ";
      }
      ofs<<std::endl;
    } 
    else if(i+1==CLASS){
      for(int j=0;j<Num-d[i];j++){
	ofs<<0<<" ";
      }
      for(int j=0;j<d[i];j++){
	ofs<<1<<" ";
      }
      ofs<<std::endl;
    }
    else{
      for(int j=0;j<tmp;j++){
	ofs<<0<<" ";
      }
      for(int j=0;j<d[i];j++){
	ofs<<1<<" ";
      }
      for(int j=0;j<Num-(d[i]+tmp);j++){
	ofs<<0<<" ";
      }
      ofs<<std::endl;
      tmp+=d[i];
    }
  }

  return 0;
}


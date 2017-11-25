#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

constexpr int CLASS=3;
constexpr int Num=150;
constexpr int d[3]={50,50,50};

int main(){

  std::ifstream ifs("iris.data");
  if(ifs.fail()){
    std::cerr<<"error"<<std::endl;
    exit(1);
  }
  std::ofstream file("iris.txt");
  std::string str;
  while(getline(ifs,str)){
    std::string token;
    std::istringstream oss(str);
    for(int i=0;i<4;i++){
      getline(oss,token,',');
      double temp=stof(token); 
      file<<temp<<"\t";
    }
    file<<std::endl;
  }
  file.close();
  
  std::ofstream ofs("iris_correct.txt");
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


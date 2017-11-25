#include "gmm.h"
#define MAX 100
int main(const int argc, const char *argv[]){
  if(argc!=6){
    std::cerr<<"main arg error"<<std::endl;
    std::exit(1);
  }
  
  const int data_number=atoi(argv[argc-5]);
  const int dimension=atoi(argv[argc-4]);
  const int centers_number=atoi(argv[argc-3]);
  
  GMM test(data_number, dimension, centers_number);
  test.input_data(argv[argc-2]);
  test.input_correct(argv[argc-1]);
  test.initialize_mu(1);
  test.initialize_pi();
  test.initialize_sigma();
  int ite=0;
  while(1){
    if(test.revise_small_p())break;
    if(ite>=MAX)break;
    test.revise_large_p();
    test.revise_pi();
    test.revise_mu();
    test.revise_sigma();
#ifdef SAMPLE
    if(ite<24)
      test.output(ite+2);
#endif
    ite++;
  }
  std::cout<<"ite:"<<ite<<std::endl;
  test.set_crisp();
  test.set_ctable();

#ifdef IRIS
  test.precision();
  test.recall();
  test.fmeasure();
#endif
  return 0;
}

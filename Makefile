CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -std=c++17

sample: sample.cxx
	$(CXX) $(CXXFLAGS) $^ -o $@.out
iris_make: iris_make.cxx
	$(CXX) $(CXXFLAGS) $^ -o $@.out
gmm_iris: gmm.cxx main.cxx
	$(CXX) $(CXXFLAGS) -DIRIS $^ -o $@.out
gmm_sample: gmm.cxx main.cxx
	$(CXX) $(CXXFLAGS) $^ -DSAMPLE -o $@.out
run_gmm_sample:
	./gmm_sample.out 500 2 3 normal_distribution.txt normal_distribution_correct.txt 
run_gmm_iris:
	./gmm_iris.out 150 4 3 iris.txt iris_correct.txt
make_gif:
	convert -delay 100 gif/0.gif gif/1.gif gif/2.gif gif/3.gif gif/4.gif gif/5.gif gif/6.gif gif/7.gif gif/8.gif gif/9.gif gif/10.gif gif/11.gif gif/12.gif gif/13.gif gif/14.gif gif/15.gif gif/16.gif gif/17.gif gif/18.gif gif/19.gif gif/20.gif gif/21.gif gif/22.gif gif/23.gif test.gif
clean:
	rm -f *.out

all: poisson_test

poisson_test: poisson_test.cpp poisson_loop_switching.cpp
	g++ -O3 -std=c++11 -Wall -g3 -o $@ $^ -lpthread

clean:
	rm poisson_test


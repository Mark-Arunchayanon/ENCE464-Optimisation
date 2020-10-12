CC=g++
CFLAGS=-O1 -std=c++11 -Wall -g3

all: poisson_test 

poisson_test: poisson_test.cpp poisson.cpp
	$(CC) $(CFLAGS) -pg -o $@ $^ -lpthread

%.prof: %
	-./$<
	gprof -lb $< > $@

clean:
	rm -f poisson_test 
	rm -f gmon.out perf.data*

.PHONY: all clean

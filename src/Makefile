CC=g++
CFLAGS=-O1 -std=c++11 -Wall -g3

all: poisson_test poisson_naive poisson_x_inner poisson_loop_switching poisson_memcpy

poisson_test: poisson_test.cpp poisson.cpp
	$(CC) $(CFLAGS) -pg -o $@ $^ -lpthread

poisson_naive: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp
	
poisson_x_inner: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp
	
poisson_loop_switching: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp
	
poisson_memcpy: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp

clean:
	rm -f poisson_test poisson_naive poisson_x_inner poisson_loop_switching poisson_memcpy
	rm -f gmon.out perf.data*

.PHONY: all clean

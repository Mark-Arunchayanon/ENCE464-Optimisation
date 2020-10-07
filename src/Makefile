CC=g++
CFLAG=-O3 -std=c++11 -Wall -g3

all: poisson_test poisson_x_inner poisson_loop_switching

poisson_test: poisson_test.cpp poisson.cpp
	$(CC) $(CFLAGS) -o $@ $^ -lpthread
	
poisson_x_inner: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp
	
poisson_loop_switching: poisson_test.cpp
	$(CC) $(CFLAGS) -o $@ $^ $@.cpp

clean:
	rm -f poisson_test poisson_x_inner poisson_loop_switching

.PHONY: all clean

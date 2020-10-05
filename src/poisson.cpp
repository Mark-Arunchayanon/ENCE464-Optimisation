#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STORE 	res -= ta->delta * ta->delta * ta->source[((z * ta->ysize) + y) * ta->xsize + x]; \
				res /= 6; \
				ta->potential[((z * ta->ysize) + y) * ta->xsize + x] = res;
		
#define Z_0 	res += ta->input[(((z + 1) * ta->ysize) + y) * ta->xsize + x] + ta->Vbound;
#define Z_MID	res += ta->input[(( (z + 1) * ta->ysize) + y) * ta->xsize + x] + ta->input[(( (z - 1) * ta->ysize) + y) * ta->xsize + x];						
#define Z_MAX 	res += ta->input[(((z - 1) * ta->ysize) + y) * ta->xsize + x] + ta->Vbound;

#define Y_0 	res += ta->input[((z * ta->ysize) + (y + 1)) * ta->xsize + x] + ta->Vbound;
#define Y_MID	res += ta->input[( (z * ta->ysize) + (y + 1) ) * ta->xsize + x] + ta->input[( (z * ta->ysize) + (y - 1) ) * ta->xsize + x];
#define Y_MAX	res += ta->input[((z * ta->ysize) + (y - 1)) * ta->xsize + x] + ta->Vbound;

#define X_0		res += ta->input[((z * ta->ysize) + y) * ta->xsize + (x + 1)] + ta->Vbound;
#define X_MID	res += ta->input[(( (z) * ta->ysize) + y ) * ta->xsize + (x + 1)] + ta->input[(( (z) * ta->ysize) + y ) * ta->xsize + (x - 1)];
#define X_MAX	res += ta->input[((z * ta->ysize) + y) * ta->xsize + (x - 1)] + ta->Vbound;

void *thread(void* args);

// structure we're going to use for arguments to our pthread functions
struct thread_args {
	double *__restrict__ source;
	double * __restrict__ potential;
	double * __restrict__ input;
	double Vbound;
	unsigned int xsize;
	unsigned int ysize;
	unsigned int zsize;
	double delta;
	unsigned int numiters;
	unsigned int numcores;
	size_t size;
	pthread_t thread;
	FILE *ptr;
}thread_args;

/// Solve Poisson's equation for a rectangular box with Dirichlet
/// boundary conditions on each face.
/// \param source is a pointer to a flattened 3-D array for the source function
/// \param potential is a pointer to a flattened 3-D array for the calculated potential
/// \param Vbound is the potential on the boundary
/// \param xsize is the number of elements in the x-direction
/// \param ysize is the number of elements in the y-direction
/// \param zsize is the number of elements in the z-direction
/// \param delta is the voxel spacing in all directions
/// \param numiters is the number of iterations to perform
/// \param numcores is the number of CPU cores to use.  If 0, an optimal number is chosen

void poisson_dirichlet (double * __restrict__ source,
                        double * __restrict__ potential,
                        double Vbound,
                        unsigned int xsize, unsigned int ysize, unsigned int zsize, double delta,
                        unsigned int numiters, unsigned int numcores)
{
	// How many threads should we create?
	struct thread_args ta[numcores];
	
    // source[i, j, k] is accessed with source[((k * ysize) + j) * xsize + i]
    // potential[i, j, k] is accessed with potential[((k * ysize) + j) * xsize + i]    
    size_t size = (size_t)ysize * zsize * xsize * sizeof(double);
    FILE *ptr;
    ptr = fopen("result_optimised.txt", "w");
    fclose(ptr);
    ptr = fopen("result_optimised.txt", "a");
	double *input = (double *)malloc(size);
	//~ double *temp = (double *)malloc(size);
	
	if (!input) {
		fprintf(stderr, "malloc failure\n");
		return;
	}
	memcpy(input, source, size);

	// Split up the incoming data, and spawn the threads
	for (unsigned int i = 0; i < numcores; i++) {
		ta[i].source = source;
		ta[i].potential = potential;
		ta[i].input = input;
		ta[i].Vbound = Vbound;
		ta[i].xsize = xsize;
		ta[i].ysize = ysize;
		ta[i].zsize = zsize;
		ta[i].delta = delta;
		ta[i].numiters = numiters;
		ta[i].numcores = numcores;
		ta[i].size = size;
		ta[i].ptr = ptr;
		
		if (pthread_create(&ta[i].thread, NULL, thread, (void *)&ta[i]) < 0)
			fprintf(stderr, "Could not create thread %d\n", i);
	}

	// Wait for each thread to finish, and add in its partial sum
	for (unsigned int i = 0; i < numcores; i++) {
		pthread_join(ta[i].thread, NULL);
	}
	
	fclose(ptr);
	free(input);
}


void *thread(void* args) {
	
	struct thread_args *ta = (struct thread_args*)args;
	
	for (unsigned int iter = 0; iter < ta->numiters; iter++) {
		double res = 0;

		// Loop through general cases (i.e. 0 < x,y,z < maximum) having dealt with zero and maximum seperately
		//  Means no condition checking in loops
		for (unsigned int z = 1; z < ta->zsize - 1; z++) {
			for (unsigned int y = 1; y < ta->ysize - 1; y++) {
				for  (unsigned int x = 1; x < ta->xsize - 1; x++) {
					res = 0;
					
					res += ta->input[(( (z) * ta->ysize) + y ) * ta->xsize + (x + 1)];
					res += ta->input[(( (z) * ta->ysize) + y ) * ta->xsize + (x - 1)];

					res += ta->input[( (z * ta->ysize) + (y + 1) ) * ta->xsize + x];
					res += ta->input[( (z * ta->ysize) + (y - 1) ) * ta->xsize + x];

					res += ta->input[(( (z + 1) * ta->ysize) + y) * ta->xsize + x];
					res += ta->input[(( (z - 1) * ta->ysize) + y) * ta->xsize + x];
						
					STORE;
				}
			}
		}
		
		//x y z'
		for (unsigned int y = 1; y < ta->ysize - 1; y++) {
			for (unsigned int x = 1; x < ta->xsize - 1; x++) {
				res = 0;
				unsigned int z = 0;
				
				Z_0
				Y_MID X_MID STORE
				
				res = 0;
				z = ta->zsize - 1;
				
				Z_MAX Y_MID X_MID
				STORE
			}
		}
		
		//x y' z
		for (unsigned int z = 1; z < ta->zsize - 1; z++) {
			for  (unsigned int x = 1; x < ta->xsize - 1; x++) {
				res = 0;
				unsigned int y = 0;
				Y_0 X_MID Z_MID
				STORE
				
				res = 0;
				y = ta->ysize - 1;
				Y_MAX X_MID Z_MID
				STORE
			}
		}
		
		//x y' z'
		for  (unsigned int x = 1; x < ta->xsize - 1; x++) {
			res = 0;
			unsigned int z = 0;
			unsigned int y = 0;
			// z = 0
			Z_0 Y_0 X_MID
			STORE
			
			res = 0;
			//--
			y = ta->ysize - 1;
			// z = 0
			Z_0 Y_MAX X_MID
			STORE
			
			res = 0;
			//---
			z = ta->zsize -1;
			y = 0;
			// z = max
			Z_MAX Y_0 X_MID
			STORE
			
			res = 0;
			//--
			y = ta->ysize - 1;
			// z = max
			Z_MAX Y_MAX X_MID
			STORE
		}
		
		//x' y z
		for (unsigned int z = 1; z < ta->zsize - 1; z++) {
			for (unsigned int y = 1; y < ta->ysize - 1; y++) {
				res = 0;
				unsigned int x = 0;
				
				X_0 Y_MID Z_MID
				STORE
				
				res = 0;
				//--
				x = ta->xsize - 1;
				X_MAX Y_MID Z_MID
				STORE
			}
		}
		
		//x' y z'
		for (unsigned int y = 1; y < ta->ysize - 1; y++) {
			res = 0;
			unsigned int x = 0;
			unsigned int z = 0;
			// x = 0
			X_0 Z_0 Y_MID
			STORE
			
			res = 0;
			//--
			z = ta->zsize - 1;
			// x = 0
			X_0 Z_MAX Y_MID
			STORE
			
			res = 0;
			//--
			x = ta->xsize - 1;
			z = 0;
			// x = max
			X_MAX Z_0 Y_MID
			STORE
			
			res = 0;
			//--
			z = ta->zsize - 1;
			// x = max
			X_MAX Z_MAX Y_MID
			STORE
		}	
		
		//x' y' z
		for (unsigned int z = 1; z < ta->zsize - 1; z++) {
			res = 0;
			unsigned int x = 0;
			unsigned int y = 0;
			// x = 0
			X_0 Y_0 Z_MID
			STORE
			
			res = 0;
			//--
			y = ta->ysize - 1;
			// x = 0
			X_0 Y_MAX Z_MID
			STORE
			
			res = 0;
			//--
			x = ta->xsize - 1;
			y = 0;
			// x = max
			X_MAX Y_0 Z_MID
			STORE
			
			res = 0;
			//--
			y = ta->ysize - 1;
			// x = max
			X_MAX Y_MAX Z_MID
			STORE
		}
		
		//x' y' z'
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int z = 0;
		res = 0;
		// x = 0
		X_0 Y_0 Z_0;
		STORE;
		
		res = 0;
		//--
		z = ta->zsize - 1;
		// x = 0, y = 0, z = MAX
		X_0 Y_0 Z_MAX;
		STORE;
		
		res = 0;
		//--
		y = ta->ysize - 1;
		z = 0;
		// x = 0
		X_0 Y_MAX Z_0;
		STORE;
		
		res = 0;
		//--
		z = ta->zsize - 1;
		// x = 0
		X_0 Y_MAX Z_MAX;	
		STORE;
		
		res = 0;
		//--
		x = ta->xsize - 1;
		y = 0;
		z = 0;
		// x = max
		X_MAX Y_0 Z_0;
		STORE;
		
		res = 0;
		//--
		z = ta->zsize - 1;
		// x = max
		X_MAX Y_0 Z_MAX;	
		STORE;
		
		res = 0;
		//--
		y = ta->ysize - 1;
		z = 0;
		// x = max
		X_MAX Y_MAX Z_0;
		STORE;
		
		res = 0;
		//---
		x = ta->xsize - 1;
		y = ta->ysize - 1;
		z = ta->zsize - 1;
		// x = max
		X_MAX Y_MAX Z_MAX;	
		STORE;


		double *temp = ta->input;
		ta->input = ta->potential;
		ta->potential = temp;
		
		
		
		//fprintf(ptr, "%2f\n", *potential);
		//memcpy(input, potential, size);
	}
	if (ta->numiters % 2 == 0) {
		memcpy(ta->potential, ta->input, ta->size);
	}
			//~ free(ta->input);
		//~ } else {
			//~ free(ta->potential);
		//~ }
	for (unsigned int y = 0; y < ta->ysize; y++) {
		for (unsigned int z = 0; z < ta->zsize; z++) {
			for (unsigned int x = 0; x < ta->xsize; x++) {
				fprintf(ta->ptr, "x: %d | y: %d | z: %d - %2f\n", x, y, z, ta->potential[((z * ta->ysize) + y) * ta->xsize + x]);
			}
		}
	}
	//~ fclose(ptr);
	//~ free(input); // free in the main
	return 0;
}

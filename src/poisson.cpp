#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    // source[i, j, k] is accessed with source[((k * ysize) + j) * xsize + i]
    // potential[i, j, k] is accessed with potential[((k * ysize) + j) * xsize + i]    
    size_t size = (size_t)ysize * zsize * xsize * sizeof(double);
	double *input = (double *)malloc(size);
	if (!input) {
		fprintf(stderr, "malloc failure\n");
		return;
	}
	memcpy(input, source, size);
	for (unsigned int iter = 0; iter < numiters; iter++) {
		double res = 0;
		
		for (unsigned int x = 0; x < xsize; x++) {
			if (x > 0)
			{
				if (x < xsize - 1) {
					// General x case
					for (unsigned int y = 0; y < ysize; y++) {
						if (y > 0)
							if (y < ysize - 1) {
								// General y case
								for (unsigned int z = 0; z < zsize; z++) {
									//z cases
									for (unsigned int z = 0; z < zsize; z++) {
										if (z > 0)
											if (z < zsize - 1) {
												// General z case
												res += input[(((z + 1) * ysize) + y) * xsize + x];
											}
											else {
												// Maximum z case
												res += input[(((z - 1) * ysize) + y) * xsize + x];
											}
										else {
											// Minimum z case
											res -= delta * delta * source[((z * ysize) + y) * xsize + x];
										}
									}
								}
							}
							else {
								// Maximum y edge case i.e. y = ysize - 1
								for (unsigned int z = 0; z < zsize; z++) {
									//ToDO
								}
							}
						}
						else {
							// Minimum y edge case y = 0
							for (unsigned int z = 0; z < zsize; z++) {
								//ToDO
							}
						}
					}
				}
				else {
					// Maximum x edge case i.e. x = zsize - 1
					for (unsigned int y = 0; y < ysize; y++) {
						if (y > 0)
							if (y < ysize - 1) {
								// General y case
								for (unsigned int z = 0; z < zsize; z++) {
									//ToDO
								}
							}
							else {
								// Maximum y edge case i.e. y = ysize - 1
								for (unsigned int z = 0; z < zsize; z++) {
									//ToDO
								}
							}
						}
						else {
							// Minimum y edge case y = 0
							for (unsigned int z = 0; z < zsize; z++) {
								//ToDO
							}
						}
					}
				}
			}
			else {
				// Minimum x edge case x = 0
				for (unsigned int y = 0; y < ysize; y++) {
					if (y > 0) {
						if (y < ysize - 1) {
							// General y case
							for (unsigned int z = 0; z < zsize; z++) {
								//ToDO
							}
						}
						else {
							// Maximum y edge case i.e. y = ysize - 1
							for (unsigned int z = 0; z < zsize; z++) {
								//ToDO
							}
						}
					else {
						// Minimum y edge case y = 0
						for (unsigned int z = 0; z < zsize; z++) {
							//ToDO
						}
			}
					
						res += Vbound;

					if (x > 0)
						res += input[((z * ysize) + y) * xsize + (x - 1)];
					else
						res += Vbound;

			for (unsigned int z = 0; z < zsize; z++) {

				if (y < ysize - 1)
						res += input[((z * ysize) + (y + 1)) * xsize + x];
				if (y > 0)
						res += input[((z * ysize) + (y - 1)) * xsize + x];
		
				for (unsigned int y = 0; y < ysize; y++) {
					

					if (x < xsize - 1)
						res += input[((z * ysize) + y) * xsize + (x + 1)];
					if (x > 0)
						res += input[((z * ysize) + y) * xsize + (x - 1)];

					

					if (z < zsize - 1)
						res += input[(((z + 1) * ysize) + y) * xsize + x];
					if (z > 0)
						res += input[(((z - 1) * ysize) + y) * xsize + x];

					res -= delta * delta * source[((z * ysize) + y) * xsize + x];

					res /= 6;

					potential[((z * ysize) + y) * xsize + x] = res;
				}
			}
		}
		memcpy(input, potential, size);
	}
	free(input);
}

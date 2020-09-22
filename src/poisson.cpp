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
		
		// Loop through general cases (i.e. 0 < x,y,z < maxSize) deal with zero and maximum seperately
		//  Means no condition checking in loops
		for (unsigned int x = 1; x < xsize-1; x++) {
			for (unsigned int y = 1; y < ysize-1; y++) {
				for (unsigned int z = 1; z < zsize-1; z++) {
					res += input[(( (z) * ysize) + y ) * xsize + x];
						
					res -= delta * delta * source[((z * ysize) + y) * xsize + x];
					res /= 6;
					potential[((z * ysize) + y) * xsize + x] = res;
				}
			}
		}

		// Deal with x = 0
			// res += ysize * 2 *zsize * Vbound;
			//Deal with y = 0
				// res += 2 * zsize * Vbound;
				
		// Deal with x = max
			// res += ysize * zsize * Vbound
			//Deal with y = 0
				// res += 2 * zsize * Vbound;
			

		memcpy(input, potential, size);
	}
	free(input);
}

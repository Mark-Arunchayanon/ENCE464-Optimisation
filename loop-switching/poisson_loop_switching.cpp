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

		// Loop through general cases (i.e. 0 < x,y,z < maximum) having dealt with zero and maximum seperately
		//  Means no condition checking in loops
		for (unsigned int z = 1; z < zsize - 1; z++) {
			for (unsigned int y = 1; y < ysize - 1; y++) {
				for  (unsigned int x = 1; x < xsize - 1; x++) {
					res = 0;
					res += input[(( (z) * ysize) + y ) * xsize + (x + 1)];
					res += input[(( (z) * ysize) + y ) * xsize + (x - 1)];

					res += input[( (z * ysize) + (y + 1) ) * xsize + x];
					res += input[( (z * ysize) + (y - 1) ) * xsize + x];

					res += input[(( (z + 1) * ysize) + y) * xsize + x];
					res += input[(( (z - 1) * ysize) + y) * xsize + x];
						
					res -= delta * delta * source[((z * ysize) + y) * xsize + x];
					res /= 6;
					potential[((z * ysize) + y) * xsize + x] = res;
				}
			}
		}
		
		//x y z'
		for (unsigned int y = 1; y < ysize - 1; y++) {
			for (unsigned int x = 1; x < xsize - 1; x++) {
				unsigned int z = 0;
				res = 0;
				res += input[(((z + 1) * ysize) + y) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
				res = 0;
				z = zsize - 1;
				res += input[(((z - 1) * ysize) + y) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
			}
		}
		
		//x y' z
		for (unsigned int z = 1; z < zsize - 1; z++) {
			for  (unsigned int x = 1; x < xsize - 1; x++) {
				unsigned int y = 0;
				res = 0;
				res += input[((z * ysize) + (y + 1)) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
				res = 0;
				y = ysize - 1;
				res += input[((z * ysize) + (y - 1)) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
			}
		}
		
		//x y' z'
		for  (unsigned int x = 1; x < xsize - 1; x++) {
			unsigned int z = 0;
			unsigned int y = 0;
			res = 0;
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			y = ysize - 1;
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//---
			z = zsize -1;
			y = 0;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			y = ysize - 1;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
		}
		
		//x' y z
		for (unsigned int z = 1; z < zsize - 1; z++) {
			for (unsigned int y = 1; y < ysize - 1; y++) {
				unsigned int x = 0;
				res = 0;
				res += input[((z * ysize) + y) * xsize + (x + 1)];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
				res = 0;
				//--
				x = xsize - 1;
				res += input[((z * ysize) + y) * xsize + (x - 1)];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
			}
		}
		
		//x' y z'
		for (unsigned int y = 1; y < ysize - 1; y++) {
			unsigned int x = 0;
			unsigned int z = 0;
			res = 0;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			z = zsize - 1;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			x = xsize - 1;
			z = 0;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			z = zsize - 1;
		
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
		}	
		
		//x' y' z
		for (unsigned int z = 1; z < zsize - 1; z++) {
			unsigned int x = 0;
			unsigned int y = 0;
			res = 0;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			y = ysize - 1;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			x = xsize - 1;
			y = 0;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			//--
			y = ysize - 1;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			res = 0;
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
		}
		
		res = 0;
		//x' y' z'
		unsigned int x = 0;
		unsigned int y = 0;
		unsigned int z = 0;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		z = zsize - 1;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		y = ysize - 1;
		z = 0;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		z = zsize - 1;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		x = xsize - 1;
		y = 0;
		z = 0;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;	
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		z = zsize - 1;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//--
		y = ysize - 1;
		z = 0;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		//---
		x = xsize - 1;
		y = ysize - 1;
		z = zsize - 1;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		res = 0;
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;

		memcpy(input, potential, size);
	}
	free(input);
}

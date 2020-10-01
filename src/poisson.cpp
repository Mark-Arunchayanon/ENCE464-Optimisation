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
				res += input[(((z + 1) * ysize) + y) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
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
				res += input[((z * ysize) + (y + 1)) * xsize + x];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
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
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			y = ysize - 1;
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//---
			z = zsize -1;
			y = 0;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			y = ysize - 1;
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
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
				res += input[((z * ysize) + y) * xsize + (x + 1)];
				res += Vbound;
				
				res -= delta * delta * source[((z * ysize) + y) * xsize + x];
				res /= 6;
				potential[((z * ysize) + y) * xsize + x] = res;
				
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
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			z = zsize - 1;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// z = max
			res += input[(((z - 1) * ysize) + y) * xsize + x];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			x = xsize - 1;
			z = 0;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// z = 0
			res += input[(((z + 1) * ysize) + y) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			z = zsize - 1;
		
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
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
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			y = ysize - 1;
			// x = 0
			res += input[((z * ysize) + y) * xsize + (x + 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			x = xsize - 1;
			y = 0;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = 0
			res += input[((z * ysize) + (y + 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			//--
			y = ysize - 1;
			// x = max
			res += input[((z * ysize) + y) * xsize + (x - 1)];
			res += Vbound;
				
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
			
			// y = max
			res += input[((z * ysize) + (y - 1)) * xsize + x];
			res += Vbound;
			
			res -= delta * delta * source[((z * ysize) + y) * xsize + x];
			res /= 6;
			potential[((z * ysize) + y) * xsize + x] = res;
		}
		
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
		
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		//--
		z = zsize - 1;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		//--
		y = ysize - 1;
		z = 0;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		//--
		z = zsize - 1;
		// x = 0
		res += input[((z * ysize) + y) * xsize + (x + 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
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
			
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		//--
		z = zsize - 1;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// y = 0
		res += input[((z * ysize) + (y + 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = max
		res += input[(((z - 1) * ysize) + y) * xsize + x];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		//--
		y = ysize - 1;
		z = 0;
		// x = max
		res += input[((z * ysize) + y) * xsize + (x - 1)];
		res += Vbound;
				
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
		// z = 0
		res += input[(((z + 1) * ysize) + y) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
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
		
		// y = max
		res += input[((z * ysize) + (y - 1)) * xsize + x];
		res += Vbound;
			
		res -= delta * delta * source[((z * ysize) + y) * xsize + x];
		res /= 6;
		potential[((z * ysize) + y) * xsize + x] = res;
		
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
	
		//// Handle zero and maximum conditions for x, y, z
		//// z cases
		//// Deal with z = zero
		//z = 0;
		//// iterate through y loop
		//for (unsigned int y = 0; y < ysize; y++) {	
			//// iterate through x loop
			//for (unsigned int x = 0; x < zsize; x += xsize) {
				//if (x < xsize - 1)
					//res += input[((z * ysize) + y) * xsize + (x + 1)];
				//else
					//res += Vbound;
				//if (x > 0)
					//res += input[((z * ysize) + y) * xsize + (x - 1)];
				//else
					//res += Vbound;

				//if (y < ysize - 1)
					//res += input[((z * ysize) + (y + 1)) * xsize + x];
				//else
					//res += Vbound;
				//if (y > 0)
					//res += input[((z * ysize) + (y - 1)) * xsize + x];
				//else
					//res += Vbound;

				////if (z < zsize - 1)
					//res += input[(((z + 1) * ysize) + y) * xsize + x];
				////else
					////res += Vbound;
				////if (z > 0)
					////res += input[(((z - 1) * ysize) + y) * xsize + x];
				////else
					//res += Vbound;
			//}
		//}
			
		//// Deal with z = max
		//z = zsize;
		//// iterate through y loop
		//for (unsigned int y = 0; y < ysize; y++) {	
			//// iterate through x loop
			//for (unsigned int x = 0; x < zsize; x += xsize) {
				//if (x < xsize - 1)
					//res += input[((z * ysize) + y) * xsize + (x + 1)];
				//else
					//res += Vbound;
				//if (x > 0)
					//res += input[((z * ysize) + y) * xsize + (x - 1)];
				//else
					//res += Vbound;

				//if (y < ysize - 1)
					//res += input[((z * ysize) + (y + 1)) * xsize + x];
				//else
					//res += Vbound;
				//if (y > 0)
					//res += input[((z * ysize) + (y - 1)) * xsize + x];
				//else
					//res += Vbound;

				////if (z < zsize - 1)
					////res += input[(((z + 1) * ysize) + y) * xsize + x];
				////else
					//res += Vbound;
				////if (z > 0)
					//res += input[(((z - 1) * ysize) + y) * xsize + x];
				////else
					////res += Vbound;
			//}
		//}

		//// y cases
		//// Deal with y = zero
		//y = 0;
		//// iterate through z loop
		//for (unsigned int z = 0; z < ysize; z++) {	
			//// iterate through x loop
			//for (unsigned int x = 0; x < zsize; x += xsize) {
				//if (x < xsize - 1)
					//res += input[((z * ysize) + y) * xsize + (x + 1)];
				//else
					//res += Vbound;
				//if (x > 0)
					//res += input[((z * ysize) + y) * xsize + (x - 1)];
				//else
					//res += Vbound;

				////if (y < ysize - 1)
					//res += input[((z * ysize) + (y + 1)) * xsize + x];
				////else
					////res += Vbound;
				////if (y > 0)
					////res += input[((z * ysize) + (y - 1)) * xsize + x];
				////else
					//res += Vbound;

				//if (z < zsize - 1)
					//res += input[(((z + 1) * ysize) + y) * xsize + x];
				//else
					////res += Vbound;
				//if (z > 0)
					////res += input[(((z - 1) * ysize) + y) * xsize + x];
				//else
					//res += Vbound;
			//}
		//}
			
		//// Deal with y = max
		//y = ymax;
		//// iterate through z loop
		//for (unsigned int z = 0; z < ysize; z++) {	
			//// iterate through x loop
			//for (unsigned int x = 0; x < zsize; x += xsize) {
				//if (x < xsize - 1)
					//res += input[((z * ysize) + y) * xsize + (x + 1)];
				//else
					//res += Vbound;
				//if (x > 0)
					//res += input[((z * ysize) + y) * xsize + (x - 1)];
				//else
					//res += Vbound;

				////if (y < ysize - 1)
					////res += input[((z * ysize) + (y + 1)) * xsize + x];
				////else
					//res += Vbound;
				////if (y > 0)
					//res += input[((z * ysize) + (y - 1)) * xsize + x];
				////else
					////res += Vbound;

				//if (z < zsize - 1)
					//res += input[(((z + 1) * ysize) + y) * xsize + x];
				//else
					//res += Vbound;
				//if (z > 0)
					//res += input[(((z - 1) * ysize) + y) * xsize + x];
				//else
					//res += Vbound;
			//}
		//}

		//~ memcpy(input, potential, size);
	//~ }
	//~ free(input);
//~ }

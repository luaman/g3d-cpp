/**
 @file g3dmath.cpp
 
 @author Morgan McGuire, graphics3d.com
  
 @created 2001-06-02
 @edited  2003-04-29
 */

#include "G3D/g3dmath.h"
#include <stdlib.h>

namespace G3D {


int highestBit(uint32 x) {
    // Binary search.
    int base = 0;

	if (x & 0xffff0000)	{
		base = 16;
		x >>= 16;
	}
    if (x & 0x0000ff00) {
		base += 8;
		x >>= 8;
	}
    if (x & 0x000000f0) {
		base += 4;
		x >>= 4;
	}

	static const int lut[] = {-1,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3};
	return base + lut[x];
}


int iRandom(int low, int high) {
	int r = low + (high - low + 1) * rand() / RAND_MAX;

	// There is a *very small* chance of generating
	// a number larger than high.
	if (r > high) {
		return high;
	} else {
		return r;
	}
}


double random(double low, double high) {
	return low + (high - low) * rand() / (double)RAND_MAX;
}

}

#include <stdio.h>
#include <stdbool.h>

#include "live_or_die.h"

bool live_or_die(unsigned const int k,  unsigned const long i, unsigned const char *grid) {
	//printf("check if cell %lu live_or_die in the next generation\n", i);
	int countalive = 0;
	unsigned const int row = i/k;
	unsigned const int col = i%k;

	// Check the 8 neighbors					
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (dx == 0 && dy == 0) continue; // Skip the current cell
			if (grid[(row+dx)*k + (col+dy)] > 0)
				countalive++;
		}
	}
	bool result;
	
	if (grid[i] > 0) // already alive	
		result = ((countalive < 2) || (countalive > 3)) ? 0 : 1;
	else // dead
		result = (countalive == 3) ? 1 : 0;
	return result;
}


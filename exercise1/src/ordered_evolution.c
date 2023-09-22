#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "ordered_evolution.h"
#include "read_write_pgm_image.h"
#include "live_or_die.h"

// For measuring time
#if defined(_OPENMP)
    #define CPU_TIME ({struct  timespec ts; clock_gettime( CLOCK_REALTIME, &ts ),\
		    	    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;})
    #define CPU_TIME_th ({struct  timespec myts; clock_gettime( CLOCK_THREAD_CPUTIME_ID, &myts ),\
		    	    (double)myts.tv_sec + (double)myts.tv_nsec * 1e-9;})
#else
#define CPU_TIME ({struct  timespec ts; clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts ),\
		    (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;})
#endif

#define ALIVE 255
#define DEAD 0

/*
 *     ordered_evolution():   performs the static evolution of the playground
 *     @param
 *     fname:  name of the file containing the initial state of the playground
 *     k:      size of the squre matrix that's going to rapresent the playground
 *     n:      number of generations must be computed
 *     s:      every how many generations save a snapshot
 */

void ordered_evolution(const char *fname, unsigned int k, unsigned const int n, unsigned const int s) {
	printf("ordered evolution\n");
	/*
	unsigned char *grid; 

	unsigned int value;
	unsigned int *maxvalue = &value;

	int i;

	read_pgm_image((void**) &grid, maxvalue, &k, &k, fname);
	
	for (unsigned int t = 1; t < n+1; t++) {
		i = t % (k*k) - 1;
		//printf("calcolo lo stato della cella %u.\n",i);
		grid[i] = (live_or_die(k, i, grid)) ? *maxvalue : 0;
		if (s==0) continue;
		if (t%s == 0) {
			char *snapname = malloc(26*sizeof(char));
			sprintf(snapname, "snaps/snapshot_%06d.pgm", t);
			write_pgm_image((void*) grid, *maxvalue, k, k, snapname);
			free(snapname);
		}
	}
	char *filename = malloc (21*sizeof(char));
	sprintf(filename, "game_of_life_END.pgm");
	write_pgm_image((void*) grid, *maxvalue, k, k, filename);
	free(filename);		
	free(grid);
	 */
	return;
}

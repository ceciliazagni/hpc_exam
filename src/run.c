#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "run.h"
#include "static_evolution.h"
#include "ordered_evolution.h"
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
 *     run():  run the provided playground.
 *     @param
 *     fname:  name of the file used to initialize the the playground
 *     k:      size of the the playground (squared matrixi)
 *     n:      number of generations
 *     s:      save a snapshot every s generations
 *     e:      0: ordered evolution
 *             1: static evolution
 */

void run(const char *fname, unsigned const int k, unsigned const int n, unsigned const int s, const char e) {
	if ( e == 0 ) {
		printf("Ordered evolution.\n");
		ordered_evolution(fname,k,n,s);
		return;
	} else {
		printf("Static evolution.\n");
		static_evolution(fname,k,n,s);
		return;	
	}
	return;
}


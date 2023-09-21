#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "run.h"
#include "static_evolution.h"
#include "ordered_evolution.h"

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
	
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if ( e == 0 ) {
		//if (rank == 0)
		//	printf("Ordered evolution.\n");
		ordered_evolution(fname,k,n,s);
		return;
	} else {
		//if (rank == 0)
		//	printf("Static evolution.\n");
		static_evolution(fname,k,n,s);
		return;	
	}


	return;
}


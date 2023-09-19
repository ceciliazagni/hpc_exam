#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "init.h"
#include "read_write_pgm_image.h"

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
 *     init(): initialize the playground.
 *     @param
 *     fname:  name of the file that's going to be written to initialize the playground
 *     k:      size of the the playground (squared matrix)
 */
void init(const char *fname, unsigned const int k, const int rank, const int size) {	
	int n, r;

	n = k / size; // numero di righe che scrivono i rank > 0
	r = k % size; // n + r numero di righe che il scrive il rank 0

	MPI_Offset* current_position=NULL;
	unsigned int buffer_size;
	buffer_size = n * k *sizeof(char);

	if ( rank == 0) {
		write_pgm_image_header(ALIVE, k, k, current_position, fname);
		printf("header scritto.\n");
		printf("lunghezza dell'header: %lld\n",(long long int)*current_position);
		buffer_size = (n + r) * k *sizeof(char);
	}

	MPI_Barrier(MPI_COMM_WORLD);

	char *grid; 
	grid = (char *)malloc(buffer_size);
	int seed = time(NULL);
        srand(seed);
	// the cell is alive with probability 0.2
	for (unsigned long i = 0; i < buffer_size; i++)
		grid[i] = rand() % 100 < 20 ? ALIVE : DEAD;

	write_pgm_image_body(grid, ALIVE, buffer_size, current_position, fname);

	free(grid);

	return; 
}

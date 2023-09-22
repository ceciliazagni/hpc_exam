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


void ordered_evolution(const char *fname, unsigned int k, unsigned const int n, unsigned const int s) {
	//printf("ordered evolution\n");
	
	unsigned char *grid; 
	
	unsigned int value;
	unsigned int *maxvalue = &value;
	unsigned int offset;
	
	int rank,size;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
								
	//printf("I'm %d e devo leggere l'header\n",rank);								
																			
	read_pgm_image_header(maxvalue, &k, &k, fname, &offset);
							
	//adesso so quanto e' grande header, quanto e' grande k e posso fare mpi io
	int m=k/size;
	int r=k%size;
	int n_rows;
	// leggo l'immagine	
	if(rank==0)
		n_rows = m+r;	
	else
		n_rows = m;

	//printf("I'm %d e ho letto l'header. nrows = %d\n",rank,n_rows);
	
	grid=malloc((n_rows+1)*k*sizeof(char));
	read_pgm_image_body(grid+k,n_rows*k*sizeof(char),offset,fname);

	MPI_Offset current_position = -1;
	int position_integer;

	for (unsigned int t = 0; t < n; t++){
		// rank 0 size-1 passa le ghost cells a rank 0
		if (rank == size-1)
			MPI_Send(grid+n_rows*k, k, MPI_CHAR, 0, 18, MPI_COMM_WORLD);
		if (rank == 0)
			MPI_Recv(grid, k, MPI_CHAR, size-1, 18, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (rank != 0)
			MPI_Recv(grid, k, MPI_CHAR, rank-1, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


		// calcolo la prossima cella
		for (int i = k; i < (n_rows+1)*k; i++) {					
			// controllo gli 8 vicini	 				
			int countalive = 0;
			for (int dx = -1; dx <= 1; dx++) {
				for (int dy = -1; dy <= 1; dy++) {
					if (dx == 0 && dy == 0) continue; // Skip the current cell					
							
					if (grid[((i/k)+dx)*k + (k+(i%k)+dy)%k] > 0)
						countalive++;
				}
			}
			//modifico next_grid		
			if (grid[i] > 0) // already alive	 		 					
				grid[i] = ((countalive < 2) || (countalive > 3)) ? DEAD : ALIVE;
			else // dead
				grid[i] = (countalive == 3) ? ALIVE : DEAD;
		}
		if (rank != size-1)
			MPI_Send(grid+n_rows*k, k, MPI_CHAR, rank+1, 20, MPI_COMM_WORLD);

		// salvo lo snapsho se necessario
		if (s==0) continue;
		if (t%s == 0) {
			char *snapname = malloc(26*sizeof(char));
			sprintf(snapname, "snaps/snapshot_%06d.pgm", t);

			//MPI_Offset current_position = -1;															
			if ( rank == 0)
				write_pgm_image_header(ALIVE, k, k, &current_position, snapname);
			
			position_integer=(int) current_position;						
			MPI_Bcast(&position_integer,1,MPI_INT,0,MPI_COMM_WORLD);
			current_position=(long long int)position_integer + rank*(m*k);
			if (rank > 0)
				current_position+=r*k;											
			MPI_Barrier(MPI_COMM_WORLD);
			write_pgm_image_body(grid+k, ALIVE, n_rows*k*sizeof(char), &current_position, snapname);
			free(snapname);				
		}
	}

	// salvo il risultato in un file pgm
	char *filename = malloc (21*sizeof(char));
	sprintf(filename, "game_of_life_END.pgm");

	if ( rank == 0) 
		write_pgm_image_header(ALIVE, k, k, &current_position, filename);

	position_integer=(int) current_position;
	MPI_Bcast(&position_integer,1,MPI_INT,0,MPI_COMM_WORLD);
	current_position=(long long int)position_integer + rank*(m*k);	
	if (rank > 0)
		current_position+=r*k;
	
	MPI_Barrier(MPI_COMM_WORLD);		
	write_pgm_image_body(grid+k, ALIVE, n_rows*k*sizeof(char), &current_position, filename);
	//printf("FINO QUA TUTTO OK\n");
	
	free(filename);
	free(grid);
	return;

}





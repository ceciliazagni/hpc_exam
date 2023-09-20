#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<time.h>
#include<omp.h>
#include<mpi.h>

#include "static_evolution.h"
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
 *     static_evolution():   performs the static evolution of the playground
 *     @param
 *     fname:  name of the file containing the initial state of the playground
 *     k:      size of the squre matrix that's going to rapresent the playground
 *     n:      number of generations must be computed
 *     s:      every how many generations save a snapshot
 */

void static_evolution(const char *fname, unsigned int k, unsigned const int n, unsigned const int s) {
	//printf("FINO QUA TUTTO OK\n");
	
	unsigned char *grid; 
	unsigned char *next_grid;
	unsigned char *tmp;

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
      		
	grid=malloc((n_rows+2)*k*sizeof(char));
      	next_grid=malloc((n_rows+2)*k*sizeof(char));
      	read_pgm_image_body(grid+k,n_rows*k*sizeof(char),offset,fname);
	
	MPI_Barrier(MPI_COMM_WORLD);

	// mi passo le ghost cells con MPI_Sendrecv
	int rank_sotto = (rank + 1) % size; // rank sopra			      
	int rank_sopra = (rank + size - 1) % size; // rank sotto

	int down = 1; // 0->1->2-> ... ->(size-1)->0
	int up = 2; // (size-1)->(size-2)-> ... ->2->1->0->(size-1)
	
	MPI_Offset current_position = -1;
	int position_integer;

	for (unsigned int t = 0; t < n; t++){
		// send down
		MPI_Sendrecv(grid+n_rows*k, k, MPI_CHAR, rank_sotto, down, grid, k, MPI_CHAR, rank_sopra, down, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// send up		
		MPI_Sendrecv(grid+k, k, MPI_CHAR, rank_sopra, up, grid+(n_rows+1)*k, k, MPI_CHAR, rank_sotto, up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	
		//printf("FINO QUA TUTTO OK\n");
	
		MPI_Barrier(MPI_COMM_WORLD);

		
		// printo la griglia a terminale
		printf("t = %d, I'm rank %d e leggo:", t, rank);
		for (int i = k; i < (n_rows+1)*k; i++) {
			if (i % k == 0)
				printf("\n");
			printf("%d ", grid[i] == ALIVE ? 1 : 0);
		}
		printf("\n");

		// calcolo la prossima generazione
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
				next_grid[i] = ((countalive < 2) || (countalive > 3)) ? DEAD : ALIVE;
			else // dead
				next_grid[i] = (countalive == 3) ? ALIVE : DEAD;
		}

		tmp = grid;
		grid = next_grid;
		next_grid = tmp;

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

	// printo il risultato finale
	printf("t = %d, I'm rank %d e leggo:", n, rank);
	for (int i = k; i < (n_rows+1)*k; i++) {
		if (i % k == 0)
			printf("\n");
		printf("%d ", grid[i] == ALIVE ? 1 : 0);
	}
	printf("\n");


	// salvo il risultato in un file pgm
	char *filename = malloc (21*sizeof(char));
	sprintf(filename, "game_of_life_END.pgm");
	
	//MPI_Offset current_position = -1;
	
	if ( rank == 0) 
		write_pgm_image_header(ALIVE, k, k, &current_position, filename);
	
	position_integer=(int) current_position;
	MPI_Bcast(&position_integer,1,MPI_INT,0,MPI_COMM_WORLD);
	current_position=(long long int)position_integer + rank*(m*k);	
	if (rank > 0)
		current_position+=r*k;	
	
	MPI_Barrier(MPI_COMM_WORLD);

	write_pgm_image_body(grid+k, ALIVE, n_rows*k*sizeof(char), &current_position, filename);
		
	printf("FINO QUA TUTTO OK\n");

	free(filename);
	free(next_grid);
	free(grid);
	
	return;
}

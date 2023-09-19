#include<stdio.h>
#include<stdlib.h>
#include<getopt.h> 
#include<omp.h>
#include<mpi.h>


#include "init.h"
#include "run.h"


#define INIT 1                    // for -i / -r : initialize or run
#define RUN 2

#define ORDERED 0                       // for -e : ordered or static evolution
#define STATIC 1

#define K_DFLT 100                   // for -k : plyaground size
#define N_DFLT 10                    // for -n : number of generations
#define S_DFLT 0                     // for -s : every how many generations save a snapshot
#define F_DFLT "game_of_life.pgm"    // for -f : file name



// variables for command line arguments:
unsigned int action = 0;
unsigned int e = STATIC; 
unsigned int k = K_DFLT;
unsigned int n = N_DFLT;
unsigned int s = S_DFLT;
char *fname = NULL;


int main(int argc, char *argv[]) {
    	char* optstring = "irk:e:f:n:s:";    
    	int c; 
    	while ((c = getopt(argc, argv, optstring)) != -1) {
		switch (c) {
	    		case 'i':
		 		action = INIT; break;
	    		case 'r':
				action = RUN; break;
	    		case 'k':
				k = atoi(optarg); break;
	    		case 'e':
				e = atoi(optarg); break;
	    		case 'f':
				fname = (char*)malloc( sizeof(optarg)+1 );
				sprintf(fname, "%s", optarg);
				break;
	    		case 'n':
				n = atoi(optarg); break;
	    		case 's':
				s = atoi(optarg); break;
	    		default :
				printf("argument -%c not known\n", c );	break;  
		}
    	} 


    	// check about the parameters:

	if (action != INIT && action != RUN) {
		printf("To run the program, the user must specify either -i or -r  option\n");
		return 0;
    	}
    	if (fname == NULL) {
		fname = (char*)malloc( sizeof(char)*17 );     
		sprintf(fname, "%s", F_DFLT);
    	}
	
	// initialize MPI
	int rank, size;	        
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    	// do what the user asked for:
	if (action == INIT) {
    		printf("Initializing a new playground...\n");
    		init(fname, k, rank, size);
    	}
    	if (action == RUN) {
		if (e != ORDERED && e != STATIC) {
			printf("The user must specify either -e 0 or -e 1\n");
			printf("-e 0 means ordered evolution, -e 1 means static evolution\n");
			return 0;
		}
    		printf("Running the provided payground...\n");
    		run(fname, k, n, s, e);
	}
	
	MPI_Finalize();

    	free(fname);

    	return 0;

} 

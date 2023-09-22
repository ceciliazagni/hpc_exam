#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include <ctype.h>



int main(int argc, char** argv) {
	
	MPI_Init(&argc, &argv);

	int max_two_power=17, num_iterations=1000;
	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "g:p:i:")) != -1) {

		switch (c)
		{
			case 'p':

				max_two_power = atoi(optarg);
				break;
			case 'i':
				num_iterations = atoi(optarg);
				break;
			case '?':

				if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:

				abort();
		}
	}
	
	int rank, size, i;
	int buffer_size[max_two_power];

	buffer_size[0] = 0;
	buffer_size[1] = 1;

	for (i = 2; i < max_two_power; i++) 
		buffer_size[i] = buffer_size[i-1]*2;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size%2 != 0) {
		printf("This benchmark requires even number of processes.\n");
		MPI_Finalize();
		return 1;
	}

	int partner;

	if (rank%2 == 0)
		partner = rank+1;
	else
		partner = rank-1;

	if (rank == 0)
		printf("#bytes\t#repetitions\t#t[usec]\t#bandwidth[MB/sec]\t#avg_latency[usec]\t#aggr_bandwidth[MB/sec]\n");

	u_int8_t *send_buf;
	u_int8_t *recv_buf;
	double start_time, end_time, total_time = 0.0;

	for (i = 0; i < max_two_power; i++) {
		total_time = 0.0;
		int iteration, j;

		MPI_Alloc_mem(buffer_size[i], MPI_INFO_NULL, &send_buf);
		MPI_Alloc_mem(buffer_size[i], MPI_INFO_NULL, &recv_buf);

		for (j = 0; j < buffer_size[i]; j++)
			send_buf[j] = 1;
		
		for (iteration = 0; iteration < num_iterations; ++iteration) {
			MPI_Barrier(MPI_COMM_WORLD);
			if((rank % 2) == 0  ) {
				start_time = MPI_Wtime();
				MPI_Send(send_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD);
				MPI_Recv(recv_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				end_time = MPI_Wtime();
			}
			else {
	      			start_time = MPI_Wtime();
      				MPI_Recv(recv_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      				MPI_Send(send_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD);
      				end_time = MPI_Wtime();
			}

			MPI_Barrier(MPI_COMM_WORLD);

                	total_time += (end_time - start_time) / 2;
		}


                double latency = total_time / num_iterations * 1e6;
		double bandwidth = buffer_size[i] / latency;
		double avg_latency, aggr_bandwidth;

		MPI_Reduce(&latency, &avg_latency, 1, MPI_DOUBLE,MPI_SUM,0, MPI_COMM_WORLD);
		avg_latency /=2;

		MPI_Reduce(&bandwidth, &aggr_bandwidth, 1, MPI_DOUBLE,MPI_SUM,0, MPI_COMM_WORLD);
		aggr_bandwidth /=2;

		
		if (rank == 0)
			printf("%d\t%d\t%f\t%f\t%f\t%f\n", buffer_size[i],num_iterations,latency,bandwidth,avg_latency,aggr_bandwidth);

		MPI_Free_mem(send_buf);
		MPI_Free_mem(recv_buf);
	}

        MPI_Finalize();

        return 0;
}

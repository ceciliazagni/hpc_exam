#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <cuda.h>
#include <cublas.h>
#include <cuda_runtime.h>
#include "cublas_v2.h"
#include "mpi-ext.h" /* Needed for CUDA-aware check */
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>

//#define NUM_ITERATIONS 1000
//#define MAX_TWO_POWER 17

#define cudaCheckError() {                                          \
	cudaError_t e=cudaGetLastError();                           \
	if(e!=cudaSuccess) {                                        \
		printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e));           \
		exit(0); \
	}                                                           \
}

int main(int argc, char** argv) {
	
	printf("Compile time check:\n");
#if defined(MPIX_CUDA_AWARE_SUPPORT) && MPIX_CUDA_AWARE_SUPPORT
        printf("This MPI library has CUDA-aware support %d.\n", MPIX_CUDA_AWARE_SUPPORT);
#elif defined(MPIX_CUDA_AWARE_SUPPORT) && !MPIX_CUDA_AWARE_SUPPORT
	printf("This MPI library does not have CUDA-aware support.\n");
#else
	printf("This MPI library cannot determine if there is CUDA-aware support.\n");
#endif /* MPIX_CUDA_AWARE_SUPPORT */
	printf("Run time check:\n");
#if defined(MPIX_CUDA_AWARE_SUPPORT)
	if (1 == MPIX_Query_cuda_support()) {
		printf("This MPI library has CUDA-aware support.\n");
	} else {
		printf("This MPI library does not have CUDA-aware support.\n");
	}
#else /* !defined(MPIX_CUDA_AWARE_SUPPORT) */
	printf("This MPI library cannot determine if there is CUDA-aware support.\n");
#endif /* MPIX_CUDA_AWARE_SUPPORT */

	MPI_Init(&argc, &argv);

	int gpu, max_two_power, num_iterations=1000;
	int c;
	opterr = 0;
	while ((c = getopt (argc, argv, "g:p:i:")) != -1) {

		switch (c)
		{
			case 'g':

				gpu = atoi(optarg);
				if(gpu < 0 || gpu > 7){
					fprintf (stderr, "Required even nu.\n");
					return 1;
				}
				break;
			case 'p':

				max_two_power = atoi(optarg);
				break;
			case 'i':
				num_iterations = atoi(optarg);
				break;
			case '?':

				if (optopt == 'g')
					fprintf (stderr, "Option -g requires an argument.\n");
				else if (optopt == 'p')
					fprintf (stderr, "Option -p requires an argument.\n");
				else if (isprint (optopt))
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

	cudaError_t error;
	cublasHandle_t handle;

	int partner;

	if (rank%2 == 0)
		partner = rank+1;
	else
		partner = rank-1;

	//process binding
	int deviceCount;
	cudaGetDeviceCount (&deviceCount) ;
        cudaSetDevice(gpu);					
        cudaCheckError();
	char processor_name[MPI_MAX_PROCESSOR_NAME];
        int name_len;
        MPI_Get_processor_name(processor_name, &name_len);

       
        int device;
        cudaGetDevice(&device);
        printf("Cuda device %d/%d, rank %d, host %s \n",device,deviceCount,rank,processor_name);
       
        cublasCreate(&handle);
        cudaCheckError();

	if (rank == 0)
		printf("#bytes\t#repetitions\t#t[usec]\t#bandwidth[MB/sec]\t#avg_latency[usec]\t#aggr_bandwidth[MB/sec]\n");

	u_int8_t *host_buf;
	u_int8_t *device_buf;
	double start_time, end_time, total_time = 0.0;

	for (i = 0; i < max_two_power; i++) {
		total_time = 0.0;
		int iteration, j;

		MPI_Alloc_mem(buffer_size[i], MPI_INFO_NULL, &host_buf);

		for (j = 0; j < buffer_size[i]; j++)
			host_buf[j] = 1;
		

		// cudaMalloc a device array
		cudaMalloc((void**)&device_buf, buffer_size[i]);
		cudaCheckError();
		
		// send the created buffer to the device:
		cudaMemcpy(device_buf, host_buf, buffer_size[i], cudaMemcpyHostToDevice);
		for (iteration = 0; iteration < num_iterations; ++iteration) {
			MPI_Barrier(MPI_COMM_WORLD);
			if((rank % 2) == 0  ) {
			start_time = MPI_Wtime();
                    	MPI_Send(device_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD);
                    	MPI_Recv(device_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                     	end_time = MPI_Wtime();
			}
			else {
			  start_time = MPI_Wtime();
			  MPI_Recv(device_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			  MPI_Send(device_buf, buffer_size[i], MPI_CHAR, partner, 0, MPI_COMM_WORLD);
			  end_time = MPI_Wtime();
			}

			MPI_Barrier(MPI_COMM_WORLD);

                	total_time += (end_time - start_time) / 2;
		}


		// download and inspect the result on the host:
		//cudaMemcpy(host_buf, device_buf, buffer_size[i], cudaMemcpyDeviceToHost);

                double latency = total_time / num_iterations * 1e6;
		double bandwidth = buffer_size[i] / latency;
		double avg_latency, aggr_bandwidth;
		
		MPI_Reduce(&latency, &avg_latency, 1, MPI_DOUBLE,MPI_SUM,0, MPI_COMM_WORLD);
		avg_latency /=2;

		MPI_Reduce(&bandwidth, &aggr_bandwidth, 1, MPI_DOUBLE,MPI_SUM,0, MPI_COMM_WORLD);
		
		if (rank == 0)
			printf("%d\t%d\t%f\t%f\t%f\t%f\n", buffer_size[i],num_iterations,latency,bandwidth,avg_latency,aggr_bandwidth/2);

		MPI_Free_mem(host_buf);
		cudaFree(device_buf);
	}

        MPI_Finalize();

        return 0;
}

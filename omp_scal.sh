#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="gol_omp_scal"
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks-per-node 128
#SBATCH --mem=490G
#SBATCH --time=02:00:00

# To know each slurm.out file belongs to which job
date
hostname
whoami

module load openMPI/4.1.5/gnu

# Remove old files if any exist and then compile
make clean
make image
make

export OMP_PLACES=sockets
export OMP_PROC_BIND=close

echo OMP_PLACES=sockets >> timings/omp${SLURM_JOBID}.csv
echo OMP_PROC_BIND=close >> timings/omp${SLURM_JOBID}.csv
echo threads,time_init,time_run >> timings/omp${SLURM_JOBID}.csv

pngfilename=gol${SLURM_JOBID}.pgm

for num_threads in {1..64}
do
	export OMP_NUM_THREADS=$num_threads
	my_time_init=`mpirun -np 2 --map-by socket ./main.x -i -k 10000 -f $pngfilename`
	wait
	my_time=`mpirun -np 2 --map-by socket ./main.x -r -s 0 -n 100 -f $pngfilename`
	echo $num_threads,$my_time_init,$my_time >> timings/omp${SLURM_JOBID}.csv
	make image
done


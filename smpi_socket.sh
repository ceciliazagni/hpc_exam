#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="smpi_gol"
#SBATCH --partition=EPYC
#SBATCH --nodes=3
#SBATCH --exclusive
#SBATCH --ntasks-per-node 128
#SBATCH --mem=490G
#SBATCH --time=06:00:00

# To know each slurm.out file belongs to which job
date
hostname
whoami

module load openMPI/4.1.5/gnu

# Remove old files if any exist and then compile
make clean
make image
make

pgmfilename=gol${SLURM_JOBID}.pgm

export OMP_PLACES=sockets
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=64

echo export OMP_PLACES=sockets >> timings/smpi${SLURM_JOBID}.csv
echo export OMP_PROC_BIND=close >> timings/smpi${SLURM_JOBID}.csv
echo export OMP_NUM_THREADS=64 >> timings/smpi${SLURM_JOBID}.csv

echo k,processes,time_init,time_run >> timings/smpi${SLURM_JOBID}.csv

K=10000
mpirun -np 6 --map-by node ./main.x -i -k $k -f $pgmfilename
wait
for i in {1..6}
do
	my_time=`mpirun -np $i --map-by node --bind-to socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
	echo $i,$my_time >> timings/smpi${SLURM_JOBID}.csv

done

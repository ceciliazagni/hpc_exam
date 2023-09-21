#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="smpi_gol"
#SBATCH --partition=EPYC
#SBATCH --nodes=3
#SBATCH --exclusive
#SBATCH --ntasks-per-node 128
#SBATCH --mem=490G
#SBATCH --time=10:00:00

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

export OMP_PLACES=threads
export OMP_PROC_BIND=close
export OMP_NUM_THREADS=1

echo export OMP_PLACES=threads >> timings/smpi${SLURM_JOBID}.csv
echo export OMP_PROC_BIND=close >> timings/smpi${SLURM_JOBID}.csv
echo export OMP_NUM_THREADS=1 >> timings/smpi${SLURM_JOBID}.csv

echo k,processes,time_run >> timings/smpi${SLURM_JOBID}.csv

k0=1000

for i in {1..128}
do
	k=$(echo "scale=0; sqrt($k0 * $k0 * $i)" | bc -l)
	mpirun -np 128 --map-by core ./main.x -i -k $k -f $pgmfilename
	wait
	my_time=`mpirun -np $i --map-by core ./main.x -r -s 0 -n 100 -f $pgmfilename`
	echo $k,$i,$my_time >> timings/smpi${SLURM_JOBID}.csv
	rm gol${SLURM_JOBID}_END.pgm
done
make image

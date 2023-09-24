#!/bin/bash

#SBATCH --nodes=2
#SBATCH --ntasks-per-node=2
#SBATCH --exclusive
#SBATCH --partition=EPYC
#SBATCH --time=20:00:00


module load openMPI/4.1.5/gnu

node_a=`srun hostname | head -n1`
node_b=`srun hostname | tail -n1`


for i in {0..127}
do
	for j in {0..127}
	do
		echo "rank 0=${node_a} slot=${i}" > myrankfile_${SLURM_JOBID}.txt
		echo "rank 1=${node_b} slot=${j}" >> myrankfile_${SLURM_JOBID}.txt
		latency=`mpirun -np 2 --rankfile myrankfile_${SLURM_JOBID}.txt ./IMB-MPI1 pingpong -iter 1000 -msglog 1 | grep -v '#' | awk '$1 ~ /[0-9]/ { print $3 }' | head -n 1`
		echo $i,$j,$latency >> heatmap_${SLURM_JOBID}.csv
	done
done

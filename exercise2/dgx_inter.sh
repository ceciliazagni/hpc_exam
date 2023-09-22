#!/bin/bash

#SBATCH --nodes=2
#SBATCH --ntasks-per-node=256
#SBATCH --exclusive
#SBATCH --partition=DGX
#SBATCH --time=02:00:00


source /etc/profile.d/modules.sh
module use /u/dssc/ceciza/hpcx-v2.15-gcc-MLNX_OFED_LINUX-5-ubuntu20.04-cuda12-gdrcopy2-nccl2.17-x86_64/modulefiles/
module load hpcx-rebuild
export LD_LIBRARY_PATH=/opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/12.0/lib64/:$LD_LIBRARY_PATH

node_a=`srun hostname | head -n1`
node_b=`srun hostname | tail -n1`

for i in {0..7}
do
	for j in {0..7}
	do
		case $i in
			0 | 1)
				echo "-np 1 --bind-to none taskset -c 48 --host ${node_a} ./spp.x -g $i -p 17 -i 100000" > myappfile_${SLURM_JOBID}.txt
				;;
			2 | 3)
				echo "-np 1 --bind-to none taskset -c 16 --host ${node_a} ./spp.x -g $i -p 17 -i 100000" > myappfile_${SLURM_JOBID}.txt
				;;
			4 | 5)
				echo "-np 1 --bind-to none taskset -c 112 --host ${node_a} ./spp.x -g $i -p 17 -i 100000" > myappfile_${SLURM_JOBID}.txt
				;;
			6 | 7)
				echo "-np 1 --bind-to none taskset -c 80 --host ${node_a} ./spp.x -g $i -p 17 -i 100000" > myappfile_${SLURM_JOBID}.txt
				;;
		esac

		case $j in
			0 | 1)
				echo "-np 1 --bind-to none taskset -c 58 --host ${node_b} ./spp.x -g $j -p 17 -i 100000" >> myappfile_${SLURM_JOBID}.txt
				;;
			2 | 3)
				echo "-np 1 --bind-to none taskset -c 26 --host ${node_b} ./spp.x -g $j -p 17 -i 100000" >> myappfile_${SLURM_JOBID}.txt
				;;
			4 | 5)
				echo "-np 1 --bind-to none taskset -c 122 --host ${node_b} ./spp.x -g $j -p 17 -i 100000" >> myappfile_${SLURM_JOBID}.txt
				;;
			6 | 7)
				echo "-np 1 --bind-to none taskset -c 90 --host ${node_b} ./spp.x -g $j -p 17 -i 100000" >> myappfile_${SLURM_JOBID}.txt
				;;
		esac
		
		latency=`mpirun --app myappfile_${SLURM_JOBID}.txt --mca btl ^openib | awk '$1 ~ /[0-9]/ { print $3 }' | head -n 2 | tail -n 1`
		echo $i,$j,$latency >> heatmap_${SLURM_JOBID}.csv
	done
done

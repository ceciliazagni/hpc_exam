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


for i in {0..7}
do
	for j in {0..7}
	do
		case $i in
			0 | 1)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx001 taskset -c 48 ./spp.x -g $i -p 17 -i 10000" > myappfile_${SLURM_JOBID}.txt
				;;
			2 | 3)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx001 taskset -c 16 ./spp.x -g $i -p 17 -i 10000" > myappfile_${SLURM_JOBID}.txt
				;;
			4 | 5)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx001 taskset -c 112 ./spp.x -g $i -p 17 -i 10000" > myappfile_${SLURM_JOBID}.txt
				;;
			6 | 7)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx001 taskset -c 80 ./spp.x -g $i -p 17 -i 10000" > myappfile_${SLURM_JOBID}.txt
				;;
		esac

		case $j in
			0 | 1)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx002 taskset -c 58 ./spp.x -g $j -p 17 -i 10000" >> myappfile_${SLURM_JOBID}.txt
				;;
			2 | 3)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx002 taskset -c 26 ./spp.x -g $j -p 17 -i 10000" >> myappfile_${SLURM_JOBID}.txt
				;;
			4 | 5)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx002 taskset -c 122 ./spp.x -g $j -p 17 -i 10000" >> myappfile_${SLURM_JOBID}.txt
				;;
			6 | 7)
				echo "-np 1 -x UCX_RNDV_THRESH=1 --bind-to none --host dgx002 taskset -c 90 ./spp.x -g $j -p 17 -i 10000" >> myappfile_${SLURM_JOBID}.txt
				;;
		esac
		
		latency=`mpirun --app myappfile_${SLURM_JOBID}.txt --mca btl ^openib | awk '$1 ~ /[0-9]/ { print $3 }' | head -n 2 | tail -n 1`
		echo $i,$j,$latency >> heatmap_${SLURM_JOBID}.csv
	done
done

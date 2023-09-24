#!/bin/bash

#SBATCH --nodes=2
#SBATCH --ntasks-per-node=256
#SBATCH --exclusive
#SBATCH --mem=400G
#SBATCH --partition=DGX
#SBATCH --time=2:00:00


source /etc/profile.d/modules.sh
module use /u/dssc/ceciza/hpcx-v2.15-gcc-MLNX_OFED_LINUX-5-ubuntu20.04-cuda12-gdrcopy2-nccl2.17-x86_64/modulefiles/
module load hpcx-rebuild
export LD_LIBRARY_PATH=/opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/12.0/lib64/:$LD_LIBRARY_PATH

for i in {1..4}
do
	echo "-np 1 -x UCX_MAX_RNDV_RAILS=$i --bind-to none --host dgx001 taskset -c 48 ./gpp.x -p 32" > myappfile_${SLURM_JOBID}.txt
	echo "-np 1 -x UCX_MAX_RNDV_RAILS=$i --bind-to none --host dgx002 taskset -c 48 ./gpp.x -p 32" >> myappfile_${SLURM_JOBID}.txt
	mpirun --app myappfile_${SLURM_JOBID}.txt --mca btl ^openib >> scatter_${i}_${SLURM_JOBID}.csv
done

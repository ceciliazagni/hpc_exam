#!/bin/bash
source /etc/profile.d/modules.sh && module use /u/dssc/ceciza/hpcx-v2.15-gcc-MLNX_OFED_LINUX-5-ubuntu20.04-cuda12-gdrcopy2-nccl2.17-x86_64/modulefiles/ && module load hpcx-rebuild && export LD_LIBRARY_PATH=/opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/12.0/lib64/:$LD_LIBRARY_PATH

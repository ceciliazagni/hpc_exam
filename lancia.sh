#!/bin/bash
##SBATCH --job-name="gol"
#SBATCH --partition=THIN
#SBATCH --nodes=1
#SBATCH --ntasks=6
#SBATCH --time=02:00:00

np=2      #number of MPI-processes

# To know each slurm.out file belongs to which job
date
hostname
whoami
pwd


# TODO: change the module load commands
#       according to the architecture you are using
module load openMPI/4.1.5/gnu

# Remove old files if any exist and then compile
make clean
# make image
make

# TODO: set environment variables
#       according to your needs

#export OMP_PLACES=cores
#export OMP_PROC_BIND=close
#export OMP_NUM_THREADS=8

k=10     # size of the grid (to be used with -i)
e=1         # evolution: 0=ordered, 1=static (to be used with -r)
s=1         # every how many steps save a snapshot (to be used with -r)
n=15       # number of steps to evolve (to be used with -r)
f=fileToLoad.pbm  # use -f $f to load a initial state from a file or write in the case of -i
                  # note that the length of the file name should not exceed 25 characters

# TODO: set the --map-by and --bind-to options
#       according to your needs

mpirun -np $np ./main.x -i -k $k
#mpirun -np $np ./main.x -r -n $n -s $s -e $e  


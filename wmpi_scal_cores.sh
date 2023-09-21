#!/bin/bash
#SBATCH --no-requeue
#SBATCH --job-name="wmpi_gol"
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

echo export OMP_PLACES=sockets >> timings/wmpi${SLURM_JOBID}.csv
echo export OMP_PROC_BIND=close >> timings/wmpi${SLURM_JOBID}.csv
echo export OMP_NUM_THREADS=64 >> timings/wmpi${SLURM_JOBID}.csv

echo k,processes,time_init,time_run >> timings/wmpi${SLURM_JOBID}.csv

# generate new playground
k=5000
i=1
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image

# generate new playground
k=7071
i=2
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image

# generate new playground
k=8660
i=3
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image

# generate new playground
k=10000
i=4
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image

# generate new playground
k=11180
i=5
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image

# generate new playground
k=12247
i=6
my_time_init=`mpirun -np 1 --map-by socket ./main.x -i -k $k -f $pgmfilename`
wait
my_time=`mpirun -np $i --map-by socket ./main.x -r -s 0 -n 100 -f $pgmfilename`
echo $k,$i,$my_time_init,$my_time >> timings/wmpi${SLURM_JOBID}.csv
make image
















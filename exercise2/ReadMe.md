# Intel pingpong benchmark

## Download
Download Intel pingpong benchmark from official github <a href="https://github.com/intel/mpi-benchmarks"> repository </a>:

`git clone --branch IMB-v2021.3 https://github.com/intel/mpi-benchmarks.git`

And navigate to the correct folder:
`cd mpi-benchmarks/src_c`

## Run
Sbatch the following files to run the benchmark.
- `epyc_intra.sh`: pingpong inside one EPYC node
- `epyc_inter.sh`: pingpong between two EPYC nodes
- `thin_intra.sh`: pingpong inside one THIN node
- `thin_inter.sh`: pingpong between two THIN nodes

The output will be a csv file (`heatmap_jobid.csv`) with the following structure: `core_from`,`core_to`,`latency`.


# My pingpong version

- `mypingpong.c` is my version of the Intel pingpong (+ avg latency and aggr bandwidth), the results are comparable.
- `pingpong_cuda_app.c` is the ping pong to be used with the DGX.

## Compile with cuda aware MPI
To compile, for example, the `pingpong_cuda_app.c` do:
```
srun -N1 -n1 -p DGX --time=0:10:00 --pty bash
source carica.sh
mpicc pingpong_cuda_app.c -o spp.x -I /opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/12.0/include -L /opt/nvidia/hpc_sdk/Linux_x86_64/23.3/cuda/12.0/lib -lcudart -lcublas
exit
```

## Run
This version of the pingpong requires the appfile to run properly:
```
mpirun --app appfile.txt
```

Here an example of a working `appfile.txt`:
```
-np 1 --bind-to none --host dgx001 ./spp.x -g 0 -p 17 -i 10000
-np 1 --bind-to none --host dgx001 ./spp.x -g 1 -p 17 -i 10000
```

Flag meaning:
- `-g` gpu binded
- `-p` power of two for the maximun size of the buffer
- `-i` number of iterations

### Ready to use
Sbatch the following files to run the benchmark.
- `dgx_intra.sh`: pingpong (pp) inside one DGX node (matrix)
- `dgx_intra_single_gpu.sh`: multiple pp between 2 gpu of the same DGX
- `dgx_intra_multi_gpu.sh`: 4 pp between gpus in the same DGX
- `dgx_inter.sh`: pp between two DGX nodes (matrix)
- `dgx_inter_ucxrndv.sh`: pp between two DGX nodes (matrix) with UCX_RNSV_THRESH = 1
- `dgx_inter_scatter.sh`: multiple pp between 2 gpu of two DGX
- `dgx_inter_cpu_cpu.sh`: multiple pp between 2 cpu of two DGX

# Plot the results
In order to plot the results with python, create a conda environment with `requirements.txt`.

### Heatmap
- `heatmap.py` takes as argument the `jobid` of the data to be plotted.


### Scatterplot
The following scripts plot the scatterplots. It is necessary to modify them to insert the name of the file it is necessary to plot.
- `scatter_dgx_intra.py`
- `scatter_dgx_inter.py`

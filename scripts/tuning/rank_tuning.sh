#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N RANK_TUNE
#$ -o RANK_TUNE_out.log
#$ -e RANK_TUNE_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
# export I_MPI_DEBUG=4

for N in 16384 32768 65536 131072; do
    for nleaf in 1024 2048; do
        for tol in 1e-3 1e-5; do
            mpirun -n 160 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L $N \
                   --hss_leaf_size $nleaf --hss_abs_tol $tol
        done
    done
done

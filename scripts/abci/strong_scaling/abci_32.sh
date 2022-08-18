#!/bin/bash
#$ -cwd
#$ -l rt_F=32
#$ -l h_rt=8:00:00
#$ -N S32
#$ -o S32_out.log
#$ -e S32_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
# export I_MPI_DEBUG=4

for N in 262144; do
    for nleaf in 1024; do
        for tol in 1e-8; do
            mpirun -n 1280 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L $N \
                   --hss_leaf_size $nleaf --hss_abs_tol $tol
        done
    done
done

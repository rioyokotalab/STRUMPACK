#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=12:00:00
#$ -N YKS4
#$ -o YKS4_out.log
#$ -e YKS4_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
# export I_MPI_DEBUG=4

for N in 59632; do
    for nleaf in 1024; do
        for tol in 1e-8; do
            mpirun -n 160 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L $N /groups/gca50014/md_data/14908x64.dat \
                   --hss_leaf_size $nleaf --hss_rel_tol $tol
        done
    done
done

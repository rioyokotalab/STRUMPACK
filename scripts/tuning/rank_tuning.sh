#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N RANK_TUNE
#$ -o RANK_TUNE_out.log
#$ -e RANK_TUNE_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
# export I_MPI_DEBUG=4

for nleaf in 1024; do
    for max_rank in 5000 10000 20000 30000 40000 50000 60000; do
        mpirun -n 160 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 65536 \
               --hss_leaf_size $nleaf --hss_max_rank $max_rank
    done
done

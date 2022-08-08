#!/bin/bash
#$ -cwd
#$ -l rt_F=16
#$ -l h_rt=8:00:00
#$ -N STRUMPACK16
#$ -o STRUMPACK16_out.log
#$ -e STRUMPACK16_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3

export OMP_NUM_THREADS=39

for nleaf in 512 1024 2048 4096; do
    echo "---- NLEAF: $nleaf ---- "
    mpirun -n 16 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 65536 \
           --hss_leaf_size $nleaf
done

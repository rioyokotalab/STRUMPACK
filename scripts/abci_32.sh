#!/bin/bash
#$ -cwd
#$ -l rt_F=32
#$ -l h_rt=8:00:00
#$ -N STRUMPACK32
#$ -o STRUMPACK32_out.log
#$ -e STRUMPACK32_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3

export OMP_NUM_THREADS=39

mpirun -n 32 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi T 65536

#!/bin/bash
#$ -cwd
#$ -l rt_F=32
#$ -l h_rt=8:00:00
#$ -N R32
#$ -o R32_out.log
#$ -e R32_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1

mpirun -n 1280 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_STRUMPACK_starsh 262144 1e-9 --hss_rel_tol 1e-8

#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N R4
#$ -o R4_out.log
#$ -e R4_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# scattered assignment of processes.
source /apps/intel/2022.1/itac/2021.5.0/env/vars.sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$VT_ROOT/slib

export OMP_PLACES=cores
export OMP_NUM_THREADS=1

mpirun -n 160 -ppn 40 -f $SGE_JOB_HOSTLIST -trace ./build/test/test_STRUMPACK_starsh 32768 1e-9 --hss_rel_tol 1e-8

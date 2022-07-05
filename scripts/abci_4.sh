#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N TESTMD
#$ -o TESTMD_out.log
#$ -e TESTMD_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

export OMP_PLACES=cores
export OMP_NUM_THREADS=40
# export I_MPI_DEBUG=4

mpirun -n 2 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_STRUMPACK_starsh \
       /groups/gca50014/md_data/57114x1.dat 57114 \
       --hss_rel_tol 1e-9

# mpirun -n 4 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 57114 \
#     /groups/gca50014/md_data/57114x1.dat --hss-verbose

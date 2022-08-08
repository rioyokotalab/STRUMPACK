#!/bin/bash
#$ -cwd
#$ -l rt_F=256
#$ -l h_rt=3:00:00
#$ -N STRUM256
#$ -o STRUM256_out.log
#$ -e STRUM256_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune


export OMP_PLACES=cores
export OMP_NUM_THREADS=40

mpirun -n 256 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_STRUMPACK_starsh \
       /groups/gca50014/md_data/57114x64.dat 3655296 \
       --hss_rel_tol 1e-9

#!/bin/bash
#$ -cwd
#$ -l rt_F=8
#$ -l h_rt=3:00:00
#$ -N R8
#$ -o R8_out.log
#$ -e R8_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

source /apps/intel/2022.1/itac/2021.5.0/env/vars.sh
# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1

mpirun -n 320 -ppn 40 -f $SGE_JOB_HOSTLIST -trace -print-rank-map ./build/test/test_STRUMPACK_starsh 65536 1e-9 --hss_rel_tol 1e-8
traceanalyzer --cli --messageprofile test_STRUMPACK_starsh.stf > P8_message_profile.csv
#rm test_STRUMPACK_starsh.stf*

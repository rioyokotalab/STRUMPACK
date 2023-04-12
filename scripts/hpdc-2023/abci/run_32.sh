#!/bin/bash
#$ -cwd
#$ -l rt_F=32
#$ -l h_rt=4:00:00
#$ -N R32
#$ -o R32_out.log
#$ -e R32_err.log

source ~/.bashrc

module purge
module load gcc/11.2.0 intel-mpi/2021.5 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

source /apps/intel/2022.1/itac/2021.5.0/env/vars.sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$VT_ROOT/slib

# scattered assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1

mpirun -n 1280 -ppn 40 -f $SGE_JOB_HOSTLIST ./build/test/test_STRUMPACK_starsh 262144 1e-9 --hss_rel_tol 1e-8
traceanalyzer --cli --messageprofile test_STRUMPACK_starsh.stf > P32_message_profile.csv
rm test_STRUMPACK_starsh.stf*

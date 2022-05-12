#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N vtunen4
#$ -o vtunen4_out.log
#$ -e vtunen4_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

export OMP_PLACES=cores
export OMP_NUM_THREADS=40
# export I_MPI_PIN_DOMAIN=socket

# mpirun -n 2 -ppn 2 -f $SGE_JOB_HOSTLIST -genv I_MPI_PIN_DOMAIN socket amplxe-cl -collect hpc-performance -r vtune_mpi -- ./build/test/test_HSS_mpi T 8192
# mpirun -n 4 -ppn 2 -f $SGE_JOB_HOSTLIST -genv I_MPI_PIN_DOMAIN socket ./build/test/test_HSS_mpi T 65536
mpirun -n 4 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 8192
mpirun -n 4 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 16384
mpirun -n 4 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 32768

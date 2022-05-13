#!/bin/bash
#$ -cwd
#$ -l rt_F=4
#$ -l h_rt=8:00:00
#$ -N proc_bunch
#$ -o proc_bunch_out.log
#$ -e proc_bunch_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

# bunched assignment of processes.

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
export I_MPI_DEBUG=4
export I_MPI_PIN_PROCESSOR_LIST=all:map=bunch

for nleaf in 512 1024 2048 4096; do
    echo "---- NLEAF: $nleaf ---- "
    mpirun -n 160 -ppn 40 -f $SGE_JOB_HOSTLIST -genv I_MPI_PIN_PROCESSOR_LIST all:map=bunch \
           ./build/test/test_HSS_mpi L 16384 \
           --hss_leaf_size $nleaf
done
# mpirun -n 4 -ppn 1 -f $SGE_JOB_HOSTLIST ./build/test/test_HSS_mpi L 32768

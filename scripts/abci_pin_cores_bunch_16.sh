#!/bin/bash
#$ -cwd
#$ -l rt_F=16
#$ -l h_rt=8:00:00
#$ -N PINBUNCH16
#$ -o PINBUNCH16_out.log
#$ -e PINBUNCH16_err.log

source ~/.bashrc

module purge
module load intel-mpi/2021.5 gcc/11.2.0 intel-mkl/2022.0.0 cmake/3.22.3

export OMP_PLACES=cores
export OMP_NUM_THREADS=1
export I_MPI_DEBUG=4
export I_MPI_PIN_PROCESSOR_LIST=all:map=bunch

for nleaf in 512 1024 2048 4096; do
    echo "---- NLEAF: $nleaf ---- "
    mpirun -n 640 -ppn 40 -f $SGE_JOB_HOSTLIST \
           -genv I_MPI_PIN_CELL core \
           -genv I_MPI_PIN_PROCESSOR_LIST allcores:grain=cores:map=bunch \
           ./build/test/test_HSS_mpi L 65536 \
           --hss_leaf_size $nleaf
done

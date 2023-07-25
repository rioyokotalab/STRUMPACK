#!/bin/bash
#YBATCH -r epyc-7502_8
#SBATCH -N 1
#SBATCH -J STRUMPACK
#SBATCH --time=24:00:00

source ~/.bashrc

source /etc/profile.d/modules.sh
module purge
module load gcc/12.2 cuda intel/2022/mkl cmake intel/2022/mpi

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/sameer.deshmukh/gitrepos/gsl-2.7.1/build/lib

export OMP_NUM_THREADS=1

max_rank=100
leaf_size=256
for i in "2 4096" "8 16384" "32 65536" "128 262144"; do
    set -- $i
    cores=$1
    N=$2

    # yukawa
    mpirun -n $cores ./build/test/test_STRUMPACK_starsh $N 1 1e-9 0 2 1e-8 $leaf_size $max_rank
done

#!/bin/bash
#PJM -L "node=1"
#PJM -L "rscgrp=small"
#PJM -L "elapse=2:00:00"
#PJM --llio cn-cache-size=1Gi
#PJM --llio sio-read-cache=on
#PJM -x PJM_LLIO_GFSCACHE=/vol0004
#PJM -x PJM_LLIO_GFSCACHE=/vol0003
#PJM --mpi "proc=4"
#PJM --mpi "max-proc-per-node=4"
#PJM -s

source /vol0004/apps/oss/spack/share/spack/setup-env.sh
# hwloc
spack load /7xx3fha

source ~/.bashrc

# mkdir build
cd build
# cmake ../ \
#       -DCMAKE_C_COMPILER=/opt/FJSVxtclanga/tcsds-1.2.34/bin/fcc \
#       -DCMAKE_CXX_COMPILER=/opt/FJSVxtclanga/tcsds-1.2.34/bin/FCC \
#       -DMPI_C=/opt/FJSVxtclanga/tcsds-1.2.34/bin/mpifcc \
#       -DMPI_CXX=/opt/FJSVxtclanga/tcsds-1.2.34/bin/mpiFCC \
#       -DBLA_VENDOR=Fujitsu_SSL2 \
#       -DBLAS_INCLUDE_DIR=/opt/FJSVxtclanga/tcsds-1.2.34/include \
#       -DCMAKE_BUILD_TYPE=Debug \
#       -DCMAKE_INSTALL_PREFIX=../build \
#       -DTPL_ENABLE_BPACK=ON \
#       -DTPL_ENABLE_COMBBLAS=OFF \
#       -DTPL_BLAS_LIBRARIES="-KSVE -SSL2BLAMP" \
#       -DTPL_LAPACK_LIBRARIES="-lfjlapackexsve_ilp64" \
#       -DTPL_SCALAPACK_LIBRARIES="-SSL2 -lfjscalapacksve /opt/FJSVxtclanga/tcsds-1.2.34/lib64/libCblacs.a" \
#       -DTPL_METIS_INCLUDE_DIRS=/home/hp190122/u01594/gitrepos/metis-5.1.0/build/Linux-aarch64/WD/build/include \
#       -DTPL_METIS_LIBRARIES=/home/hp190122/u01594/gitrepos/metis-5.1.0/build/Linux-aarch64/WD/build/lib/libmetis.a
# make -j install examples

# checking backtraces: https://irp.cdn-website.com/ffe01d68/files/uploaded/D1.%203M1b.%20Software%20of%20HPC%20Systems%20_%20Kento%20Sato.pdf
ulimit -c unlimited
mpiexec -n 4 ./examples/dense/testStructuredMPI


#!/bin/bash

. /vol0004/apps/oss/spack/share/spack/setup-env.sh

export METIS_DIR="$HOME/gitrepos/metis-5.1.0"

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$FJSVXTCLANGA/lib64
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/vol0003/hp190122/u01594/gitrepos/hicma-x-dev/stars-h/build/installdir/lib/pkgconfig
ROOT_FOLDER=$PWD

mkdir build
cd build
export LDFLAGS="$(pkg-config --libs starsh)"
export CXX=mpiFCC
export FX=mpifrt
CXX=mpiFCC FC=mpifrt CC=mpifcc cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
	-DTPL_SCALAPACK_LIBRARIES="-fopenmp -SSL2BLAMP -SCALAPACK" \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh) -Nclang -fopenmp" \
        -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)" \
        -DCMAKE_INSTALL_RPATH="/vol0003/hp190122/u01594/gitrepos/hicma-x-dev/stars-h/build/installdir/lib" \
        -DTPL_METIS_INCLUDE_DIRS=$METIS_DIR/include -DTPL_METIS_LIBRARIES=$METIS_DIR/lib/libmetis.a
make -j install

cd test

/opt/FJSVxtclanga/tcsds-1.2.35/bin/mpiFCC -I/vol0003/hp190122/u01594/gitrepos/hicma-x-dev/stars-h/build/installdir/include  -Nclang -fopenmp -O3 -DNDEBUG -L/vol0003/hp190122/u01594/gitrepos/lorapo/stars-h-rio/build/installdir/lib -lstarsh CMakeFiles/test_STRUMPACK_starsh.dir/test_STRUMPACK_starsh.cpp.o -o test_STRUMPACK_starsh /vol0003/hp190122/u01594/gitrepos/lorapo/stars-h-rio/build/installdir/lib/libstarsh.a  ../libstrumpack.a -fopenmp -SSL2BLAMP -SCALAPACK -SSL2 /home/hp190122/u01594/gitrepos/metis-5.1.0/lib/libmetis.a -lfjprofmpif -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh

# CC=mpifcc CXX=mpiFCC cmake .. -DCMAKE_INSTALL_PREFIX=`pwd`/installdir -DCMAKE_BUILD_TYPE="Release" \
#       -DMPI=OFF -DSTARPU=OFF -DCUDA=OFF -DOPENMP=OFF -DGSL=OFF -DCMAKE_C_COMPILER=mpifcc \
#       -DCMAKE_CXX_COMPILER=mpiFCC -DCMAKE_Fortran_COMPILER=frt \
# -DCMAKE_EXE_LINKER_FLAGS="-L/home/hp190122/u01594/gsl-2.7.1/build/lib -lgsl -lm -L$FJSVXTCLANGA/lib64 -lfjlapacksve -SSL2" \
#       -DCMAKE_C_FLAGS="-Nclang -fPIC -Ofast -I/home/hp190122/u01594/gsl-2.7.1/build/include" \
#       -DBLAS_LIBRARIES="-L/home/hp190122/u01594/gsl-2.7.1/build/lib -lgsl -lm -L$FJSVXTCLANGA/lib64 -lfjlapacksve -SSL2"  \
#       -DCBLAS_DIR="$FJSVXTCLANGA/lib64" \
#       -DCBLAS_LIBRARIES="-L/home/hp190122/u01594/gsl-2.7.1/build/lib -lgsl -lm -L$FJSVXTCLANGA/lib64 -lfjlapacksve -SSL2"

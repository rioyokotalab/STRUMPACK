#!/bin/bash

. /vol0004/apps/oss/spack/share/spack/setup-env.sh

export METIS_DIR="$HOME/gitrepos/metis-5.1.0"

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$HOME/gitrepos/lorapo/stars-h-rio/build/installdir/lib/pkgconfig
ROOT_FOLDER=$PWD

mkdir build
cd build
export LDFLAGS="$(pkg-config --libs starsh)"
export CXX=mpiFCC
export FX=mpifrt
CXX=mpiFCC FC=mpifrt CC=mpifcc cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
	-DTPL_SCALAPACK_LIBRARIES="-fopenmp -SSL2BLAMP -SCALAPACK" \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh) -Nclang -fopenmp" -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)" \
        -DCMAKE_INSTALL_RPATH="$HOME/gitrepos/lorapo/stars-h-rio/build/installdir/lib" \
        -DTPL_METIS_INCLUDE_DIRS=$METIS_DIR/include -DTPL_METIS_LIBRARIES=$METIS_DIR/lib/libmetis.a
make -j install

cd test

/opt/FJSVxtclanga/tcsds-1.2.35/bin/mpiFCC -I/vol0003/hp190122/u01594/gitrepos/lorapo/stars-h-rio/build/installdir/include  -Nclang -fopenmp -O3 -DNDEBUG -L/vol0003/hp190122/u01594/gitrepos/lorapo/stars-h-rio/build/installdir/lib -lstarsh CMakeFiles/test_STRUMPACK_starsh.dir/test_STRUMPACK_starsh.cpp.o -o test_STRUMPACK_starsh $HOME/gitrepos/lorapo/stars-h-rio/build/installdir/lib/libstarsh.a  ../libstrumpack.a -fopenmp -SSL2BLAMP -SCALAPACK -SSL2 /home/hp190122/u01594/gitrepos/metis-5.1.0/lib/libmetis.a -lfjprofmpif -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh

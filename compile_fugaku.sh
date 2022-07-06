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
	-DTPL_SCALAPACK_LIBRARIES="-Kopenmp,SVE -SSL2BLAMP -SCALAPACK" \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh) -Nclang -Kopenmp,SVE" -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)" \
        -DCMAKE_INSTALL_RPATH="$HOME/gitrepos/lorapo/stars-h-rio/build/installdir/lib" \
        -DTPL_METIS_INCLUDE_DIRS=$METIS_DIR/include -DTPL_METIS_LIBRARIES=$METIS_DIR/lib/libmetis.a
make -j install

#!/bin/bash

. /vol0004/apps/oss/spack/share/spack/setup-env.sh

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/hp190122/u01594/gitrepos/lorapo/stars-h-rio/build/installdir/lib/pkgconfig
ROOT_FOLDER=$PWD

mkdir build
cd build
export LDFLAGS="$(pkg-config --libs starsh)"
export CXX=mpiFCC
export FX=mpifrt
CXX=mpiFCC FC=mpifrt CC=mpifcc cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
	-DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh)" -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)" \
      -DCMAKE_INSTALL_RPATH="/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib"


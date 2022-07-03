#!/bin/sh

if [ "$HOSTNAME" == "pieterg-X8DA3" ]; then
  export PATH=/home/pieterg/local/spack/opt/spack/linux-ubuntu20.04-nehalem/gcc-9.3.0/swig-fortran-ci74erxydd2kpo2hqesoqj6k3ktqt564/bin:$PATH
  # export SWIG_LIB=/rnsdhpc/code/src/swig/Lib
fi
if [ "$HOSTNAME" == "cs-it-7098760" ]; then
  # export PATH=/home/pieterg/local/spack/opt/spack/linux-ubuntu20.10-zen2/gcc-10.2.0/swig-fortran-qvx2j2brtnmnpkynu3yaerzogcsxpkjm/bin:$PATH
  export PATH=/home/pieterg/local/spack/opt/spack/linux-ubuntu22.04-zen2/gcc-12.0.1/swig-fortran-icer6klyhz3ttxosp6hascchrjwwfuau/bin/:$PATH
fi
which swig

# Since this "flat" interface simply provides fortran interface function and no
# wrapper code, we can ignore the generated wrap.c file .
exec swig -fortran -outdir . -o /dev/null strumpack.i

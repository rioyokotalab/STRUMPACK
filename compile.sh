
module purge
module load gcc/11.2.0 intel-mpi intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/pkgconfig

rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
      -DTPL_BLAS_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_LAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_SCALAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh)" -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)"


VERBOSE=1 make install
VERBOSE=1 make examples

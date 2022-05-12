
module purge
module load gcc/11.2.0 openmpi/4.1.3 intel-mkl/2022.0.0 cmake/3.22.3

rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
      -DTPL_BLAS_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_LAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_SCALAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -lmkl_scalapack_ilp64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_ilp64 -lgomp -lpthread -lm -ldl"

make -j install
make -j tests
make -j test


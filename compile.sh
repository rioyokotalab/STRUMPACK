module purge
module load gcc/11.2.0 intel-mpi intel-mkl/2022.0.0 cmake/3.22.3 intel-itac intel-vtune

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/pkgconfig

source /apps/intel/2022.1/itac/2021.5.0/env/vars.sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$VT_ROOT/slib


ROOT_FOLDER=$PWD
# rm -rf build
# mkdir build
cd build
export LDFLAGS="$(pkg-config --libs starsh)"

cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
      -DTPL_BLAS_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_LAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_SCALAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl" \
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh) -I/home/apps/intel/2022.1/itac/2021.5.0/include" \
      -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh) -L/home/apps/intel/2022.1/itac/2021.5.0/slib -lVT" \
      -DCMAKE_INSTALL_RPATH="/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib"


make -j install
cd /home/acb10922qh/gitrepos/STRUMPACK/build/test

# test_HSS_mpi
g++ \
      -I/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/include \
      -O3 -DNDEBUG \
      -L/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib -lstarsh \
      -Xlinker --enable-new-dtags -Xlinker -rpath \
      -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib/release -Xlinker \
      -rpath -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib -Xlinker \
      --enable-new-dtags -Xlinker --enable-new-dtags -Xlinker -rpath \
      -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib/release -Xlinker -rpath -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib -Xlinker --enable-new-dtags -L/home/apps/intel/2022.1/mpi/2021.5.1/lib CMakeFiles/test_HSS_mpi.dir/test_HSS_mpi.cpp.o -o test_HSS_mpi /home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/libstarsh.a -L/home/apps/intel/2022.1/itac/2021.5.0/slib -lVT  -Wl,-rpath,/home/acb10922qh/gitrepos/metis-5.1.0/lib ../libstrumpack.a /home/apps/intel/2022.1/mpi/2021.5.1/lib/libmpicxx.so /home/apps/intel/2022.1/mpi/2021.5.1/lib/libmpifort.so /home/apps/intel/2022.1/mpi/2021.5.1/lib/release/libmpi.so /lib64/librt.so /lib64/libdl.so /apps/centos7/gcc/11.2.0/lib64/libgomp.so -lpthread -L/apps/intel/2022.1/mkl/2022.0.2/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl -L/apps/intel/2022.1/mkl/2022.0.2/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl /home/acb10922qh/gitrepos/metis-5.1.0/lib/libmetis.so -lgfortran -lquadmath

# test_STRUMPACK_starsh.
g++ \
      -I/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/include -O3 -I/home/apps/intel/2022.1/itac/2021.5.0/include \
      -DNDEBUG -L/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib \
      -lstarsh -Xlinker --enable-new-dtags -Xlinker -rpath -Xlinker \
      /apps/intel/2022.1/mpi/2021.5.1/lib/release -Xlinker -rpath -Xlinker \
      /apps/intel/2022.1/mpi/2021.5.1/lib -Xlinker --enable-new-dtags \
      -Xlinker --enable-new-dtags -Xlinker -rpath -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib/release \
      -Xlinker -rpath -Xlinker /apps/intel/2022.1/mpi/2021.5.1/lib -Xlinker --enable-new-dtags \
      -L/home/apps/intel/2022.1/mpi/2021.5.1/lib CMakeFiles/test_STRUMPACK_starsh.dir/test_STRUMPACK_starsh.cpp.o -o \
      test_STRUMPACK_starsh /home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib/libstarsh.a -L/home/apps/intel/2022.1/itac/2021.5.0/slib -lVT \
      -Wl,-rpath,/home/acb10922qh/gitrepos/metis-5.1.0/lib ../libstrumpack.a /home/apps/intel/2022.1/mpi/2021.5.1/lib/libmpicxx.so /home/apps/intel/2022.1/mpi/2021.5.1/lib/libmpifort.so /home/apps/intel/2022.1/mpi/2021.5.1/lib/release/libmpi.so /lib64/librt.so /lib64/libdl.so /apps/centos7/gcc/11.2.0/lib64/libgomp.so -lpthread -L/apps/intel/2022.1/mkl/2022.0.2/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl -L/apps/intel/2022.1/mkl/2022.0.2/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl /home/acb10922qh/gitrepos/metis-5.1.0/lib/libmetis.so -lgfortran -lquadmath

make -j install

cd $ROOT_FOLDER/build
make examples

cd $ROOT_FOLDER
mpirun -n 2 ./build/test/test_STRUMPACK_starsh \
       /groups/gca50014/md_data/57114x1.dat 57114 \
       --hss_rel_tol 1e-9
# mpirun -n 4 ./build/test/test_HSS_mpi L 57114 /groups/gca50014/md_data/57114x1.dat --hss_verbose

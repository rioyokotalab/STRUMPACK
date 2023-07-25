source ~/.bashrc
# source /home/sameer.deshmukh/gitrepos/parsec/build/bin/parsec.env.sh

source /etc/profile.d/modules.sh
module purge
module load gcc/12.2 cuda intel/2022/mkl cmake intel/2022/mpi

STARSH_BUILD_DIR=/home/sameer.deshmukh/gitrepos/stars-h/build
MPI_INSTALL_DIR=/mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$STARSH_BUILD_DIR/lib/pkgconfig

ROOT_FOLDER=$PWD
rm -rf build
mkdir build
cd build
export LDFLAGS="$(pkg-config --libs starsh)"

cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/gitrepos/STRUMPACK/build \
      -DTPL_BLAS_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_LAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl" \
      -DTPL_SCALAPACK_LIBRARIES="-L${MKLROOT}/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl" \
      -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(pkg-config --cflags starsh)" \
      -DCMAKE_EXE_LINKER_FLAGS="$(pkg-config --libs starsh)" -DTPL_ENABLE_METIS=OFF -DTPL_ENABLE_PARMETIS=OFF -DTPL_ENABLE_PTSCOTCH=OFF -DTPL_ENABLE_SLATE=OFF -DTPL_ENABLE_SCOTCH=OFF \
      -DMETIS_LIBRARIES=/home/sameer.deshmukh/gitrepos/METIS/build/libmetis -DMETIS_INCLUDE_DIR=/home/sameer.deshmukh/gitrepos/METIS/build/xinclude
# \
      # -DCMAKE_INSTALL_RPATH="/home/acb10922qh/gitrepos/lorapo/stars-h-rio/build/installdir/lib"


VERBOSE=1 make -j install

/mnt/nfs/packages/x86_64/gcc/gcc-12.2.0/bin/c++ -I/home/sameer.deshmukh/gitrepos/stars-h/build/include -O3 -DNDEBUG -L/home/sameer.deshmukh/gitrepos/stars-h/build/lib -lstarsh -Xlinker --enable-new-dtags -Xlinker -rpath -Xlinker /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib/release -Xlinker -rpath -Xlinker /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib -Xlinker --enable-new-dtags -Xlinker --enable-new-dtags -Xlinker -rpath -Xlinker /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib/release -Xlinker -rpath -Xlinker /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib -Xlinker --enable-new-dtags -L/mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib CMakeFiles/test_STRUMPACK_starsh.dir/test_STRUMPACK_starsh.cpp.o -o test_STRUMPACK_starsh /home/sameer.deshmukh/gitrepos/stars-h/build/lib/libstarsh.a  ../libstrumpack.a /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib/libmpicxx.so /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib/libmpifort.so /mnt/nfs/packages/x86_64/intel/2022/mpi/2021.6.0/lib/release/libmpi.so /lib/x86_64-linux-gnu/librt.so /lib/x86_64-linux-gnu/libdl.so /mnt/nfs/packages/x86_64/gcc/gcc-12.2.0/lib64/libgomp.so /lib/x86_64-linux-gnu/libpthread.so -L/mnt/nfs/packages/x86_64/intel/2022/mkl/2022.1.0/lib/intel64 -lmkl_scalapack_lp64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lmkl_blacs_intelmpi_lp64 -lgomp -lpthread -lm -ldl -L/mnt/nfs/packages/x86_64/intel/2022/mkl/2022.1.0/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl -lgfortran -lquadmath -L/home/sameer.deshmukh/gitrepos/gsl-2.7.1/build/lib -lgsl

make -j install

cd $ROOT_FOLDER/build
make examples

cd $ROOT_FOLDER
mpirun -n 2 ./build/test/test_STRUMPACK_starsh \
       /groups/gca50014/md_data/57114x1.dat 57114 \
       --hss_rel_tol 1e-9
# mpirun -n 4 ./build/test/test_HSS_mpi L 57114 /groups/gca50014/md_data/57114x1.dat --hss_verbose

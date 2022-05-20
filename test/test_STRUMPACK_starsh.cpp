#include <cmath>
#include <iostream>
#include <chrono>
using namespace std;

#include "dense/DistributedMatrix.hpp"
#include "HSS/HSSMatrixMPI.hpp"

#include <starsh-randtlr.h>
#include <starsh-electrodynamics.h>
#include <starsh-spatial.h>
#include <starsh-rbf.h>

extern "C" {
#include <starsh-fugaku_gc.h>
}

using namespace strumpack;
using namespace strumpack::HSS;

#define ERROR_TOLERANCE 1e1
#define SOLVE_TOLERANCE 1e-12

int ndim;
STARSH_kernel *s_kernel;
STARSH_laplace *starsh_data;
STARSH_int * starsh_index;

int run(int argc, char* argv[]) {
  int m = 150;
  int n = 1;

  HSSOptions<double> hss_opts;
  hss_opts.set_verbose(false);

  enum STARSH_PARTICLES_PLACEMENT place = STARSH_PARTICLES_UNIFORM;
  double add_diag = 1e-8;

  auto usage = [&]() {
    if (!mpi_rank()) {
      cout << "# Usage:\n"
           << "#     OMP_NUM_THREADS=4 ./test_HSS_mpi"
           << " problem options [HSS Options]\n"
           << "# where:\n"
           << "#  - problem: a char that can be\n"
           << "#      'T': solve a Toeplitz problem\n"
           << "#            options: m (matrix dimension)\n"
           << "#      'f': read matrix from file (binary)\n"
           << "#            options: filename\n";
      hss_opts.describe_options();
    }
    exit(1);
  };

  BLACSGrid grid(MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  return 0;
}


int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  if (!mpi_rank()) {
    cout << "# Running with:\n# ";
#if defined(_OPENMP)
    cout << "OMP_NUM_THREADS=" << omp_get_max_threads()
         << " mpirun -n " << mpi_nprocs() << " ";
#else
    cout << "mpirun -n " << mpi_nprocs() << " ";
#endif
    for (int i=0; i<argc; i++) cout << argv[i] << " ";
    cout << endl;
  }

  int ierr;
#pragma omp parallel
#pragma omp single nowait
  ierr = run(argc, argv);

  scalapack::Cblacs_exit(1);
  MPI_Finalize();

  return ierr;
}

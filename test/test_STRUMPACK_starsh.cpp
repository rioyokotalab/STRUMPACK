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


#include "structured/StructuredMatrix.hpp"
#include "iterative/IterativeSolversMPI.hpp"

using namespace strumpack;
using namespace strumpack::HSS;

#define ERROR_TOLERANCE 1e1
#define SOLVE_TOLERANCE 1e-12

int ndim;
STARSH_kernel *s_kernel;
STARSH_laplace *starsh_data;
STARSH_int * starsh_index;

int run(int argc, char* argv[]) {
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

  char * md_file_name = argv[1];
  fprintf(stderr, "using file %s for data.\n", md_file_name);

  int64_t ndim = 3;
  STARSH_int N = std::atol(argv[2]);
  starsh_data = (STARSH_laplace*)malloc(sizeof(STARSH_laplace));

  starsh_data->N = N;
  starsh_data->PV = 1e-8;
  starsh_data->ndim = ndim;
  s_kernel = starsh_laplace_block_kernel;
  starsh_file_grid_read_kmeans(md_file_name,
                               &starsh_data->particles,
                               N, ndim);

  starsh_index = (STARSH_int*)malloc( N * sizeof(STARSH_int) );
  for(STARSH_int i = 0; i < N; ++i)
    starsh_index[i] = i;

  // Define an options object, set to the default options.
  structured::StructuredOptions<double> options;
  // Suppress some output
  options.set_verbose(false);
  options.set_from_command_line(argc, argv);
  options.set_type(structured::Type::HSS);
  // user defined matrix-vector multiplication routine. This routine
  // expects you to perform and return the product of an entire matrix
  // vector product. Not a partial product.
  auto Amult2d =
    [](Trans t,
       const DistributedMatrix<double>& R,
       DistributedMatrix<double>& S) {
      // simply call PxGEMM using A2d
      // gemm(t, Trans::N, double(1.), A2d, R, double(0.), S);
      // A2d.mult(t, R, S); // same as gemm above

      // This lambda should output a full-matvec for a given process.
      // A2d * R = S.
      // generate A2d blocks.
      // loop over all the rows of A2d.
      // rowblocks, colblocks -> row matrix dim / block size
    };

  // auto extract_laplace =
  //   [](std::size_t i, std::size_t j) {
  //     return starsh_laplace_point_kernel(starsh_index + i,
  //                                        starsh_index + j,
  //                                        starsh_data,
  //                                        starsh_data);
  //   };

  auto extract_laplace_block =
    [](const std::vector<std::size_t>& I,
       const std::vector<std::size_t>& J,
       DistributedMatrix<double>& block) {
      for (int i = 0; i < I.size(); ++i) {
        for (int j = 0; j < J.size(); ++j) {
          double value = func(I[i], J[j]);
          block.global(i, j, value);
        }
        std::cout << I[i] << std::endl;
      }
    };

  std::cout << "start HSS construction\n";
  auto Hmat = structured::construct_partially_matrix_free<double>(&grid,
                                                                  (int)N,
                                                                  (int)N,
                                                                  Amult2d,
                                                                  extract_laplace_block,
                                                                  options);

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

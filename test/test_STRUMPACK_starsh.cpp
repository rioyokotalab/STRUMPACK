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
STARSH_matern *starsh_matern;
STARSH_laplace *starsh_laplace;
STARSH_yukawa *starsh_yukawa;
STARSH_int * starsh_index;

double matern_kernel(int i, int j) {
  return starsh_matern_point_kernel(starsh_index + i, starsh_index + j,
                                    starsh_matern, starsh_matern);
}

double laplace_kernel(int i, int j) {
  return starsh_laplace_point_kernel(starsh_index + i, starsh_index + j,
                                     starsh_laplace, starsh_laplace);
}

double yukawa_kernel(int i, int j) {
  return starsh_yukawa_point_kernel(starsh_index + i, starsh_index + j,
                                    starsh_yukawa, starsh_yukawa);
}

int run(int argc, char* argv[]) {
  int n = 1;

  MPIComm c;
  HSSOptions<double> hss_opts;
  hss_opts.set_verbose(false);

  strumpack::structured::extract_t<double> kernel_function;

  enum STARSH_PARTICLES_PLACEMENT place = STARSH_PARTICLES_UNIFORM;

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

  int64_t ndim = 2;
  STARSH_int N = std::atol(argv[1]);
  double param_1 = std::atof(argv[2]);
  double param_2 = std::atof(argv[3]);
  double param_3 = std::atof(argv[4]);
  int kernel_choice = std::atoi(argv[5]);


  switch(kernel_choice) {
  case 0:                       // laplace
    starsh_laplace = (STARSH_laplace*)malloc(sizeof(STARSH_laplace));
    starsh_laplace->N = N;
    starsh_laplace->ndim = ndim;
    starsh_laplace->PV = param_1;
    starsh_laplace_grid_generate(&starsh_laplace, N, ndim, param_1, place);
    kernel_function = laplace_kernel;
    break;
  case 1:                       // matern
    starsh_matern = (STARSH_matern*)malloc(sizeof(STARSH_matern));
    starsh_matern->N = N;
    starsh_matern->ndim = ndim;
    starsh_matern->sigma = param_1;
    starsh_matern->nu = param_2;
    starsh_matern->smoothness = param_3;
    starsh_matern_grid_generate(&starsh_matern, N, ndim, param_1, param_2, param_3, place);
    kernel_function = matern_kernel;
    break;
  case 2:                       // yukawa
    starsh_yukawa = (STARSH_yukawa*)malloc(sizeof(STARSH_yukawa));
    starsh_yukawa->N = N;
    starsh_yukawa->ndim = ndim;
    starsh_yukawa->alpha = param_1;
    starsh_yukawa->singularity = param_2;
    starsh_yukawa_grid_generate(&starsh_yukawa, N, ndim, param_1, param_2, place);
    kernel_function = yukawa_kernel;
    break;
  }


  starsh_index = (STARSH_int*)malloc( N * sizeof(STARSH_int) );
  for(STARSH_int i = 0; i < N; ++i) {
    starsh_index[i] = i;
  }

  // Define an options object, set to the default options.
  structured::StructuredOptions<double> options;
  // Suppress some output
  options.set_verbose(false);
  options.set_from_command_line(argc, argv);
  options.set_type(structured::Type::HSS);

  if (!mpi_rank()) {
    std::cout << "start HSS construction NDIM=" << ndim << std::endl;
  }

  auto begin_construct = std::chrono::system_clock::now();
  auto HSS_matrix = strumpack::structured::construct_from_elements<double>(
    c,
    &grid,
    N,
    N,
    kernel_function,
    options
  );
  free(starsh_index);
  auto end_construct = std::chrono::system_clock::now();

  auto HSS_rank_pre_factorization = HSS_matrix.get()->rank();

  DistributedMatrix<double> B(&grid, N, 1), X(&grid, N, 1);
  X.random();
  HSS_matrix.get()->mult(Trans::N, X, B);

  if (!mpi_rank()) {
    std::cout << "start HSS factor.\n";
  }

  auto begin_factor = std::chrono::system_clock::now();
  HSS_matrix.get()->factor();
  auto end_factor = std::chrono::system_clock::now();

  auto HSS_rank_post_factorization = HSS_matrix.get()->rank();

  if (!mpi_rank()) {
    std::cout << "start HSS solve.\n";
  }

  auto begin_solve = std::chrono::system_clock::now();
  HSS_matrix.get()->solve(B);
  auto end_solve = std::chrono::system_clock::now();


  double construct_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(end_construct - begin_construct).count();
  double factor_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(end_factor - begin_factor).count();
  double solve_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(end_solve - begin_solve).count();

  double solve_error = B.scaled_add(-1., X).normF() / X.normF();

  MPI_Barrier(MPI_COMM_WORLD);

  if (!mpi_rank()) {
    std::cout << "RESULT: np-- " << mpi_nprocs()
              << " --N " << N
              << " --params " << param_1 << "," << param_2 << "," << param_3
              << " --solve_error " << solve_error
              << " --construct_time " << construct_time
              << " --factor_time " << factor_time
              << " --solve_time " << solve_time
              << " --pre_factor_rank " << HSS_rank_pre_factorization
              << " --post_factor_rank " << HSS_rank_post_factorization
	      << " --ndim " << ndim
              << ""
              << std::endl;
  }


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

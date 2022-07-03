/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The
nn * Regents of the University of California, through Lawrence Berkeley
 * National Laboratory (subject to receipt of any required approvals
 * from the U.S. Dept. of Energy).  All rights reserved.
 *
 * If you have questions about your rights to use or distribute this
 * software, please contact Berkeley Lab's Technology Transfer
 * Department at TTD@lbl.gov.
 *
 * NOTICE. This software is owned by the U.S. Department of Energy. As
 * such, the U.S. Government has been granted for itself and others
 * acting on its behalf a paid-up, nonexclusive, irrevocable,
 * worldwide license in the Software to reproduce, prepare derivative
 * works, and perform publicly and display publicly.  Beginning five
 * (5) years after the date permission to assert copyright is obtained
 * from the U.S. Department of Energy, and subject to any subsequent
 * five (5) year renewals, the U.S. Government is granted for itself
 * and others acting on its behalf a paid-up, nonexclusive,
 * irrevocable, worldwide license in the Software to reproduce,
 * prepare derivative works, distribute copies to the public, perform
 * publicly and display publicly, and to permit others to do so.
 *
 * Developers: Pieter Ghysels, Francois-Henry Rouet, Xiaoye S. Li.
 *             (Lawrence Berkeley National Lab, Computational Research
 *             Division).
 *
 */
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
STARSH_molecules *starsh_data;
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
  DistributedMatrix<double> A;

  auto start_init = std::chrono::system_clock::now();
  char test_problem = 'T';
  if (argc > 1) test_problem = argv[1][0];
  else usage();
  switch (test_problem) {
  case 'T': { // Toeplitz
    if (argc > 2) m = stoi(argv[2]);
    if (argc <= 2 || m < 0) {
      cout << "# matrix dimension should be positive integer" << endl;
      usage();
    }
    A = DistributedMatrix<double>(&grid, m, m);
    // TODO only loop over local rows and columns, get the global coordinate..
    for (int j=0; j<m; j++)
      for (int i=0; i<m; i++)
        A.global(i, j, (i==j) ? 1. : 1./(1+abs(i-j)));
  } break;
  case 'L': {
    if (argc > 2) m = stoi(argv[2]);
    if (argc <= 2 || m < 0) {
      cout << "# matrix dimension should be positive integer" << endl;
      usage();
    }
    A = DistributedMatrix<double>(&grid, m, m);
    ndim = 3;
    // s_kernel = starsh_laplace_block_kernel ;
    STARSH_int starsh_n = (STARSH_int)m;
    starsh_data = (STARSH_molecules*)malloc(sizeof(STARSH_molecules));
    starsh_data->N = m;
    starsh_data->ndim = ndim;
    starsh_file_grid_read_kmeans(argv[3],
                                 &(starsh_data->particles),
                                 m, ndim);
    starsh_index = (STARSH_int*)malloc(sizeof(STARSH_int) * m);
    for (int j = 0; j < m; ++j) {
      starsh_index[j] = j;
    }

    for (STARSH_int i = 0; i < m; ++i) {
      for (STARSH_int j = 0; j < m; ++j) {
        double value = starsh_yukawa_point_kernel(starsh_index + i, starsh_index + j,
                                                   starsh_data, starsh_data);
        A.global(i, j, value);
      }
    }
    free(starsh_index);
  } break;
  case 'f': { // matrix from a file
    DenseMatrix<double> Aseq;
    if (!mpi_rank()) {
      string filename;
      if (argc > 2) filename = argv[2];
      else {
        cout << "# specify a filename" << endl;
        usage();
      }
      cout << "Opening file " << filename << endl;
      ifstream file(filename, ifstream::binary);
      file.read(reinterpret_cast<char*>(&m), sizeof(int));
      Aseq = DenseMatrix<double>(m, m);
      file.read(reinterpret_cast<char*>(Aseq.data()), sizeof(double)*m*m);
    }
    MPI_Bcast(&m, 1, mpi_type<int>(), 0, MPI_COMM_WORLD);
    A = DistributedMatrix<double>(&grid, m, m);
    A.scatter(Aseq);
  } break;
  default:
    usage();
    exit(1);
  }
  hss_opts.set_from_command_line(argc, argv);
  auto stop_init = std::chrono::system_clock::now();
  double init_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(stop_init - start_init).count();

  if (hss_opts.verbose()) A.print("A");
  if (!mpi_rank()) cout << "# tol = " << hss_opts.rel_tol() << endl;

  auto start_compress = std::chrono::system_clock::now();
  HSSMatrixMPI<double> H(A, hss_opts);
  auto stop_compress = std::chrono::system_clock::now();
  double compress_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(stop_compress - start_compress).count();
  if (H.is_compressed()) {
    if (!mpi_rank()) {
      cout << "# created H matrix of dimension "
           << H.rows() << " x " << H.cols()
           << " with " << H.levels() << " levels" << endl;
      cout << "# compression succeeded!" << endl;
    }
  } else {
    if (!mpi_rank()) cout << "# compression failed!!!!!!!!" << endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  auto Hrank = H.max_rank();
  auto Hmem = H.total_memory();
  auto Amem = A.total_memory();
  if (!mpi_rank()) {
    cout << "# rank(H) = " << Hrank << endl;
    cout << "# memory(H) = " << Hmem/1e6 << " MB, "
         << 100. * Hmem / Amem << "% of dense" << endl;
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (!mpi_rank()) cout << "# computing ULV factorization of HSS matrix .. ";
  auto ulv_start = std::chrono::system_clock::now();
  H.factor();
  auto ulv_stop =  std::chrono::system_clock::now();
  auto ulv_time = std::chrono::duration_cast<
    std::chrono::milliseconds>(ulv_stop - ulv_start).count();
  if (!mpi_rank()) cout << "Done!" << endl;

  if (!mpi_rank()) cout << "# solving linear system .." << endl;
  DistributedMatrix<double> B(&grid, m, n);
  B.random();
  DistributedMatrix<double> C(B);
  H.solve(C);

  DistributedMatrix<double> Bcheck(&grid, m, n);
  apply_HSS(Trans::N, H, C, 0., Bcheck);
  Bcheck.scaled_add(-1., B);
  auto Bchecknorm = Bcheck.normF();
  auto Bnorm = B.normF();
  double solve_error = Bchecknorm / Bnorm;
  if (!mpi_rank())
    cout << "# relative error = ||B - H * (H\\B) || _F/||B||_F = "
         << solve_error << endl;
  // if (B.active() && Bchecknorm / Bnorm > SOLVE_TOLERANCE) {
  //   if (!mpi_rank())
  //     cout << "ERROR: ULV solve relative error too big!!" << endl;
  //   MPI_Abort(MPI_COMM_WORLD, 1);
  // }

  if (!mpi_rank()) {
    std::cout << "RESULT: "
              << compress_time << ","
              << init_time << ","
              << solve_error << ","
              << ulv_time << ","
              << Hrank << ","
              << m << ","
              << hss_opts.leaf_size() << ","
              << hss_opts.abs_tol() << ","
              << mpi_nprocs()
              << std::endl;
  }

  if (!mpi_rank()) cout << "# test succeeded, exiting" << endl;
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

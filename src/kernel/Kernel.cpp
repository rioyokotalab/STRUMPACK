/*
 * STRUMPACK -- STRUctured Matrices PACKage, Copyright (c) 2014, The
 * Regents of the University of California, through Lawrence Berkeley
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
#include "KernelRegression.hpp"
#include "Kernel.h"
#if defined(STRUMPACK_USE_MPI)
#include "mpi.h"
#endif

using namespace strumpack;
using namespace strumpack::kernel;
using namespace strumpack::HSS;

template<typename scalar_t>
class STRUMPACKKernelRegression {
public:
  STRUMPACKKernelRegression() {}
  std::unique_ptr<Kernel<scalar_t>> K_;
  scalar_t* diag_ = nullptr;
  bool dist_ = false;
  DenseMatrix<scalar_t> training_, weights_;
#if defined(STRUMPACK_USE_MPI)
  BLACSGrid grid_;
  DistributedMatrix<scalar_t> dweights_;
#endif
};

template<typename scalar_t> STRUMPACKKernel STRUMPACK_create_kernel
(int n, int d, scalar_t* train, scalar_t h, scalar_t lambda, int p, int type) {
  int rank = 0;
#if defined(STRUMPACK_USE_MPI)
  int initialized;
  MPI_Initialized(&initialized);
  if (initialized) MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
  if (!rank)
    std::cout << "# C++, creating kernel: n=" << n << ", d=" << d
              << " h=" << h << " lambda=" << lambda << std::endl;
  auto kernel = new STRUMPACKKernelRegression<scalar_t>();
  kernel->training_ = DenseMatrix<scalar_t>(d, n, train, d);
  switch(type) {
  case 0:
    kernel->K_.reset(new GaussKernel<scalar_t>(kernel->training_, h, lambda));
    break;
  case 1:
    kernel->K_.reset(new LaplaceKernel<scalar_t>(kernel->training_, h, lambda));
    break;
  case 2:
    kernel->K_.reset(new ANOVAKernel<scalar_t>(kernel->training_, h, lambda, p));
    break;
  default: std::cout << "ERROR: Kernel type not recognized!" << std::endl;
  }
  return kernel;
}

template<typename scalar_t> STRUMPACKKernel STRUMPACK_create_Gauss_kernel
(int n, int d, scalar_t* train, scalar_t h, scalar_t lambda) {
  auto kernel = new STRUMPACKKernelRegression<scalar_t>();
  kernel->training_ = DenseMatrix<scalar_t>(d, n, train, d);
  kernel->K_.reset(new GaussKernel<scalar_t>(kernel->training_, h, lambda));
  return kernel;
}
template<typename scalar_t> STRUMPACKKernel STRUMPACK_create_Laplace_kernel
(int n, int d, scalar_t* train, scalar_t h, scalar_t lambda) {
  auto kernel = new STRUMPACKKernelRegression<scalar_t>();
  kernel->training_ = DenseMatrix<scalar_t>(d, n, train, d);
  kernel->K_.reset(new LaplaceKernel<scalar_t>(kernel->training_, h, lambda));
  return kernel;
}
template<typename scalar_t> STRUMPACKKernel STRUMPACK_create_ANOVA_kernel
(int n, int d, scalar_t* train, scalar_t h, scalar_t lambda, int p) {
  auto kernel = new STRUMPACKKernelRegression<scalar_t>();
  kernel->training_ = DenseMatrix<scalar_t>(d, n, train, d);
  kernel->K_.reset(new ANOVAKernel<scalar_t>(kernel->training_, h, lambda, p));
  return kernel;
}
template<typename scalar_t> STRUMPACKKernel STRUMPACK_create_general_kernel
(int n, int d, scalar_t* train,
 void(*Kx)(std::size_t, const std::size_t*,
           std::size_t, const std::size_t*,
           scalar_t*, std::size_t, void*),
 void* data, scalar_t lambda) {
  auto kernel = new STRUMPACKKernelRegression<scalar_t>();
  auto Kelem =
    [Kx,data](const std::vector<std::size_t>& I,
              const std::vector<std::size_t>& J,
              DenseMatrix<scalar_t>& B) -> void {
      Kx(I.size(), I.data(), J.size(), J.data(),
         B.data(), B.ld(), data); };
  if (train) {
    kernel->training_ = DenseMatrix<scalar_t>(d, n, train, d);
    kernel->K_.reset
      (new GeneralKernel<scalar_t>
       (kernel->training_, Kelem, lambda));
  } else
    kernel->K_.reset
      (new GeneralKernel<scalar_t>(n, Kelem, lambda));
  return kernel;
}

template<typename scalar_t> void STRUMPACK_kernel_set_diagonal
(STRUMPACKKernel kernel, scalar_t* D) {
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  KR->diag_ = D;
}

template<typename scalar_t> void STRUMPACK_kernel_fit_HSS
(STRUMPACKKernel kernel, scalar_t* labels, int argc, char* argv[]) {
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  std::vector<scalar_t> vl(labels, labels+KR->K_->n());
  HSSOptions<scalar_t> opts;
  opts.set_verbose(false);
  opts.set_clustering_algorithm(ClusteringAlgorithm::COBBLE);
  opts.set_from_command_line(argc, argv);
  KR->weights_ = KR->K_->fit_HSS(vl, opts);
}

#if defined(STRUMPACK_USE_MPI)
template<typename scalar_t> void STRUMPACK_kernel_fit_HSS_MPI
(STRUMPACKKernel kernel, scalar_t* labels, int argc, char* argv[]) {
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  KR->grid_ = std::move(BLACSGrid(MPIComm(MPI_COMM_WORLD)));
  std::vector<scalar_t> vl(labels, labels+KR->K_->n());
  HSSOptions<scalar_t> opts;
  opts.set_verbose(false);
  opts.set_clustering_algorithm(ClusteringAlgorithm::COBBLE);
  opts.set_from_command_line(argc, argv);
  KR->dweights_ = KR->K_->fit_HSS(KR->grid_, vl, opts);
  KR->dist_ = true;
}

template<typename scalar_t> void STRUMPACK_kernel_fit_HODLR_MPI
(STRUMPACKKernel kernel, scalar_t* labels, int argc, char* argv[]) {
#if defined(STRUMPACK_USE_BPACK)
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  std::vector<scalar_t> vl(labels, labels+KR->K_->n());
  HODLR::HODLROptions<scalar_t> opts;
  opts.set_verbose(false);
  opts.set_clustering_algorithm(ClusteringAlgorithm::COBBLE);
  opts.set_from_command_line(argc, argv);
  KR->weights_ = KR->K_->fit_HODLR(MPI_COMM_WORLD, vl, opts);
#else
  std::cerr << "ERROR: STRUMPACK was not configured with HODLR support."
            << "       Using HSS compression as fallback!!" << std::endl;
  STRUMPACK_kernel_fit_HSS_MPI<scalar_t>(kernel, labels, argc, argv);
#endif
}
#endif

template<typename scalar_t> void STRUMPACK_kernel_predict
(STRUMPACKKernel kernel, int m, scalar_t* test, scalar_t* prediction) {
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  DenseMatrixWrapper<scalar_t> test_(KR->K_->d(), m, test, KR->K_->d());
  if (KR->dist_) {
#if defined(STRUMPACK_USE_MPI)
    auto pred = KR->K_->predict(test_, KR->dweights_);
    std::copy(pred.begin(), pred.end(), prediction);
#endif
  } else {
    auto pred = KR->K_->predict(test_, KR->weights_);
    std::copy(pred.begin(), pred.end(), prediction);
  }
}

template<typename scalar_t> void STRUMPACK_general_kernel_predict
(STRUMPACKKernel kernel, int m,
 void(*Kxy)(std::size_t, const std::size_t*,
            std::size_t, const std::size_t*,
            scalar_t*, std::size_t, void*),
 void* data, scalar_t* prediction) {
  auto KR = static_cast<STRUMPACKKernelRegression<scalar_t>*>(kernel);
  auto D = KR->diag_;
  auto Kelem = [Kxy,data,D]
    (const std::vector<std::size_t>& I,
     const std::vector<std::size_t>& J,
     DenseMatrix<scalar_t>& B) -> void {
                 if (D && I.size()==1 && J.size()==1 && I[0]==J[0]) {
                   B(0, 0) = D[I[0]];
                   return;
                 }
                 Kxy(I.size(), I.data(), J.size(), J.data(),
                     B.data(), B.ld(), data);
               };
  if (KR->dist_) {
#if defined(STRUMPACK_USE_MPI)
    auto pred = KR->K_->predict(m, Kelem, KR->dweights_);
    std::copy(pred.begin(), pred.end(), prediction);
#endif
  } else {
    auto pred = KR->K_->predict(m, Kelem, KR->weights_);
    std::copy(pred.begin(), pred.end(), prediction);
  }
}

#ifdef __cplusplus
extern "C" {
#endif

  STRUMPACKKernel STRUMPACK_create_kernel_double
  (int n, int d, double* train, double h, double lambda, int p, int type) {
    return STRUMPACK_create_kernel<double>(n, d, train, h, lambda, p, type);
  }
  STRUMPACKKernel STRUMPACK_create_kernel_float
  (int n, int d, float* train, float h, float lambda, int p, int type) {
    return STRUMPACK_create_kernel<float>(n, d, train, h, lambda, p, type);
  }

  STRUMPACKKernel STRUMPACK_create_Gauss_kernel_double
  (int n, int d, double* train, double h, double lambda) {
    return STRUMPACK_create_Gauss_kernel<double>(n, d, train, h, lambda);
  }
  STRUMPACKKernel STRUMPACK_create_Laplace_kernel_double
  (int n, int d, double* train, double h, double lambda, int p) {
    return STRUMPACK_create_Laplace_kernel<double>(n, d, train, h, lambda);
  }
  STRUMPACKKernel STRUMPACK_create_ANOVA_kernel_double
  (int n, int d, double* train, double h, double lambda, int p) {
    return STRUMPACK_create_ANOVA_kernel<double>(n, d, train, h, lambda, p);
  }
  STRUMPACKKernel STRUMPACK_create_general_kernel_double
  (int n, int d, double* train,
   void(*Kx)(std::size_t, const std::size_t*,
             std::size_t, const std::size_t*,
             double*, std::size_t, void*),
   void* data, double lambda) {
    return STRUMPACK_create_general_kernel<double>
      (n, d, train, Kx, data, lambda);
  }

  STRUMPACKKernel STRUMPACK_create_Gauss_kernel_float
  (int n, int d, float* train, float h, float lambda) {
    return STRUMPACK_create_Gauss_kernel<float>(n, d, train, h, lambda);
  }
  STRUMPACKKernel STRUMPACK_create_Laplace_kernel_float
  (int n, int d, float* train, float h, float lambda, int p) {
    return STRUMPACK_create_Laplace_kernel<float>(n, d, train, h, lambda);
  }
  STRUMPACKKernel STRUMPACK_create_ANOVA_kernel_float
  (int n, int d, float* train, float h, float lambda, int p) {
    return STRUMPACK_create_ANOVA_kernel<float>(n, d, train, h, lambda, p);
  }
  STRUMPACKKernel STRUMPACK_create_general_kernel_float
  (int n, int d, float* train,
   void(*Kx)(std::size_t, const std::size_t*,
             std::size_t, const std::size_t*,
             float*, std::size_t, void*),
   void* f, float lambda) {
    return STRUMPACK_create_general_kernel<float>
      (n, d, train, Kx, f, lambda);
  }

  void STRUMPACK_destroy_kernel_double(STRUMPACKKernel kernel) {
    delete static_cast<STRUMPACKKernelRegression<double>*>(kernel);
  }
  void STRUMPACK_destroy_kernel_float(STRUMPACKKernel kernel) {
    delete static_cast<STRUMPACKKernelRegression<float>*>(kernel);
  }

  void STRUMPACK_kernel_set_diagonal_double
  (STRUMPACKKernel kernel, double* D) {
    STRUMPACK_kernel_set_diagonal<double>(kernel, D);
  }
  void STRUMPACK_kernel_set_diagonal_float
  (STRUMPACKKernel kernel, float* D) {
    STRUMPACK_kernel_set_diagonal<float>(kernel, D);
  }

  void STRUMPACK_kernel_fit_HSS_double
  (STRUMPACKKernel kernel, double* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HSS<double>(kernel, labels, argc, argv);
  }
  void STRUMPACK_kernel_fit_HSS_float
  (STRUMPACKKernel kernel, float* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HSS<float>(kernel, labels, argc, argv);
  }

#if defined(STRUMPACK_USE_MPI)
  void STRUMPACK_kernel_fit_HSS_MPI_double
  (STRUMPACKKernel kernel, double* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HSS_MPI<double>(kernel, labels, argc, argv);
  }
  void STRUMPACK_kernel_fit_HSS_MPI_float
  (STRUMPACKKernel kernel, float* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HSS_MPI<float>(kernel, labels, argc, argv);
  }

  void STRUMPACK_kernel_fit_HODLR_MPI_double
  (STRUMPACKKernel kernel, double* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HODLR_MPI<double>(kernel, labels, argc, argv);
  }
  void STRUMPACK_kernel_fit_HODLR_MPI_float
  (STRUMPACKKernel kernel, float* labels, int argc, char* argv[]) {
    STRUMPACK_kernel_fit_HODLR_MPI<float>(kernel, labels, argc, argv);
  }
#endif

  void STRUMPACK_kernel_predict_double
  (STRUMPACKKernel kernel, int m, double* test, double* prediction) {
    STRUMPACK_kernel_predict<double>(kernel, m, test, prediction);
  }
  void STRUMPACK_kernel_predict_float
  (STRUMPACKKernel kernel, int m, float* test, float* prediction) {
    STRUMPACK_kernel_predict<float>(kernel, m, test, prediction);
  }

  void STRUMPACK_general_kernel_predict_double
  (STRUMPACKKernel kernel, int m,
   void(*Kxy)(std::size_t, const std::size_t*,
              std::size_t, const std::size_t*,
              double*, std::size_t, void*),
   void* data, double* prediction) {
    STRUMPACK_general_kernel_predict<double>
      (kernel, m, Kxy, data, prediction);
  }
  void STRUMPACK_general_kernel_predict_float
  (STRUMPACKKernel kernel, int m,
   void(*Kxy)(std::size_t, const std::size_t*,
              std::size_t, const std::size_t*,
              float*, std::size_t, void*),
   void* data, float* prediction) {
    STRUMPACK_general_kernel_predict<float>
      (kernel, m, Kxy, data, prediction);
  }

#ifdef __cplusplus
}
#endif

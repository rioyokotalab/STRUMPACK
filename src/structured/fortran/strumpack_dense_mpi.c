/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 4.1.0+fortran
 *
 * This file is not intended to be easily readable and contains a number of
 * coding conventions designed to improve portability and efficiency. Do not make
 * changes to this file unless you know what you are doing--modify the SWIG
 * interface file instead.
 * ----------------------------------------------------------------------------- */


#ifndef SWIGFORTRAN
#define SWIGFORTRAN
#endif

/* -----------------------------------------------------------------------------
 *  This section contains generic SWIG labels for method/variable
 *  declarations/attributes, and other compiler dependent labels.
 * ----------------------------------------------------------------------------- */

/* template workaround for compilers that cannot correctly implement the C++ standard */
#ifndef SWIGTEMPLATEDISAMBIGUATOR
# if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x560)
#  define SWIGTEMPLATEDISAMBIGUATOR template
# elif defined(__HP_aCC)
/* Needed even with `aCC -AA' when `aCC -V' reports HP ANSI C++ B3910B A.03.55 */
/* If we find a maximum version that requires this, the test would be __HP_aCC <= 35500 for A.03.55 */
#  define SWIGTEMPLATEDISAMBIGUATOR template
# else
#  define SWIGTEMPLATEDISAMBIGUATOR
# endif
#endif

/* inline attribute */
#ifndef SWIGINLINE
# if defined(__cplusplus) || (defined(__GNUC__) && !defined(__STRICT_ANSI__))
#   define SWIGINLINE inline
# else
#   define SWIGINLINE
# endif
#endif

/* attribute recognised by some compilers to avoid 'unused' warnings */
#ifndef SWIGUNUSED
# if defined(__GNUC__)
#   if !(defined(__cplusplus)) || (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#     define SWIGUNUSED __attribute__ ((__unused__))
#   else
#     define SWIGUNUSED
#   endif
# elif defined(__ICC)
#   define SWIGUNUSED __attribute__ ((__unused__))
# else
#   define SWIGUNUSED
# endif
#endif

#ifndef SWIG_MSC_UNSUPPRESS_4505
# if defined(_MSC_VER)
#   pragma warning(disable : 4505) /* unreferenced local function has been removed */
# endif
#endif

#ifndef SWIGUNUSEDPARM
# ifdef __cplusplus
#   define SWIGUNUSEDPARM(p)
# else
#   define SWIGUNUSEDPARM(p) p SWIGUNUSED
# endif
#endif

/* internal SWIG method */
#ifndef SWIGINTERN
# define SWIGINTERN static SWIGUNUSED
#endif

/* internal inline SWIG method */
#ifndef SWIGINTERNINLINE
# define SWIGINTERNINLINE SWIGINTERN SWIGINLINE
#endif

/* exporting methods */
#if defined(__GNUC__)
#  if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#    ifndef GCC_HASCLASSVISIBILITY
#      define GCC_HASCLASSVISIBILITY
#    endif
#  endif
#endif

#ifndef SWIGEXPORT
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define SWIGEXPORT
#   else
#     define SWIGEXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(GCC_HASCLASSVISIBILITY)
#     define SWIGEXPORT __attribute__ ((visibility("default")))
#   else
#     define SWIGEXPORT
#   endif
# endif
#endif

/* calling conventions for Windows */
#ifndef SWIGSTDCALL
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   define SWIGSTDCALL __stdcall
# else
#   define SWIGSTDCALL
# endif
#endif

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif

/* Deal with Apple's deprecated 'AssertMacros.h' from Carbon-framework */
#if defined(__APPLE__) && !defined(__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES)
# define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#endif

/* Intel's compiler complains if a variable which was never initialised is
 * cast to void, which is a common idiom which we use to indicate that we
 * are aware a variable isn't used.  So we just silence that warning.
 * See: https://github.com/swig/swig/issues/192 for more discussion.
 */
#ifdef __INTEL_COMPILER
# pragma warning disable 592
#endif


#ifndef SWIGEXTERN
# ifdef __cplusplus
#   define SWIGEXTERN extern
# else
#   define SWIGEXTERN
# endif
#endif


#include <assert.h>
#define SWIG_exception_impl(DECL, CODE, MSG, RETURNNULL) \
 { printf("In " DECL ": " MSG); assert(0); RETURNNULL; }


#include <stdio.h>
#if (defined(_MSC_VER) && (_MSC_VER < 1900)) || defined(__BORLANDC__) || defined(_WATCOM)
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif


/* Support for the `contract` feature.
 *
 * Note that RETURNNULL is first because it's inserted via a 'Replaceall' in
 * the fortran.cxx file.
 */
#define SWIG_contract_assert(RETURNNULL, EXPR, MSG) \
 if (!(EXPR)) { SWIG_exception_impl("$decl", SWIG_ValueError, MSG, RETURNNULL); } 


#define SWIGVERSION 0x040100 
#define SWIG_VERSION SWIGVERSION


#define SWIG_as_voidptr(a) (void *)((const void *)(a)) 
#define SWIG_as_voidptrptr(a) ((void)SWIG_as_voidptr(*a),(void**)(a)) 


#ifdef __STDC_NO_COMPLEX__
#error "This generated file requires C complex number support"
#endif


#include <complex.h>

#define SWIG_ccomplex_construct(REAL, IMAG) ((REAL) + I * (IMAG))


typedef void* CSPStructMat;
#include "../StructuredMatrixMPI.h"

#ifdef __cplusplus
extern "C" {
#endif
int SP_s_struct_from_dense2d_f(CSPStructMat* S, int comm,
                               int rows, int cols, const float* A,
                               int IA, int JA, int* DESCA,
                               const CSPOptions* opts) {
  return SP_s_struct_from_dense2d
    (S, MPI_Comm_f2c(comm), rows, cols, A, IA, JA, DESCA, opts);
}
int SP_d_struct_from_dense2d_f(CSPStructMat* S, int comm,
                               int rows, int cols, const double* A,
                               int IA, int JA, int* DESCA,
                               const CSPOptions* opts) {
  return SP_d_struct_from_dense2d
    (S, MPI_Comm_f2c(comm), rows, cols, A, IA, JA, DESCA, opts);
}
int SP_c_struct_from_dense2d_f(CSPStructMat* S, int comm,
                               int rows, int cols, const float _Complex* A,
                               int IA, int JA, int* DESCA,
                               const CSPOptions* opts) {
  return SP_c_struct_from_dense2d
    (S, MPI_Comm_f2c(comm), rows, cols, A, IA, JA, DESCA, opts);
}
int SP_z_struct_from_dense2d_f(CSPStructMat* S, int comm,
                               int rows, int cols, const double _Complex* A,
                               int IA, int JA, int* DESCA,
                               const CSPOptions* opts) {
  return SP_z_struct_from_dense2d
    (S, MPI_Comm_f2c(comm), rows, cols, A, IA, JA, DESCA, opts);
}
int SP_s_struct_from_elements_mpi_f(CSPStructMat* S, int comm,
                                    int rows, int cols,
                                    float A(int i, int j),
                                    const CSPOptions* opts) {
  return SP_s_struct_from_elements_mpi
    (S, MPI_Comm_f2c(comm), rows, cols, A, opts);
}
int SP_d_struct_from_elements_mpi_f(CSPStructMat* S, int comm,
                                    int rows, int cols,
                                    double A(int i, int j),
                                    const CSPOptions* opts) {
  return SP_d_struct_from_elements_mpi
    (S, MPI_Comm_f2c(comm), rows, cols, A, opts);
}
int SP_c_struct_from_elements_mpi_f(CSPStructMat* S, int comm,
                                    int rows, int cols,
                                    float _Complex A(int i, int j),
                                    const CSPOptions* opts) {
  return SP_c_struct_from_elements_mpi
    (S, MPI_Comm_f2c(comm), rows, cols, A, opts);
}
int SP_z_struct_from_elements_mpi_f(CSPStructMat* S, int comm,
                                    int rows, int cols,
                                    double _Complex A(int i, int j),
                                    const CSPOptions* opts) {
  return SP_z_struct_from_elements_mpi
    (S, MPI_Comm_f2c(comm), rows, cols, A, opts);
}

#ifdef __cplusplus
}
#endif




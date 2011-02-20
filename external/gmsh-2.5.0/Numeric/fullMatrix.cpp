// Gmsh - Copyright (C) 1997-2010 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to <gmsh@geuz.org>.

#include <complex>
#include <string.h>
#include "GmshConfig.h"
#include "fullMatrix.h"
#include "GmshMessage.h"

//#if defined(_MSC_VER)
//#define F77NAME(x) (x)
//#endif

#if !defined(F77NAME)
#define F77NAME(x) (x##_)
#endif

// Specialisation of fullVector/Matrix operations using BLAS and LAPACK

#if defined(HAVE_BLAS)

extern "C" {
  void F77NAME(daxpy)(int *n, double *alpha, double *x, int *incx, double *y, int *incy);
  void F77NAME(dgemm)(const char *transa, const char *transb, int *m, int *n, int *k, 
                      double *alpha, double *a, int *lda, 
                      double *b, int *ldb, double *beta, 
                      double *c, int *ldc);
  void F77NAME(zgemm)(const char *transa, const char *transb, int *m, int *n, int *k, 
                      std::complex<double> *alpha, std::complex<double> *a, int *lda, 
                      std::complex<double> *b, int *ldb, std::complex<double> *beta, 
                      std::complex<double> *c, int *ldc);
  void F77NAME(dgemv)(const char *trans, int *m, int *n, 
                      double *alpha, double *a, int *lda, 
                      double *x, int *incx, double *beta, 
                      double *y, int *incy);
  void F77NAME(zgemv)(const char *trans, int *m, int *n, 
                      std::complex<double> *alpha, std::complex<double> *a, int *lda, 
                      std::complex<double> *x, int *incx, std::complex<double> *beta, 
                      std::complex<double> *y, int *incy);
  void F77NAME(dscal)(int *n, double *alpha,double *x,  int *incx);
  void F77NAME(zscal)(int *n, std::complex<double> *alpha,std::complex<double> *x,  int *incx);
}

template<> 
void fullVector<double>::axpy(const fullVector<double> &x,double alpha)
{
  int M = _r, INCX = 1, INCY = 1;
  F77NAME(daxpy)(&M, &alpha, x._data,&INCX, _data, &INCY);
}

template<> 
void fullMatrix<double>::scale(const double s)
{
  int N = _r * _c;
  int stride = 1;
  double ss = s;
  F77NAME(dscal)(&N, &ss,_data, &stride);
}

template<> 
void fullMatrix<std::complex<double> >::scale(const double s)
{
  int N = _r * _c;
  int stride = 1;
  std::complex<double> ss(s, 0.);
  F77NAME(zscal)(&N, &ss,_data, &stride);
}

template<> 
void fullMatrix<double>::mult(const fullMatrix<double> &b, fullMatrix<double> &c) const
{
  int M = c.size1(), N = c.size2(), K = _c;
  int LDA = _r, LDB = b.size1(), LDC = c.size1();
  double alpha = 1., beta = 0.;
  F77NAME(dgemm)("N", "N", &M, &N, &K, &alpha, _data, &LDA, b._data, &LDB, 
                 &beta, c._data, &LDC);
}

template<> 
void fullMatrix<std::complex<double> >::mult(const fullMatrix<std::complex<double> > &b, 
                                             fullMatrix<std::complex<double> > &c) const
{
  int M = c.size1(), N = c.size2(), K = _c;
  int LDA = _r, LDB = b.size1(), LDC = c.size1();
  std::complex<double> alpha = 1., beta = 0.;
  F77NAME(zgemm)("N", "N", &M, &N, &K, &alpha, _data, &LDA, b._data, &LDB, 
                 &beta, c._data, &LDC);
}

template<> 
void fullMatrix<double>::gemm(const fullMatrix<double> &a, const fullMatrix<double> &b, 
                              double alpha, double beta)
{
  int M = size1(), N = size2(), K = a.size2();
  int LDA = a.size1(), LDB = b.size1(), LDC = size1();
  F77NAME(dgemm)("N", "N", &M, &N, &K, &alpha, a._data, &LDA, b._data, &LDB, 
                 &beta, _data, &LDC);
}

template<> 
void fullMatrix<std::complex<double> >::gemm(const fullMatrix<std::complex<double> > &a, 
                                             const fullMatrix<std::complex<double> > &b, 
                                             std::complex<double> alpha, 
                                             std::complex<double> beta)
{
  int M = size1(), N = size2(), K = a.size2();
  int LDA = a.size1(), LDB = b.size1(), LDC = size1();
  F77NAME(zgemm)("N", "N", &M, &N, &K, &alpha, a._data, &LDA, b._data, &LDB, 
                 &beta, _data, &LDC);
}

template<> 
void fullMatrix<double>::mult(const fullVector<double> &x, fullVector<double> &y) const
{
  int M = _r, N = _c, LDA = _r, INCX = 1, INCY = 1;
  double alpha = 1., beta = 0.;
  F77NAME(dgemv)("N", &M, &N, &alpha, _data, &LDA, x._data, &INCX,
                 &beta, y._data, &INCY);
}

template<> 
void fullMatrix<std::complex<double> >::mult(const fullVector<std::complex<double> > &x, 
                                             fullVector<std::complex<double> > &y) const
{
  int M = _r, N = _c, LDA = _r, INCX = 1, INCY = 1;
  std::complex<double> alpha = 1., beta = 0.;
  F77NAME(zgemv)("N", &M, &N, &alpha, _data, &LDA, x._data, &INCX,
                 &beta, y._data, &INCY);
}

#endif



#if defined(HAVE_LAPACK)

extern "C" {
  void F77NAME(dgesv)(int *N, int *nrhs, double *A, int *lda, int *ipiv,
                      double *b, int *ldb, int *info);
  void F77NAME(dgetrf)(int *M, int *N, double *A, int *lda, int *ipiv, int *info);
  void F77NAME(dgetri)(int *M, double *A, int *lda, int *ipiv, double *work, 
                       int *lwork, int *info);
  void F77NAME(dgesvd)(const char* jobu, const char *jobvt, int *M, int *N,
                       double *A, int *lda, double *S, double* U, int *ldu,
                       double *VT, int *ldvt, double *work, int *lwork, int *info);
  void F77NAME(dgeev)(const char *jobvl, const char *jobvr, int *n, double *a,
                      int *lda, double *wr, double *wi, double *vl, int *ldvl, 
                      double *vr, int *ldvr, double *work, int *lwork, int *info);
}

static void swap(double *a, int inca, double *b, int incb, int n)
{
  double tmp;
  for (int i = 0; i < n; i++, a += inca, b += incb) {
    tmp = (*a);
    (*a) = (*b);
    (*b) = tmp;
  }
}

static void eigSort(int n, double *wr, double *wi, double *VL, double *VR)
{
  // Sort the eigenvalues/vectors in ascending order according to
  // their real part. Warning: this will screw up the ordering if we
  // have complex eigenvalues.
  for (int i = 0; i < n - 1; i++){
    int k = i;
    double ek = wr[i];
    // search for something to swap
    for (int j = i + 1; j < n; j++){
      const double ej = wr[j];
      if(ej < ek){
        k = j;
        ek = ej;
      }
    }
    if (k != i){
      swap(&wr[i], 1, &wr[k], 1, 1);
      swap(&wi[i], 1, &wi[k], 1, 1);
      swap(&VL[n * i], 1, &VL[n * k], 1, n);
      swap(&VR[n * i], 1, &VR[n * k], 1, n);
    }
  }
}

template<> 
bool fullMatrix<double>::eig(fullVector<double> &DR, fullVector<double> &DI,
                             fullMatrix<double> &VL, fullMatrix<double> &VR,
                             bool sortRealPart)
{
  int N = size1(), info;
  int lwork = 10 * N;
  double *work = new double[lwork];
  F77NAME(dgeev)("V", "V", &N, _data, &N, DR._data, DI._data,
                 VL._data, &N, VR._data, &N, work, &lwork, &info);
  delete [] work;

  if(info > 0)
    Msg::Error("QR Algorithm failed to compute all the eigenvalues", info, info);
  else if(info < 0)
    Msg::Error("Wrong %d-th argument in eig", -info);
  else if(sortRealPart) 
    eigSort(N, DR._data, DI._data, VL._data, VR._data);
  
  return true;
}

template<> 
bool fullMatrix<double>::luSolve(const fullVector<double> &rhs, fullVector<double> &result)
{
  int N = size1(), nrhs = 1, lda = N, ldb = N, info;
  int *ipiv = new int[N];
  for(int i = 0; i < N; i++) result(i) = rhs(i);
  F77NAME(dgesv)(&N, &nrhs, _data, &lda, ipiv, result._data, &ldb, &info);
  delete [] ipiv;
  if(info == 0) return true;
  if(info > 0)
    Msg::Error("U(%d,%d)=0 in LU decomposition", info, info);
  else
    Msg::Error("Wrong %d-th argument in LU decomposition", -info);
  return false;
}

template<>
bool fullMatrix<double>::invert(fullMatrix<double> &result) const
{
  int M = size1(), N = size2(), lda = size1(), info;
  int *ipiv = new int[std::min(M, N)];
  result = *this;
  F77NAME(dgetrf)(&M, &N, result._data, &lda, ipiv, &info);
  if(info == 0){
    int lwork = M * 4;
    double *work = new double[lwork];
    F77NAME(dgetri)(&M, result._data, &lda, ipiv, work, &lwork, &info);
    delete [] work;
  }
  delete [] ipiv;
  if(info == 0) return true;
  else if(info > 0)
    Msg::Error("U(%d,%d)=0 in matrix inversion", info, info);
  else
    Msg::Error("Wrong %d-th argument in matrix inversion", -info);
  return false;
}

template<> 
bool fullMatrix<double>::invertInPlace()
{
  int N = size1(), nrhs = N, lda = N, ldb = N, info;
  int *ipiv = new int[N];
  double * invA = new double[N*N];

  for (int i = 0; i < N * N; i++) invA[i] = 0.;
  for (int i = 0; i < N; i++) invA[i * N + i] = 1.;

  F77NAME(dgesv)(&N, &nrhs, _data, &lda, ipiv, invA, &ldb, &info);
  memcpy(_data, invA, N * N * sizeof(double));

  delete [] invA;
  delete [] ipiv;

  if(info == 0) return true;
  if(info > 0)
    Msg::Error("U(%d,%d)=0 in matrix in place inversion", info, info);
  else
    Msg::Error("Wrong %d-th argument in matrix inversion", -info);
  return false;
}

template<> 
double fullMatrix<double>::determinant() const
{
  fullMatrix<double> tmp(*this);
  int M = size1(), N = size2(), lda = size1(), info;
  int *ipiv = new int[std::min(M, N)];
  F77NAME(dgetrf)(&M, &N, tmp._data, &lda, ipiv, &info);
  double det = 1.;
  if(info == 0){
    for(int i = 0; i < size1(); i++){
      det *= tmp(i, i);
      if(ipiv[i] != i + 1) det = -det;
    }
  }
  else if(info > 0)
    det = 0.;
  else
    Msg::Error("Wrong %d-th argument in matrix factorization", -info);
  delete [] ipiv;  
  return det;
}

template<> 
bool fullMatrix<double>::svd(fullMatrix<double> &V, fullVector<double> &S)
{
  fullMatrix<double> VT(V.size2(), V.size1());
  int M = size1(), N = size2(), LDA = size1(), LDVT = VT.size1(), info;
  int lwork = std::max(3 * std::min(M, N) + std::max(M, N), 5 * std::min(M, N));
  fullVector<double> WORK(lwork);
  F77NAME(dgesvd)("O", "A", &M, &N, _data, &LDA, S._data, _data, &LDA,
                  VT._data, &LDVT, WORK._data, &lwork, &info);
  V = VT.transpose();
  if(info == 0) return true;
  if(info > 0)
    Msg::Error("SVD did not converge");
  else
    Msg::Error("Wrong %d-th argument in SVD decomposition", -info);
  return false;
}

#endif

#include "Bindings.h"

template<>
void fullMatrix<double>::registerBindings(binding *b)
{
  classBinding *cb = b->addClass<fullMatrix<double> >("fullMatrix");
  cb->setDescription("A full matrix of double-precision floating point numbers. "
                     "The memory is allocated in one continuous block and stored "
                     "in column major order (like in fortran).");
  methodBinding *cm;
  cm = cb->setConstructor<fullMatrix<double>,int,int>();
  cm->setDescription ("A new matrix of size 'nRows' x 'nColumns'");
  cm->setArgNames("nRows","nColumns",NULL);
  cm = cb->addMethod("size1", &fullMatrix<double>::size1);
  cm->setDescription("Returns the number of rows in the matrix");
  cm = cb->addMethod("size2", &fullMatrix<double>::size2);
  cm->setDescription("Returns the number of columns in the matrix");
  cm = cb->addMethod("get", &fullMatrix<double>::get);
  cm->setArgNames("i","j",NULL);
  cm->setDescription("Returns the (i,j) entry of the matrix");
  cm = cb->addMethod("set", &fullMatrix<double>::set);
  cm->setArgNames("i","j","v",NULL);
  cm->setDescription("Sets the (i,j) entry of the matrix to v");
  cm = cb->addMethod("resize", &fullMatrix<double>::resize);
  cm->setArgNames("nRows","nColumns","reset",NULL);
  cm->setDescription("Change the size of the fullMatrix (and re-alloc if needed), "
                     "values are set to zero if reset is true");
  cm = cb->addMethod("gemm", &fullMatrix<double>::gemm);
  cm->setArgNames("A","B","alpha","beta",NULL);
  cm->setDescription("this = beta*this + alpha * (A.B)");
  cm = cb->addMethod("gemm_naive", &fullMatrix<double>::gemm_naive);
  cm->setArgNames("A","B","alpha","beta",NULL);
  cm->setDescription("this = beta*this + alpha * (A.B)");
  cm = cb->addMethod("print", &fullMatrix<double>::print);
  cm->setArgNames("name",NULL);
  cm->setDescription("print the matrix");
  cm = cb->addMethod("invertInPlace", &fullMatrix<double>::invertInPlace);
  cm->setDescription("invert the matrix and return the determinant");
}
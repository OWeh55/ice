#ifndef QRDECOMP_H
#define QRDECOMP_H
#include "Matrix.h"

namespace ice
{
  int QRDecompositionPacked(const Matrix& A, Matrix& a, Vector& c, Vector& d);
  int QRDecomposition(const Matrix& A, Matrix& Q, Matrix& R);
}

#endif

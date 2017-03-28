#include <image_nonvis.h>
#include <ludecomp.h>

// tests zum lösen linearer Gleichungssysteme

int main(int argc, char* argv[])
{
  Matrix A1 = Vector(1, 2, 3, 4, 5) &&
              Vector(2, 1, 0, 1, 1) &&
              Vector(5, 4, 3, 2, 1) &&
              Vector(2, 1, 0, 1, 5) &&
              Vector(1, 1, 0, 1, 1);

  Matrix A2 = Vector(1, 2, 3, 4, 5) &&
              Vector(2, 1, 0, 1, 1) &&
              Vector(5, 4, 3, 2, 1) &&
              Vector(2, 1, 0, 1, 5) &&
              Vector(4, 2, 0, 2, 3);

  Matrix A3 = Vector(1, 0, 0, 0, 0) &&
              Vector(0, 1, 0, 0, 0) &&
              Vector(0, 0, 1, 0, 0) &&
              Vector(0, 0, 0, 1, 0) &&
              Vector(0, 0, 0, 0, 1);

// überbestimmtes Gleichungssystem
  Matrix A4 = Vector(1, 2, 3, 4, 5) &&
              Vector(2, 1, 0, 1, 1) &&
              Vector(5, 4, 3, 2, 1) &&
              Vector(2, 1, 0, 1, 5) &&
              Vector(5, 4, 3, 0, 1) &&
              Vector(2, 1, 7, 1, 5) &&
              Vector(1, 1, 0, 1, 1);

  Matrix A = A3;
  Vector x = Vector(1, 2, 3, 4, 5);

  Vector x1, x2;
  Vector b = A * x; // Inhomogenität passend zur Lösung konstruieren

  x1 = SolveLinEqu(A, b);
  cout << x1 << " " << x1 - x << endl;

  if (A.cols() != A.rows())
    {
      b = A.MulTrans(b);
      A = A.MulTrans(A);
    }

  Matrix LU;
  IVector indx;
  LUDecompositionPacked(A, LU, indx, true);
  x2 = LUSolve(LU, indx, b);
  cout << x2 << " " << x2 - x << endl;

  // Test mit klasse matrix<T>

  matrix<double> Am(A.rows(), A.cols());
  for (int r = 0; r < A.rows(); ++r)
    for (int c = 0; c < A.cols(); ++c)
      Am[r][c] = A[r][c];
  vector<double> bv(b.size());
  for (int i = 0; i < b.size(); i++)
    bv[i] = b[i];

  vector<double> xv = SolveLinearEquation(Am, bv);
  for (int i = 0; i < xv.size(); ++i)
    {
      cout << xv[i] << " " << x[i] - xv[i] << endl;
    }

  return 0;
}

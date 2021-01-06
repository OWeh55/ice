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

  Matrix A = A1;
  Vector x = Vector(1, 2, 3, 4, 5);

  Vector x1, x2;
  Vector b = A * x; // Inhomogenität passend zur Lösung konstruieren

  x1 = SolveLinearEquation(A, b);
  cout << "Matrix: " << A;
  cout << "Lösung:      Abweichung:" << endl;
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

  cout << "Matrix: " << A;
  cout << "Lösung:      Abweichung:" << endl;
  cout << x2 << " " << x2 - x << endl;

  // Test mit klasse matrix<T>

  matrix<double> Am(A.rows(), A.cols());
  for (int r = 0; r < A.rows(); ++r)
    for (int c = 0; c < A.cols(); ++c)
      Am[r][c] = A[r][c];

  vector<double> bv(b.size());
  for (unsigned int i = 0; i < b.size(); i++)
    bv[i] = b[i];

  vector<double> xx(x.size());
  for (unsigned int i = 0; i < x.size(); i++)
    xx[i] = x[i];

  vector<double> xv = solveLinearEquation(Am, bv);
  cout << "matrix<double>: " << Am;
  cout << "Lösung:      Abweichung:" << endl;
  cout << xv << " " << (xv - xx) << endl;

  return 0;
}

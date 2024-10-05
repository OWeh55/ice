// test der Funktion QR-Faktorisierung

#include "image_nonvis.h"
#include "qrdecomp.h"

int main(int argc, char* argv[])
{
  int i, j;

  Matrix A;
  Matrix Q;
  Matrix R;

  Matrix A1 = Vector(1, 2, 3) &&
              Vector(2, 3, 2) &&
              Vector(3, 2, 1);

  Matrix A2 = Vector(1, 2, 3) &&
              Vector(1, 2, 3) &&
              Vector(1, 2, 3);

  Matrix A3 = Vector(1, 2, 3) &&
              Vector(0, 5, 6) &&
              Vector(0, 0, 9);

  Matrix A4 = Vector(1, 2, 3, 4) &&
              Vector(0, 5, 6, 7) &&
              Vector(0, 0, 9, 10) &&
              Vector(0, 0, 0, 11) ;

  Matrix A5(20, 20);

  for (i = 0; i < 20; i++)
    for (j = 0; j < 20; j++)
      A5[i][j] = rand() % 100;

  A = A5;

  QRDecomposition(A, Q, R);

  cout.precision(2);
  cout.setf(ios_base::fixed);

  cout << "Ausgangsmatrix A: " << A << endl;
  cout << "Orthonormalmatrix Q: " << Q << endl;

  cout << "Obere Dreieckmatrix R: " << R << endl;

  cout << "Q*R: " << (Q * R) << endl;
  cout << "Q*Q^T: " << (Q * (!Q)) << endl;
  return OK;
}

#include <image_nonvis.h>
#include <ludecomp.h>

using namespace ice;

// tests zum lösen linearer Gleichungssysteme

#define TIME 5.0

string zeit(double t)
{
  if (t < 1e-3) return NumberString(t * 1e6) + " Mikrosekunden ";

  if (t < 1) return NumberString(t * 1e3) + " Millisekunden ";

  return NumberString(t) + " Sekunden ";
}

int main(int argc, char* argv[])
{
  int i, j;
  int n = 4;

  if (argc > 1)
    n = atol(argv[1]);

  double t, ta;

  Matrix A(n, n);
  Vector x(n);

  Vector b(n);
  Vector x1, x2;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = rand() % 100;

  for (i = 0; i < n; i++)
    x[i] = rand() % 100;

  b = A * x;

  i = 0;
  t = TimeD();

  do
    {
      x1 = SolveLinearEquation(A, b);
      i++;
      ta = TimeD();
    }
  while (ta < t + TIME);

  cout << "Zeit: " << zeit(ta - t) << "Zyklen: " << i << " - " << zeit((ta - t) / i) << "/Zyklus" << endl;
  cout << "Abweichung: " << (x1 - x).Length() << endl;

  Matrix LU;
  IVector indx;

  x2 = Vector(3, 3, 3, 3, 3, 3);
  i = 0;
  t = TimeD();

  do
    {
      LUDecompositionPacked(A, LU, indx, true);
      x2 = LUSolve(LU, indx, b);
      i++;
      ta = TimeD();
    }
  while (ta < t + TIME);

  cout << "Zeit: " << zeit(ta - t) << "Zyklen: " << i << " - " << zeit((ta - t) / i) << "/Zyklus" << endl;
  cout << "Abweichung: " << (x2 - x).Length() << endl;

  return 0;
}

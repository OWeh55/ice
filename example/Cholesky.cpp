#include <iomanip>
#include <image.h>

using namespace std;

int main(int argc, char** argv)
{
  matrix<double> m(5, 5, 1);
  /*
  m[0][0]=3; m[0][1]=2; m[0][2]=1; m[0][3]=0; m[0][4]=2;
  m[1][0]=2; m[1][1]=3; m[1][2]=2; m[1][3]=1; m[1][4]=0;
  m[2][0]=1; m[2][1]=2; m[2][2]=3; m[2][3]=2; m[2][4]=1;
  m[3][0]=0; m[3][1]=1; m[3][2]=2; m[3][3]=3; m[3][4]=2;
  m[4][0]=0; m[4][1]=0; m[4][2]=1; m[4][3]=2; m[4][4]=3;
  */
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
      m[i][j] = drand48();

  cout << setprecision(5) << fixed << m << endl;
  matrix<double> mm = m * !m;
  if (Cholesky(mm))
    {
      cout << "Cholesky (m*!m): " << endl << setw(8) << mm << endl;
      cout << " m * !m - mm * !mm" << endl << setw(8) << (m * !m - mm * !mm) << endl;
    }
  else
    cout << "nix Cholesky" << endl;

  mm = m * !m;

  matrix<double> l;
  matrix<double> u;

  LUDecomposition(mm, l, u);
  cout << "LU" << endl;
  cout << setw(8) << l << endl;
  cout << setw(8) << u << endl;
  cout << "mm - l * u" << endl;
  cout << scientific << setw(12) << (mm - l * u) << endl;

  return OK;
}

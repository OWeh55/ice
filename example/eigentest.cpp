#include <image.h>

using namespace ice;

void usage(const string& pn)
{
  cout << "Usage: " << pn << " <options>" << endl;
  cout << "Options:" << endl;
  cout << " -h             this Help" << endl;
  exit(1);
}

void testMatrix()
{
  Matrix a(4, 4);

  a[0][0] = 1.0;
  a[0][1] = 3.0;
  a[0][2] = 2.0;
  a[0][3] = 9.0;

  a[1][1] = 3.0;
  a[1][2] = 1.0;
  a[1][3] = 4.0;

  a[2][2] = 2.0;
  a[2][3] = 1.0;

  a[3][3] = 8.1;

  for (int i = 1; i < 4; i++)
    for (int j = 0; j < i; j++)
      a[i][j] = a[j][i];

  cout << a << endl;

  Vector eval(4);
  Matrix evek(4, 4);

  Eigenvalue(a, eval, evek);

  cout << eval << endl;
  cout << evek << endl;

  cout << "a * evektor[i] ==  eval[i]*evektor[i]" << endl;

  for (int i = 0; i < 4; i++)
    {
      Vector evektor(evek[0][i], evek[1][i], evek[2][i], evek[3][i]);
      cout << setw(10) << a* evektor << " == " << setw(10) << eval[i] * evektor << endl;
    }
}

void testmatrix(int msize)
{
  matrix<double> a(msize, msize);

  if (msize == 4)
    {
      a[0][0] = 1.0;
      a[0][1] = 0.8;
      a[0][2] = 0.6;
      a[0][3] = 0.4;

      a[1][1] = 1.0;
      a[1][2] = 0.8;
      a[1][3] = 0.6;

      a[2][2] = 1.0;
      a[2][3] = 0.8;

      a[3][3] = 1.0;

      for (int i = 1; i < 4; i++)
        for (int j = 0; j < i; j++)
          a[i][j] = a[j][i];
    }
  else
    for (int i = 1; i < msize; i++)
      for (int j = 0; j <= i; j++)
        a[i][j] = a[j][i] = RandomD();

  cout << setprecision(5) << setw(11) << a << endl;

  vector<double> eval(msize);
  vector<vector<double>> evec;

  Eigenvalue(a, eval, evec);

  cout << eval << endl;
  cout << evec << endl;

  cout << "a * evector[i] ==  eval[i]*evector[i]" << endl;
  cout << setprecision(5);
  for (unsigned int i = 0; i < evec.size(); i++)
    {
      cout << setw(11) << a* evec[i] << " == " << setw(11) << eval[i] * evec[i] << "  diff: " << setw(11) << a* evec[i] - eval[i]*evec[i] << endl;
    }
}

int main(int argc, char* argv[])
{
  cout << "Test zu Eigenwerten" << endl;
  cout << "mit Typ Matrix/Vector" << endl;

  testMatrix();
  cout << "----------------------" << endl;
  cout << "Test zu Eigenwerten" << endl;
  cout << "mit Typ matrix<double>/vector<double>" << endl;

  testmatrix(4);
  //  testmatrix(10);
  return 0;
}

#include <fstream>
#include <image_nonvis.h>

#define dim 4

// Test der Statistikklassen

int main(int argc, char* argv[])
{
  Statistics st(dim);

  for (int i = 0; i < 1000000; i++)
    {
      Vector vv(dim);

      for (int j = 0; j < dim; j++)
        {
          double v = GaussRandom(1.0) + 2000;
          vv[j] = v;
        }

      Put(st, vv);
    }

  Vector v = Mean(st);
  cout << v << endl;
  Matrix m = Covariance(st);
  cout << m;

  {
    ofstream os("test.stat");
    os << st;
  }

  Statistics st1(dim);
  {
    ifstream is("test.stat");
    is >> st1;
  }
  cout << Mean(st1) - v << endl;
  cout << Covariance(st1) - m << endl;
  return OK;
}

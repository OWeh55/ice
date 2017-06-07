#include <image_nonvis.h>

#define DIM 4

int PrintMatrix(MatrixStruct m)
{
  int i, j;
  cout << "<" << endl;;

  for (i = 0; i < DIM; i++)
    {
      cout << "<" ;

      for (j = 0; j < DIM; j++)
        {
          cout << m->data[i][j] << ",";
        }

      cout << ">" << endl;
    }

  return OK;
}

int main(int argc, char* argv[])
{
  int i, j;
  Matrix a =
    Vector(1, 1, 0, 1) &&
    Vector(0, 1, 1, 0) &&
    Vector(0, 0, 1, 0) &&
    Vector(1, -10, 0, 2);

  Matrix u, d, v;

  SingularValueDcmp(a, u, d, v);

  cout << fixed;
  cout << u << endl;
  cout << d << endl;
  cout << v << endl;

  cout << (u * d * (!v)) << endl;

  cout << "---------------------------------------------" << endl;

  cout << ((!u) * u) << endl;
  cout << ((!v) * v) << endl;

  cout << "---------------------------------------------" << endl;

  return 0;
}

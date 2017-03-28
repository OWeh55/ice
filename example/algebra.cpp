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

// alte Matrizen
  MatrixStruct A = NewMatrix(MAT_DOUBLE, DIM, DIM);
  MatrixStruct U = NewMatrix(MAT_DOUBLE, DIM, DIM);
  MatrixStruct D = NewMatrix(MAT_DOUBLE, DIM, DIM);
  MatrixStruct V = NewMatrix(MAT_DOUBLE, DIM, DIM);

  MatrixStruct R1 = NewMatrix(MAT_DOUBLE, DIM, DIM);
  MatrixStruct R2 = NewMatrix(MAT_DOUBLE, DIM, DIM);
  MatrixStruct R3 = NewMatrix(MAT_DOUBLE, DIM, DIM);


  for (i = 0; i < DIM; i++)
    for (j = 0; j < DIM; j++)
      A->data[i][j] = a[i][j];

  SingularValueDcmp(A, &U, &D, &V);

//   PrintMatrix(A);
  PrintMatrix(U);
  PrintMatrix(D);
  PrintMatrix(V);
  cout << "-" << endl;
  TranspMat(V, R1);
  MulMat(D, R1, R2);
  MulMat(U, R2, R3);
  PrintMatrix(R3);

  return 0;
}

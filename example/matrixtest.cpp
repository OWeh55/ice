#include <image.h>

int main(int argc, char* argv[])
{
  matrix<double> mat1(5, 5, 1);
  matrix<double> mat2(5, 5, 1);
  matrix<double> mat3;

  for (int i = 0; i < 5; ++i)
    for (int j = 0; j < 5; ++j)
      mat2[i][j] = i - j;

  // unary operators
  cout << "mat1:" << endl;
  cout /*<< setw(3)*/ << mat1 << endl;
  cout << "-mat1:" << endl;
  cout << setw(3) << -mat1 << endl;
  cout << "mat2:" << endl;
  cout << setw(3) << mat2 << endl;
  cout << "!mat2:" << endl;
  cout << setw(3) << !mat2 << endl;

  // binary operators
  cout << "mat1 + mat2:" << endl;
  cout << setw(3) << (mat1 + mat2) << endl;
  cout << "mat3 = mat1:" << endl;
  mat3 = mat1;
  cout << setw(3) << mat3 << endl;
  cout << "mat3 += mat1:" << endl;
  mat3 += mat1;
  cout << setw(3) << mat3 << endl;

  cout << "mat1 - mat2:" << endl;
  cout << setw(3) << (mat1 - mat2) << endl;
  cout << "mat3 = mat1 - mat2:" << endl;
  mat3 = mat1 - mat2;
  cout << setw(3) << mat3 << endl;
  cout << "mat3 -= mat1:" << endl;
  mat3 -= mat1;
  cout << setw(3) << mat3 << endl;
  cout << "mat1 + mat2 - mat1:" << endl;
  cout << setw(3) << (mat1 + mat2 - mat1) << endl;

  cout << "mat1 * mat2:" << endl;
  cout << setw(3) << mat1* mat2 << endl;

  vector<double> vec1(5);
  for (int  i = 0; i < 5; ++i)
    vec1[i] = 100 * i;

  cout << "vec1" << endl;
  cout << setw(3) << vec1 << endl;

  cout << "vec1 * mat1" << endl;
  cout << setw(3) << setw(3) << vec1* mat1 << endl;

  cout << "mat1 * vec1" << endl;
  cout << setw(3) << mat1* vec1  << endl;

  cout << "mat2 * !mat2:" << endl;
  cout << setw(3) << mat2 * !mat2 << endl;

  cout << "mat2 * !mat2 - !mat2*mat2:" << endl;
  cout << setw(3) << mat2 * (!mat2) - (!mat2) *mat2 << endl;

  cout << "vec1 * mat2" << endl;
  cout << setw(4) << vec1* mat2 << endl;

  cout << "mat2 * vec1" << endl;
  cout << setw(4) <<  mat2* vec1 << endl;

  cout << "mat2 * 100" << endl;
  cout << setw(4) << mat2 * 100 << endl;

  cout << "100 * mat2" << endl;
  cout << setw(4) << 100 * mat2 << endl;

  cout << "mat2(2,1,4,3) (Teilmatrix)" << endl;
  cout << setw(2) << mat2(2, 1, 4, 3) << endl;

  // type conversions
  matrix<int> mi(3, 4, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11});
  matrix<double> md(mi);
  cout << "matrix<int> mi:" << endl;
  cout << fixed << setprecision(2);
  cout << setw(5) << mi << endl;
  cout << "matrix<double> md(mi):" << endl;
  cout << setw(5) << md << endl;
  md *= 1.234;
  cout << "md*=1.234:" << endl;
  cout << setw(5) << md << endl;
  mi = (matrix<int>)md;
  cout << "mi = (matrix<int>)md;" << endl;
  cout << setw(5) << mi << endl;

  do
    {
      for (int r = 0; r < mat1.rows(); r++)
        for (int c = 0; c < mat1.cols(); c++)
          mat1[r][c] = (drand48() - 0.5) * 10;
    }
  while (isPositiveDefinite(mat1));

  mat2 = mat1 * !mat1;
  cout << "mat1 = rand()" << endl;
  cout << setw(5) << mat1 << endl;

  cout << "mat2 = mat1*!mat1" << endl;
  cout << setw(5) << mat2 << endl;

  cout << boolalpha << "mat1 is positiv definit: " << isPositiveDefinite(mat1) << endl;
  cout << "mat2 is positiv definit: " << isPositiveDefinite(mat2) << endl;

  Matrix mat(mat2);
  cout << "Matrix mat(mat2)" << endl;
  cout << setw(5) << mat << endl;
  cout << "Matrix mat is positiv definit: " << isPositiveDefinite(mat) << endl;

  cout << "Determinant(mat1)" << endl;
  cout << setw(5) << Determinant(mat1) << endl;

  cout << "Determinant(mat2)" << endl;
  cout << setw(5) << Determinant(mat2) << endl;
  cout << setw(5) << CholeskyDeterminant(mat2) << endl;

  cout << "Determinant(mat)" << endl;
  cout << setw(5) << Determinant(mat) << endl;
  cout << setw(5) << CholeskyDeterminant(mat) << endl;

  Matrix cimat;
  matrix<double> cimat2;
  cout << "CholeskyInverse(Matrix mat): " << endl;
  cout << setw(5) << (cimat = CholeskyInverse(mat)) << endl;
  cout << "CholeskyInverse(matrix<double> mat2): " << endl;
  cout << setw(5) << (cimat2 = CholeskyInverse(mat2)) << endl;

  Matrix imat;
  matrix<double> imat2;
  cout << "Inverse(Matrix mat): " << endl;
  cout << setw(5) << (imat = Inverse(mat)) << endl;
  cout << "Inverse(matrix<double> mat2): " << endl;
  cout << setw(5) << (imat2 = Inverse(mat2)) << endl;

  cout << "CholeskyInverse - Inverse" << endl;
  cout << setw(5) << (cimat - imat) << endl;
  cout << setw(5) << (cimat2 - imat2) << endl;
  return OK;
}

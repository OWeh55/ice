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

  for (int r=0;r<mat2.rows();r++)
    for (int c=0;c<mat2.cols();c++)
      mat2[r][c]=(drand48()-0.5)*10;

  mat2 = mat2 * !mat2;
  cout << "mat2 = rand()" << endl;
  cout << mat2 << endl;

  Matrix mat(mat2);
  cout << "Matrix mat(mat2)"<<endl;
  cout << mat << endl;

  cout << "Determinant(mat2)"<<endl;
  cout << Determinant(mat2) <<endl;
  cout << CholeskyDeterminant(mat2) <<endl;

  cout << "Determinant(mat)"<<endl;
  cout << Determinant(mat) <<endl;
  cout << CholeskyDeterminant(mat) <<endl;
  return OK;
}

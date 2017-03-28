#include <image_nonvis.h>
#include <ludecomp.h>

int main(int argc, char* argv[])
{
  Matrix t1, t2;
  Matrix m;

  Matrix d1, d2;
  t1 = Vector(1, 0, 0) &&
       Vector(4, 5, 0) &&
       Vector(7, 8, 9) ;
  t2 = Vector(1, 2, 3) &&
       Vector(0, 5, 6) &&
       Vector(0, 0, 9) ;

  m = t1 * t2;

  LUDecomposition(m, d1, d2);

  cout << d1 << endl;
  cout << d2 << endl;

  cout << d1* d2 - t1* t2 << endl;
  //  cout << m.LUDecompositionPacked() << endl;

  return 0;

}

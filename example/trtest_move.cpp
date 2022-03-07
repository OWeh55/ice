#include <image.h>

int main(int argc, char** argv)
{
  double dx = 1;
  double dy = 2;
  double dz = 3;
  double alpha = 1.1;
  double beta = 1.2;
  double gamma = 1.3;

  Trafo tr(3, 3);
  tr.move(dx, dy, dz, alpha, beta, gamma);
  cout << tr.getMatrix() << endl;


  double ca = cos(alpha);
  double sa = sin(alpha);

  double cb = cos(beta);
  double sb = sin(beta);

  double cc = cos(gamma);
  double sc = sin(gamma);


  matrix<double> m(4, 4, 1);

  m[0][0] = ca * cb;
  m[0][1] = -sa * cb;
  m[0][2] = sb;
  m[0][3] = cb * ca * dx - cb * sa * dy + sb * dz;

  m[1][0] = sa * cc + sb * sc * ca;
  m[1][1] = cc * ca - sa * sb * sc;
  m[1][2] = -cb * sc;
  m[1][3] = cc * sa * dx + cc * ca * dy + sc * sb * ca * dx - sc * sb * sa * dy - sc * cb * dz;

  m[2][0] = sa * sc - cc * sb * ca;
  m[2][1] = sc * ca + cc * sa * sb;
  m[2][2] = cb * cc;
  m[2][3] = sc * sa * dx + sc * ca * dy - cc * sb * ca * dx + cc * sb * sa * dy + cc * cb * dz;

  cout << m << endl;

  cout << (m - tr.getMatrix()) << endl;
  return OK;
}

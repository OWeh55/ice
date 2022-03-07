#include <image.h>

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  int rad = 5;
  int size = rad * 2 + 1;
  matrix<double> m(size, size);
  m.set(0);
  double sum = 0;
  for (int r = 0; r < m.rows(); ++r)
    for (int c = 0; c < m.cols(); ++c)
      {
        int rn = r - rad;
        int cn = c - rad;
        double r2 = rn * rn + cn * cn;
#if 1
        if (r2 <= rad * rad)
          {
            //      m[r][c]=1.0;
            m[r][c] = drand48() - 0.5;
          }
#endif
#if 0
        m[r][c] = exp(-r2 / 20);
#endif
        sum += m[r][c];

      }

  for (int r = 0; r < m.rows(); ++r)
    for (int c = 0; c < m.cols(); ++c)
      {
        m[r][c] /= sum;
      }

  Image source = ReadImg(fn);
  Image dest = NewImg(source);
  Image rek = NewImg(source);
  Show(ON, source);
  Show(ON, dest);
  Show(ON, rek);

  LsiFilter f(m);
  f.filter(source, dest, 0);
  cout << fixed << setprecision(3) << setw(7) << f << endl;

  LsiFilter fi = f.getInverse(39);
  cout << setprecision(3) << setw(7) << fi << endl;

  fi.filter(dest, rek, 0);
  GetChar();
  return 0;
}

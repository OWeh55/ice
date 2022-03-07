#include <image.h>

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  int rad = 5;
  int size = rad * 2 + 1;
  matrix<double> m(size, size);

  cout << "creating filter mask" << endl;
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
            m[r][c] = 1.0;
            //m[r][c] = drand48() - 0.5;
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

  Image source;
  source.read(fn);
  Image dest;
  dest.create(source);
  Image rec;
  rec.create(source);

  Show(ON, source, "source");
  Show(ON, dest, "filtered");
  Show(ON, rec, "reconstructed");

  LsiFilter f(m);
  cout << "filter:" << endl;
  cout << fixed << setprecision(3) << setw(7) << f << endl;
  cout << "filtering..." ;
  cout.flush();
  f.filter(source, dest, 0);
  cout << "done." << endl;

  cout << "calculating inverse filter...";
  cout.flush();
  LsiFilter fi = f.getInverse(39);
  cout << "done." << endl;

  cout << "inverse filter:" << endl;

  cout << setprecision(3) << setw(7) << fi << endl;
  GetChar();

  cout << "filtering..." ;
  cout.flush();
  fi.filter(dest, rec, 0);
  cout << "done." << endl;
  GetChar();
  return 0;
}

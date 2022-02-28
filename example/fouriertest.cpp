#include <iomanip>
#include <image.h>
#include <FourierTrafo.h>

// test class FourierTrafo

void print(const vector<double>& v, bool index = true, bool sc = false)
{
  int width = 9;
  int dim = v.size();
  int n = dim;
  if (n > 20)
    n = 20;
  int fm = n / 2;
  if (index)
    {
      for (int i = -fm; i < -fm + n; ++i)
        cout << setw(width) << i << " ";
      cout << endl;
    }
  double sum2 = 0;
  for (int i = -fm; i < -fm + n; ++i)
    {
      int idx = i;
      while (idx < 0) idx += dim;
      sum2 += v[idx] * v[idx];
      if (sc)
        cout << setw(width) << setprecision(2) << scientific << v[idx] << " ";
      else
        cout << setw(width) << setprecision(4) << fixed << v[idx] << " ";
    }
  cout << "(" << sqrt(sum2) << ")";
  cout << endl;
}

const int vsize = 9;

int main(int argc, char** argv)
{
  // two functions
  vector<double> f1(vsize);
  vector<double> f2(vsize);

  for (int i = 0; i < vsize; ++i)
    {
      double fi = M_PI * 2 * i / vsize;
      f1[i] = 0.3 * sin(fi) + 0.5 * cos(2 * fi) + 0.2;
      f2[i] = 0.5 * cos(fi) + 0.3 * sin(3 * fi) - 0.2;
    }

  cout << "two given functions" << endl;
  print(f1);
  print(f2, false);

  // FT of functions:
  vector<double> s1r, s1i;
  vector<double> s2r, s2i;
  FourierTrafo ft(vsize, true);
  // transform f1
  ft.setInput(f1);
  ft.getResult(s1r, s1i);

  // transform f2
  ft.setInput(f2);
  ft.getResult(s2r, s2i);

  cout << "the fourier transformed functions" << endl;

  print(s1r, true);
  print(s1i, false);

  print(s2r, false);
  print(s2i, false);

  // combined transform (f1 + i*f2)
  vector<double> s1cr, s1ci;
  vector<double> s2cr, s2ci;
  ft.setParameter(vsize, true);

  ft.setInput(f1, f2);
  ft.getResultFromReal(s1cr, s1ci);
  ft.getResultFromImag(s2cr, s2ci);

  cout << "fourier transformed functions from combined transform" << endl;
  print(s1cr, true);
  print(s1ci, false);

  print(s2cr, false);
  print(s2ci, false);

  print(s1cr - s1r, true, true);
  print(s1ci - s1i, false, true);

  print(s2cr - s2r, false, true);
  print(s2ci - s2i, false, true);

  vector<double> f1r(vsize);
  vector<double> f2r(vsize);

  ft.setParameter(vsize, false); // inverse transform
  // inverse transform f1
  ft.setInput(s1r, s1i);
  ft.getResult(f1r);
  // inverse transform f2
  ft.setInput(s2r, s2i);
  ft.getResult(f2r);
  cout << "the back transformed functions" << endl;
  print(f1r, true);
  print(f2r, false);
  cout << "differences" << endl;
  print(f1r - f1, false, true);
  print(f2r - f2, false, true);
  cout << "---------------------------------------" << endl;
  cout << "simplified calls for vector<double>" << endl;
  cout << "real source" << endl;

  vector<double> scr, sci;
  print(f1, true);
  FourierTrafo::transform(f1, scr, sci, true);
  cout << "spectrum" << endl;
  print(scr, true);
  print(sci, false);
  FourierTrafo::transform(scr, sci, f1r, f2r, false);
  print(f1r, true);
  print(f2r, false);
  cout << "differences" << endl;
  print(f1r - f1, false, true);

  cout << "complex source" << endl;
  print(f1, true);
  print(f2, false);
  FourierTrafo::transform(f1, f2, scr, sci, true);
  cout << "spectrum" << endl;
  print(scr, true);
  print(sci, false);
  FourierTrafo::transform(scr, sci, f1r, f2r, false);
  print(f1r, true);
  print(f2r, false);
  cout << "differences" << endl;
  print(f1r - f1, false, true);
  print(f2r - f2, false, true);
  return 0;
}

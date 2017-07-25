#include <iomanip>
#include <image.h>
#include <fouriertrafo.h>

// test class FourierTrafo

void print(const vector<double>& v, bool index = true)
{
  int dim = v.size();
  int n = dim;
  if (n > 20)
    n = 20;
  int fm = n / 2;
  if (index)
    {
      for (int i = -fm; i <= fm; ++i)
        cout << setw(7) << i << " ";
      cout << endl;
    }
  for (int i = -fm; i <= fm; ++i)
    {
      int idx = i;
      while (idx < 0) idx += dim;
      cout << setw(7) << setprecision(4) << fixed << v[idx] << " ";
    }
  cout << endl;
}

const int vsize = 11;

int main(int argc, char** argv)
{
  // two functions
  vector<double> f1(vsize);
  vector<double> f2(vsize);

  for (int i = 0; i < vsize; ++i)
    {
      double fi = M_PI * 2 * i / vsize;
      f1[i] = sin(fi) + 0.5 * cos(2 * fi) + 0.2;
      f2[i] = cos(fi) + 0.3 * sin(3 * fi) + 0.3;
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
  cout << "the fourier transformed fuctions" << endl;
  print(s1r);
  print(s1i, false);

  print(s2r, false);
  print(s2i, false);

  // combined transform (f1 + i*f2)
  ft.setParameter(vsize, true);

  ft.setInput(f1, f2);
  ft.getResultFromReal(s1r, s1i);
  ft.getResultFromImag(s2r, s2i);

  cout << "fourier transformed fuctions from combined transform" << endl;
  print(s1r);
  print(s1i, false);

  print(s2r, false);
  print(s2i, false);

  ft.setParameter(vsize, false); // inverse transform
  // inverse transform f1
  ft.setInput(s1r, s1i);
  ft.getResult(f1);
  // inverse transform f2
  ft.setInput(s2r, s2i);
  ft.getResult(f2);
  cout << "the back transformed fuctions" << endl;
  print(f1);
  print(f2, false);

  return 0;
}

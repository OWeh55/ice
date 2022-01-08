#include <iomanip>
#include <image.h>
#include <FourierTrafo.h>
// test der Klasse FourierTrafo

void print(const vector<double>& v)
{
  int dim = v.size();
  int n = dim;
  if (n > 20)
    n = 20;
  int fm = n / 2;
  for (int i = -fm; i <= fm; ++i)
    cout << setw(7) << i << " ";
  cout << endl;
  for (int i = -fm; i <= fm; ++i)
    {
      int idx = i;
      while (idx < 0) idx += dim;
      cout << setw(7) << setprecision(4) << fixed << v[idx] << " ";
    }
  cout << endl;
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  if (argc > 1)
    fn = argv[1];

  Image input = ReadImg(fn);
  ImageD f_real = NewImgD(input);
  ImageD f_imag = NewImgD(input);
  Show(ON, input, "input");

  double tstart = TimeD();

  FourierTrafo ft(input.xsize, true, true);

  vector<int> src(input.xsize);
  vector<double> d_imag(input.xsize);
  vector<double> d_real(input.xsize);

  for (int y = 0; y < input.ysize; ++y)
    {
      for (int x = 0; x < input.xsize; ++x)
        src[x] = input.getPixel(x, y);
      ft.setInput(src);
      ft.getResult(d_real, d_imag);
      for (int x = 0; x < input.xsize; ++x)
        {
          PutValD(f_real, x, y, d_real[x]);
          PutValD(f_imag, x, y, d_imag[x]);
        }
    }

  ft.setParameter(input.ysize, true);
  d_real.resize(input.ysize);
  d_imag.resize(input.ysize);

  for (int x = 0; x < input.xsize; ++x)
    {
      for (int y = 0; y < input.ysize; ++y)
        {
          d_real[y] = GetValD(f_real, x, y);
          d_imag[y] = GetValD(f_imag, x, y);
        }
      ft.setInput(d_real, d_imag);
      ft.getResult(d_real, d_imag);
      for (int y = 0; y < input.ysize; ++y)
        {
          PutValD(f_real, x, y, d_real[y]);
          PutValD(f_imag, x, y, d_imag[y]);
        }
    }

  Image freal = NewImg(input);
  Image fimag = NewImg(input);
  Show(ON, freal, "Realteil");
  Show(ON, fimag, "Imaginärteil");

  double f0 = GetValD(f_real, 0, 0);
  PutValD(f_real, 0, 0, 0);

  ConvImgDImg(f_real, freal, ADAPTIVE, SIGNED);
  ConvImgDImg(f_imag, fimag, ADAPTIVE, SIGNED);

  PutValD(f_real, 0, 0, f0);

  Image output = NewImg(input);
  Show(ON, output, "output");
  ft.setParameter(input.ysize, false);
  for (int x = 0; x < input.xsize; ++x)
    {
      for (int y = 0; y < input.ysize; ++y)
        {
          d_real[y] = GetValD(f_real, x, y);
          d_imag[y] = GetValD(f_imag, x, y);
        }
      ft.setInput(d_real, d_imag);
      ft.getResult(d_real, d_imag);
      for (int y = 0; y < input.ysize; ++y)
        {
          PutValD(f_real, x, y, d_real[y]);
          PutValD(f_imag, x, y, d_imag[y]);
        }
    }

  ft.setParameter(input.xsize, false);
  d_real.resize(input.xsize);
  d_imag.resize(input.xsize);

  for (int y = 0; y < input.ysize; ++y)
    {
      for (int x = 0; x < input.xsize; ++x)
        {
          d_real[x] = GetValD(f_real, x, y);
          d_imag[x] = GetValD(f_imag, x, y);
        }
      ft.setInput(d_real, d_imag);
      ft.getResult(d_real, d_imag);
      for (int x = 0; x < input.xsize; ++x)
        {
          output.setPixelLimited(x, y, d_real[x]);
          // PutVal(output, x, y, d_imag[x]);
        }
    }
  cout << TimeD() - tstart << " Sekunden" << endl;
  GetChar();
  return 0;
}

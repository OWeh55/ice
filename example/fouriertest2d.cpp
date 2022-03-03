#include <image.h>
// test FourierTrafo2D

void print(const matrix<double>& m)
{
  for (int i = 0; i < m.rows(); i++)
    {
      for (int k = 0; k < m.cols(); k++)
        cout << m[i][k] << " ";
      cout << endl;
    }
}

int main(int argc, char** argv)
{
  try
    {
      // with matrix
      cout << "matrix<double>:" << endl;

      FourierTrafo2D ft1;
      cout << "ft1 constructed without parameter" << endl;

      matrix<double> m1(4, 8);
      for (int i = 0; i < 4; i++)
        for (int k = 0; k < 8; k++)
          m1[i][k] = i + 0.1 * k;

      ft1.setInput(m1);
      cout << "ft1 set input with matrix<double>" << endl;

      matrix<double> m2(4, 8);
      matrix<double> m3(4, 8);
      ft1.getResult(m2, m3);
      cout << "ft1 got result" << endl;

      FourierTrafo2D ft2(m2, m3, false);
      cout << "ft2 constructed with 2 matrix<double>" << endl;

      matrix<double> m4(4, 8);
      matrix<double> m5(4, 8);
      ft2.getResult(m4, m5);
      cout << "ft2 got result" << endl;

      double fsum = 0.0;

      for (int i = 0; i < 4; i++)
        for (int k = 0; k < 8; k++)
          {
            fsum += fabs(m4[i][k] - m1[i][k]);
            fsum += fabs(m5[i][k]);
          }
      cout << "sum of deviations of " << (m1.rows()*m1.cols()) << " elements: " << fsum << endl;

      // with ImageD
      cout << "ImageD:" << endl;
      int xs = 900;
      int ys = 600;
      ImageD source;
      source.create(xs, ys);
      for (int x = 0; x < xs; x++)
        for (int y = 0; y < ys; y++)
          source.setPixel(x, y, (2 * x - 3 * y) % 250 -125);
      FourierTrafo2D ft3(ys, xs, true);
      ft3.setInput(source);
      ImageD real;
      real.create(xs, ys);
      ImageD imag;
      imag.create(xs, ys);
      ft3.getResult(real, imag);

      real.adaptLimits();
      Show(GRAY,real);
      GetChar();
      
      FourierTrafo2D ft4(ys, xs, false);
      ft4.setInput(real, imag);
      ImageD res;
      res.create(xs, ys);
      ImageD resi;
      resi.create(xs, ys);
      ft4.getResult(res, resi);

      fsum = 0.0;
      for (int x = 0; x < xs; x++)
        for (int y = 0; y < ys; y++)
          {
            fsum += fabs(source.getPixel(x, y) - res.getPixel(x, y));
            fsum += fabs(resi.getPixel(x, y));
          }

      cout << "sum of deviations of " << (xs * ys) << " elements: " << fsum << endl;

      // with Image
      cout << "Image:" << endl;
      int xsI = 300;
      int ysI = 200;
      Image sourceImg;
      sourceImg.create(xsI, ysI, 255);
      for (int x = 0; x < xsI; x++)
        for (int y = 0; y < ysI; y++)
          sourceImg.setPixel(x, y, (x + 3 * y) % 100);
      FourierTrafo2D ft5(ysI, xsI, true);
      ft5.setInput(sourceImg);
      //    ImageD real;
      real.create(xsI, ysI);
      //    ImageD imag;
      imag.create(xsI, ysI);
      ft5.getResult(real, imag);

      FourierTrafo2D ft6(ysI, xsI, false);
      ft6.setInput(real, imag);
      Image resImg;
      resImg.create(xsI, ysI, 255);
      Image resiImg;
      resiImg.create(xsI, ysI, 255);
      ft6.getResult(resImg, resiImg);

      int sum = 0.0;
      for (int x = 0; x < xsI; x++)
        for (int y = 0; y < ysI; y++)
          {
            sum += abs(sourceImg.getPixel(x, y) - resImg.getPixel(x, y));
            sum += abs(resiImg.getPixel(x, y) - 128);
          }
      cout << "sum of deviations of " << (xsI * ysI) << " elements: " << sum << endl;
      Show(OFF, real);
      return 0;
    }

  catch (const char* msg)
    {
      cerr << "Exception: " << msg << endl;
    }
  catch (const string& msg)
    {
      cerr << "Exception: " << msg << endl;
    }
  catch (const IceException& ex)
    {
      cerr << "IceException: " << ex.what() << endl;
    }
  catch (const exception& ex)
    {
      cerr << "exception: " << ex.what() << endl;
    }

  return 1;
}

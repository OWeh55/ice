// test zu den Klassifikatoren

#include <image.h>

#define classes 3
#define samples 5

const int xsize = 1200;
const int ysize = 800;
const int nClasses = 9;

int main(int argc, char* argv[])
{
  Image p1, m1;

  ClearAlpha();
  m1 = NewImg(xsize, ysize, 255);

  Show(OVERLAY, m1);

  Matrix features(samples * classes, 2);
  IVector classnr(samples * classes);

  //  ClassifierNN cl(nClasses,2);
  ClassifierKNN cl(nClasses, 2, 3);
  //  ClassifierMD cl(nClasses,2);
  //  ClassifierBayes cl(nClasses,2);
  for (int i = 0; i < nClasses * 10; ++i)
    {
      int clnr = i % nClasses;
      vector<double> v(2);
      v[0] = rand() % xsize;
      v[1] = rand() % ysize;
      cl.Train(clnr, v);
    }

  cl.Finish();

  WindowWalker ww(m1);

  for (ww.init(); !ww.ready(); ww.next())
    {
      vector<double> v(2);
      v[0] = ww.x;
      v[1] = ww.y;
      m1.setPixel(ww, cl.Classify(v) + 3);
    }

  GetChar();

  return 0;
}

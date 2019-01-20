#include <image.h>
// simple comparisn of images:
//  - image of absolute differences (normalized to 0..255)
//  - sum of absolute differences (L1 norm)

int main(int argc, char** argv)
{
  if (argc != 3)
    {
      cerr << "usage: " << argv[0] << " <src1> <src2>" << endl;
      exit(-1);
    }

  string fn1 = argv[1];
  string fn2 = argv[2];

  int xs, ys, mv, ch;
  InfImgFile(fn1, xs, ys, mv, ch);
  Image src1;
  src1.create(xs, ys, mv);
  ReadImg(fn1, src1);

  InfImgFile(fn2, xs, ys, mv, ch);
  Image src2;
  src2.create(xs, ys, mv);
  ReadImg(fn2, src2);

  if ((src1->xsize != src2->xsize) || (src1->ysize != src2->ysize))
    {
      cerr << "Images differ in size!" << endl;
      exit(-1);
    }

  Image diff;
  diff.create(xs, ys, mv);

  Show(ON, src1, fn1);
  Show(ON, src2, fn2);
  Show(ON, diff, fn1 + " - " + fn2);

  double l1error = 0.0;
  int maxdiff = 1;

  WindowWalker w(src1);
  for (w.init(); !w.ready(); w.next())
    {
      int g1 = src1.getPixel(w);
      int g2 = src2.getPixel(w);
      int d = abs(g1 - g2);
      if (d > maxdiff)
        maxdiff = d;
      diff.setPixel(w, d);
      l1error += d;
    }

  // normalize differences 0...mv
  for (w.init(); !w.ready(); w.next())
    {
      diff.setPixel(w, mv * diff.getPixel(w) / maxdiff);
    }
  l1error /= src1->xsize * src1->ysize;

  cerr << "Average pixel difference: " << l1error << endl;
  GetChar();

  return 0;
}

#include <image.h>
// simper Test zum vergleich mit nice

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
  Image src1(xs, ys, mv);
  ReadImg(fn1, src1);

  InfImgFile(fn2, xs, ys, mv, ch);
  Image src2(xs, ys, mv);
  ReadImg(fn2, src2);

  if ((src1->xsize != src2->xsize) || (src1->ysize != src2->ysize))
    {
      cerr << "Images differ in size!" << endl;
      exit(-1);
    }

  //  Show(ON,src1);

  for (int i = 0; i < 100; i++)
    {
      double l1error = 0.0;

      for (int y = 0 ; y < src1->ysize; y++)
        for (int x = 0 ; x < src1->xsize; x++)
          {
            l1error += abs(GetVal(src1, x, y) - GetVal(src2, x, y));
          }

      l1error /= src1->xsize * src1->ysize;

      //      GetChar();
      cerr << "Average pixel difference: " << l1error << endl;
    }

  return 0;
}

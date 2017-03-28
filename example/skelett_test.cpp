#include <image.h>

int main(int argc, char** argv)
{
  int xs, ys, mv, ch;
  InfImgFile("test_gray.jpg", xs, ys, mv, ch);
  Image src = NewImg(xs, ys, mv);
  Show(ON, src, "source");
  ReadImg("test_gray.jpg", src);

  Image direction, filtered;

#if 0
  filtered = NewImg(src, false);
  SetImg(filtered, filtered->maxval);

  direction = NewImg(src->xsize, src->ysize, 90);

  CalcDirectionStructImg(src, direction, 13);

  // DirectedSmearImg(src,direction,filtered,31,31,1);
  DirectedDoBImg(src, direction, filtered, 13, 13, 1);

  //DoBImg(filtered,src,7,15);
  CopyImg(filtered, src);
#endif
  Image dst = NewImg(src);

  Show(ON, dst, "destination");

  double ta = TimeD();

  for (int i = 0; i < 2; i++)
    SkelettImg(src, dst, 131);

  double tg = TimeD() - ta;
  Printf("Zeit: %4.2lf s\n", tg);

#if 0

  Image dstx = NewImg(src);
  Show(ON, dstx, "destinationXXX");

  ta = TimeD();

  for (int i = 0; i < 10; i++)
    SkelettImgX(src, dstx);

  tg = TimeD() - ta;
  Printf("Zeitx: %4.2lf s\n", tg);

  Image mark = NewImg(src);
  Show(OVERLAY, src, mark);

  int nError = 0;

  for (IPoint p = src.begin(); p != src.end(); src.next(p))
    {
      bool v1 = GetVal(dst, p) > 0;
      bool v2 = GetVal(dstx, p) > 0;

      if (v1 == v2)
        {
          PutVal(mark, p, 2); //OK
        }
      else
        {
          nError++;
          PutVal(mark, p, 1); //  !!
        }
    }

  Printf("Fehler: %d\n", nError);
#endif

  GetChar();

  return 0;
}

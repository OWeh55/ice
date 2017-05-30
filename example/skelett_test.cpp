#include <image.h>

int main(int argc, char** argv)
{
  int xs, ys, mv, ch;
  InfImgFile("test_gray.jpg", xs, ys, mv, ch);
  Image src = NewImg(xs, ys, mv);
  Show(ON, src, "source");
  ReadImg("test_gray.jpg", src);

#if 0
  Image direction, filtered;

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
    //SkelettImg(src, dst, 131);
    skeletonImg(src, dst, 131);
  
  double tg = TimeD() - ta;
  Printf("Zeit: %4.2lf s\n", tg);

#if 1

  Image dstx = NewImg(src);
  Show(ON, dstx, "destinationXXX");

  ta = TimeD();

  for (int i = 0; i < 2; i++)
    skeletonImg(src, dstx);
  //    SkelettImgX(src, dstx);

  tg = TimeD() - ta;
  Printf("Zeitx: %4.2lf s\n", tg);

  Image mark = NewImg(src);
  Show(OVERLAY, src, mark);

  int nError = 0;
  WindowWalker ww(src);
  for (ww.init(); !ww.ready(); ww.next())
    {
      bool v1 = GetVal(dst, ww) > 0;
      bool v2 = GetVal(dstx, ww) > 0;

      if (v1 == v2)
        {
          PutVal(mark, ww, 2); //OK
        }
      else
        {
          nError++;
          PutVal(mark, ww, 1); //  !!
        }
    }

  Printf("Fehler: %d\n", nError);
#endif

  GetChar();

  return 0;
}

#include <image.h>

#define MAXVALUE 230+20
#define QSIZE 32
#define SSIZE 3
#define SNUMBER 22

int main(int argc, char** argv)
{
  int x, y;
  OpenAlpha("Smear");
  Alpha(ON);
  ClearAlpha();
  Print("Smear\n");
  Image src;
  src.create(512, 512, MAXVALUE);
  Show(ON, src, "Source");
  for (int y = 0; y < src.ysize; y++)
    for (int x = 0; x < src.xsize; x++)
      {
        int h = ((x / QSIZE) & 1) ^ ((y / QSIZE) & 1);

        if (h) PutVal(src, x, y, MAXVALUE - 20);
        else PutVal(src, x, y, 20);
      }
  Image dst;
  dst.create(512, 512, MAXVALUE);
  Image mrk;
  mrk.create(512, 512, 4);
  clearImg(mrk);
  Show(OVERLAY, dst, mrk, "Destination");
  smearImg(src, dst, SSIZE);
  Print(".");
  int i = 1;

  while (i < SNUMBER)
    {
      smearImg(dst, dst, SSIZE);
      Print(".");
      i++;
//    sleep(1);
      for (int y = 0; y < dst.ysize; y++)
        for (int x = 0; x < dst.xsize; x++)
          {
            int v = GetVal(dst, x, y);

            if (v == 20) PutVal(mrk, x, y, 1);
            else if (v == MAXVALUE - 20) PutVal(mrk, x, y, 2);
            else PutVal(mrk, x, y, 0);
          }
    }

  Print("\n");

  while (true)
    {
      IPoint p = SelPoint(1, dst);
      Printf("x: %d y: %d val: %d\n", p.x, p.y, GetVal(dst, p.x, p.y));
    }

  return OK;
}

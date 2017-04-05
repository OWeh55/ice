#include <stdio.h>
#include <stdlib.h>
#include <image.h>

using namespace ice;

#define XSIZE 1280
#define YSIZE 1024

//#define XSIZE 512
//#define YSIZE 512

#define GMAX 255

#define XS 5
#define YS 5

#define TIMES 44

int main(int argc, char* argv[])
{
  Image img = NewImg(XSIZE, YSIZE, GMAX);
  Image img2 = NewImg(XSIZE, YSIZE, GMAX);
  Image img3 = NewImg(XSIZE, YSIZE, GMAX);

  OpenAlpha("ICE Text Server");
  SetAttribute(0, 7, 0, 0);
  ClearAlpha();

  Display(ON);
  Show(ON, img, "Testbild");
  Show(ON, img2, "Min");
  Show(ON, img3, "Max");

  clearImg(img);
  clearImg(img2);
  clearImg(img3);

  PutVal(img, XSIZE / 2, YSIZE / 2, GMAX);
  PutVal(img, XSIZE / 2 + 2, YSIZE / 2, GMAX);
  PutVal(img, 1, YSIZE / 2, GMAX);
  PutVal(img, 0, YSIZE / 4, GMAX);
  PutVal(img, XSIZE - 1, YSIZE / 2, GMAX);
  PutVal(img, XSIZE - 2, YSIZE * 3 / 4, GMAX);

  PutVal(img, XSIZE * 3 / 5, 0, GMAX);
  PutVal(img, XSIZE * 3 / 4, 1, GMAX);
  PutVal(img, XSIZE * 3 / 5, YSIZE - 2, GMAX);
  PutVal(img, XSIZE * 3 / 4, YSIZE - 1, GMAX);

  Circle c(XSIZE / 2, YSIZE / 4, YSIZE / 10);
  draw(c, img, GMAX, GMAX);

  Circle c2(XSIZE / 2, YSIZE * 3 / 4, YSIZE / 10);
  draw(c2, img, GMAX, GMAX);

  MinMaxImg(img, 3, 3, img2, img3);

  Printf("Bitte irgendeine Taste drücken\n");
  GetChar();

  double ta = TimeD();

  for (int i = 0; i < TIMES; i++)
    DilateImg(img, XS, YS, img);

  Printf("Dilatation: %4.3f s\n", TimeD() - ta);
  GetChar();

  ta = TimeD();

  for (int i = 0; i < TIMES; i++)
    ErodeImg(img, XS, YS, img);

  Printf("Erosion: %4.3f s\n", TimeD() - ta);
  GetChar();

  ta = TimeD();

  for (int i = 0; i < TIMES; i++)
    MinMaxImg(img, XS, YS, img2, img3);

  Printf("MinMax: %4.3f s\n", TimeD() - ta);
  GetChar();
  return 0;
}












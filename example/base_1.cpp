/********************************************************************
base_1    Test Bildanforderung, Bildpunktzugriff
    Visualisierung, Bildeingabe, Bildausgabe
********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <image.h>

// #include <visdrv.h>

#define maxcount 255

using namespace ice;

int main(int argc, char* argv[])
{
  Image img, img2;
  int sx = 512, sy = 512, z;
  double fx, fy, a, b, fh;

  ClearAlpha();
  SetAlphaCursor(10, 5);
  Printf("Test Bildanforderung, Bildpunktzugriff");
  SetAlphaCursor(10, 6);
  Printf("Visualisierung, Bildeingabe, Bildausgabe");
  SetAlphaCursor(18, 18);

  // visinfo(&sx,&sy,&z,&z,&z,&z);
  img = NewImg(sx - 2, sy - 2, maxcount);
  Show(ON, img, "Testbild1");
  Display(ON);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      PutVal(img, x, y, Random(maxcount));

  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        a = -2 + 3 * ((double)x / (double)sx);
        b = -1.5 + 3 * ((double)y / (double)sy);
        fx = 0;
        fy = 0;
        z = 0;

        while ((z < maxcount) && (fabs(fx) < 4) && (fabs(fy) < 4))
          {
            fh = fx * fx - fy * fy + a;
            fy = fx * fy * 2 + b;
            fx = fh;
            z++;
          }

        PutVal(img, x, y, z);
      }
  WriteImg(img, "apfel.jpg"); /*bild retten */

  img2 = ReadImg("apfel.jpg");
  Show(ON, img2, "Wieder eingelesen");

  Printf("NewImg mit Kopieren");
  Image i3 = NewImg(img2(Window(5, 5, 333, 333)), true);
  Show(ON, i3, "Neues Bild");

  GetChar();

  Display(OFF);

  return 0;
}

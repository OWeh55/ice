#include <stdio.h>
#include <math.h>
#include <image.h>

int main(int argc, char* argv[])
{
  Image imgv, imgo;

  imgv = NewImg(256, 256, 255);
  clearImg(imgv);
  imgo = NewImg(256, 256, 255);
  clearImg(imgo);
  Show(OVERLAY, imgv, imgo);

  Circle c(110, 110, 100);
  draw(c, imgo, 3, -1);

  for (int x = 0; x < 256; x++)
    for (int y = 0; y < 256; y++)
      if (c.Inside(x, y))
        PutVal(imgv, x, y, 222);

  GetChar();


  Ellipse e(110, 110, 100, 50, 0.2);
  draw(e, imgo, 1, -1);

  for (int x = 0; x < 256; x++)
    for (int y = 0; y < 256; y++)
      if (e.Inside(x, y))
        PutVal(imgv, x, y, 255);

  GetChar();

  return 0;
}

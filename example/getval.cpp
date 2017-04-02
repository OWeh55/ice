#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define TIMES 1000

int main(int argc, char* argv[])
{
  Image img;
  img.create(1024, 1024, 255);
  BEGIN(1, "GetVal(img,x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        GetVal(img, x, y);

  END(1, "");

  BEGIN(1, "GetValUnchecked(img,x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        GetValUnchecked(img, x, y);

  END(1, "");

  BEGIN(1, "GetVal(img,x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        GetVal(img, x, y);

  END(1, "");

  BEGIN(1, "GetValUnchecked(img,x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        GetValUnchecked(img, x, y);

  END(1, "");

  BEGIN(1, "img->GetVal(x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        img.getPixel(x, y);

  END(1, "");

  BEGIN(1, "img->GetValUnchecked(x,y)");

  for (int i = 0; i < TIMES; i++)
    for (int y = 0; y < img.ysize; y++)
      for (int x = 0; x < img.xsize; x++)
        img.getPixelUnchecked(x, y);

  END(1, "");

  volatile unsigned char cf[1024 * 1024];

  BEGIN(0, "C array");

  for (int i = 0; i < TIMES; i++)
    {
      for (int k = 0; k < 1024; k++)
        for (int j = 0; j < 1024; j++)
          cf[j + k * 1024];
    }

  END(0, "");

  GetChar();
  return 0;
}

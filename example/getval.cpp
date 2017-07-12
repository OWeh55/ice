#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define TIMES 1000
#define SIZEX 1024
#define SIZEY 2048

int main(int argc, char* argv[])
{
  Image img;
  img.create(SIZEX, SIZEY, 255);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      img.setPixel(x, y, (x + y) % 256);
  ImageD imgd;
  imgd.create(SIZEX, SIZEY, 0, 10);
  for (int y = 0; y < imgd.ysize; y++)
    for (int x = 0; x < imgd.xsize; x++)
      imgd.setPixel(x, y, (x + y) / 256.0);
  unsigned char cf[SIZEX * SIZEY];
  matrix<int> mat(SIZEY, SIZEX);
  matrix<double> matd(SIZEY, SIZEX);
  matrix<unsigned char> matc(SIZEY, SIZEX);

  Print("Timing of pixel access methods\n");

  volatile int vol;
  volatile double dvol;

  for (int c = 0; c < 2; c++)
    {
      BEGIN(1, "Function GetVal(img,x,y)");
      for (int i = 0; i < TIMES; i++)
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            vol = GetVal(img, x, y);
      END(1, "");

      BEGIN(1, "Function GetValUnchecked(img,x,y)");
      for (int i = 0; i < TIMES; i++)
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            vol = GetValUnchecked(img, x, y);
      END(1, "");

      BEGIN(1, "Method Image::getPixel(x,y)");
      for (int i = 0; i < TIMES; i++)
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            vol = img.getPixel(x, y);
      END(1, "");

      BEGIN(1, "Method Image::getPixelUnchecked(x,y)");
      for (int i = 0; i < TIMES; i++)
        for (int y = 0; y < img.ysize; y++)
          for (int x = 0; x < img.xsize; x++)
            vol = img.getPixelUnchecked(x, y);
      END(1, "");

      BEGIN(1, "Method ImageD::getPixel(x,y)");
      for (int i = 0; i < TIMES; i++)
        for (int y = 0; y < imgd.ysize; y++)
          for (int x = 0; x < imgd.xsize; x++)
            dvol = imgd.getPixel(x, y);

      END(1, "");

      BEGIN(1, "operator[] in c array");
      for (int i = 0; i < TIMES; i++)
        {
          for (int k = 0; k < SIZEY; k++)
            for (int j = 0; j < SIZEX; j++)
              vol = cf[j + k * SIZEX];
        }
      END(1, "");

      BEGIN(1, "operator[] in matrix<int>");
      for (int i = 0; i < TIMES; i++)
        {
          for (int k = 0; k < SIZEY; k++)
            for (int j = 0; j < SIZEX; j++)
              vol = mat[k][j];
        }
      END(1, "");

      BEGIN(1, "operator[] in matrix<unsigned char>");
      for (int i = 0; i < TIMES; i++)
        {
          for (int k = 0; k < SIZEY; k++)
            for (int j = 0; j < SIZEX; j++)
              vol = matc[k][j];
        }
      END(1, "");

      BEGIN(1, "operator[] in matrix<double>");
      for (int i = 0; i < TIMES; i++)
        {
          for (int k = 0; k < SIZEY; k++)
            for (int j = 0; j < SIZEX; j++)
              dvol = matd[k][j];
        }
      END(1, "");

      BEGIN(1, "operator[] in matrix<double> (column first)");
      for (int i = 0; i < TIMES; i++)
        {
          for (int j = 0; j < SIZEX; j++)
            for (int k = 0; k < SIZEY; k++)
              dvol = matd[k][j];
        }
      END(1, "");
    }
  Print("Done.\n");
  GetChar();
  return 0;
}

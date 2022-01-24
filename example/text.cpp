#include <image.h>

int main(int argc, char** argv)
{
  Image img;
  img.create(1100, 900, 255);
  Show(ON, img);

  for (int s = 0; s < 5; s++)
    {
      clearImg(img);

      for (int i = 0; i < 16; i++)
        for (int k = 0; k < 16; k++)
          Text(string() + (char)(i + k * 16), 5 + i * 13 * 4, 5 + k * 13 * 4, 200 + ((i ^ k) & 1) * 50, s, img);

      Delay(1.00);
      GetChar();
    }

  GetChar();
  Image small = NewImg(110, 90, 255);
  Show(ON, small);
  Text("Hallo, Welt", 0, 0, 255, 0, small);
  clearImg(img);

  for (int y = 0; y < img->ysize; y++)
    for (int x = 0; x < img->xsize; x++)
      {
        double xs = x * 0.1;
        double ys = y * 0.1;
        int v = GetInterpolVal(small, xs, ys);
        PutVal(img, x, y, v);
      }

  GetChar();
  return 0;
}


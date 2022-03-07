#include <image.h>

int main(int argc, char** argv)
{
  ImageD img;
  img.create(1000, 700, 0, 1);
  Show(GRAY, img);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      img.setPixel(x, y, sin(x / 100.)*sin(y / 100.));
  cout << img.minValue() << " .. " << img.maxValue() << endl;
  GetChar();
  img.adaptLimits();
  cout << img.minValue() << " .. " << img.maxValue() << endl;
  GetChar();
  Show(OFF, img);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      img.setPixel(x, y, sin(x / 100.)*sin(y / 100.));
  Show(GRAY, img);
  cout << img.minValue() << " .. " << img.maxValue() << endl;
  GetChar();
}

#include <image.h>

void flip(const Image& img)
{
  WindowWalker w(img);
  for (w.init(); !w.ready(); w.next())
    {
      IPoint p2(img.xsize - 1 - w.x, w.y);
      if (w.x < p2.x)
        {
          int g1 = img.getPixel(w);
          int g2 = img.getPixel(p2);
          img.setPixel(w, g2);
          img.setPixel(p2, g1);
        }
    }
}

void flop(const Image& img)
{
  WindowWalker w(img);
  for (w.init(); !w.ready(); w.next())
    {
      IPoint p2(w.x, img.ysize - 1 - w.y);
      if (w.y < p2.y)
        {
          int g1 = img.getPixel(w);
          int g2 = img.getPixel(p2);
          img.setPixel(w, g2);
          img.setPixel(p2, g1);
        }
    }
}

int main(int argc, char** argv)
{
  Image img = ReadImg("test_gray.jpg");
  WriteImg(img, "lo.jpg");
  ::flip(img);
  WriteImg(img, "ro.jpg");
  flop(img);
  WriteImg(img, "ru.jpg");
  ::flip(img);
  WriteImg(img, "lu.jpg");
  return 0;
}

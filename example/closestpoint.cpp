#include <image.h>
const int xsize = 1400;
const int ysize = 900;

int main(int arc, char* argv[])
{
  Image img;
  img.create(xsize, ysize, 255);
  Show(ON, img);
  Point p1, p2;
  p1.x = 130;
  p1.y = 228;
  p2.x = 400;
  p2.y = 400;
  while (true)
    {
      LineSeg ls(p1, p2);
      draw(ls, img, 188);

      for (unsigned int x = 5; x < xsize; x += 40)
        for (unsigned int y = 5; y < ysize; y += 40)
          {
            Point s(x, y);
            Point z = ls.ClosestPoint(s);
            Line(s, z, 222, img);
          }
      p2 = p1;
      p1 = SelPoint(img);
      img.set(0);
    }
  GetChar();
  return OK;
}

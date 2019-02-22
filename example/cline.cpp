/*************************************************
 test LineContur
 *************************************************/

#include <stdlib.h>

#include <image.h>

using namespace ice;

int main(int argc, char* argv[])
{
  Image img;
  img.create(100, 100, 63);

  IPoint p1{10, 10};
  IPoint p2{70, 90};

  img.setPixel(p1, 63);
  img.setPixel(p2, 63);

  Contur c = LineContur(p1, p2);

  MarkContur(c, 32, img);
  Show(ON, img);

  while (GetKey() != 0);

  GetChar();

  return 0;
}


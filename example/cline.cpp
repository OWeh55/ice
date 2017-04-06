/*************************************************
 test LineContur
 *************************************************/

#include <stdlib.h>

#include <image.h>

using namespace ice;

int main(int argc, char* argv[])
{
  Contur c;
  Image img;
  int p1[2] = {10, 10}, p2[2] = {70, 90};
  int stat, x1[2];
  img = NewImg(100, 100, 63);
  PutVal(img, 10, 10, 63);
  PutVal(img, 70, 90, 63);
  c = LineContur(p1, p2);
  p2[0] = 30;
  p2[1] = 70;
  c.add(30, 70);
  MarkContur(c, 63, img);
  Show(ON, img);
  Display(ON);
  x1[0] = 50;
  x1[1] = 50;
  stat = SelPoint(1, img, x1);
  c = SelContur(img);
  MarkContur(c, 0, img);

  while (GetKey() != 0);

  GetChar();
  Display(OFF);
  FreeImg(img);
  return 0;
}


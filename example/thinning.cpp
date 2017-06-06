#include <image.h>

int main(int argc, char* argv[])
{
  OpenAlpha("Terminal");
  ClearAlpha();

  Print("Test von Thinning\n");
  Image i1 = ReadImg("test_gray.jpg");
  Image i2;
  i2.create(i1.xsize, i1.ysize, 255);

  Show(ON, i1, "Ausgangsbild");
  Show(ON, i2, "Ergebnisbild");

  GradImg(i1, i1, 1);

  binImg(i1, 15, i1);
  Thinning(i1, i2);

  GetChar();
  return 0;
}

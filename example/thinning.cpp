#include <image.h>

int main(int argc, char* argv[])
{
  Image i1, i2;
  OpenAlpha("Terminal");
  ClearAlpha();
  Print("Test von Thinning\n");
  Display(ON);
  i1 = ReadImg("pic.jpg");
  i2 = NewImg(i1->xsize / 2, i1->ysize / 2, 255);

  ReadImg("pic.jpg", i2);
  GradImg(i2, 1, i2);
  CopyImg(i2, i2);
  Show(ON, i1);
  Show(ON, i2);
  binImg(i1, 15, i1);
  Thinning(i1, i2);
  Display(ON);
  Show(ON, i1);
  Show(ON, i2);
  GetChar();
  return 0;
}

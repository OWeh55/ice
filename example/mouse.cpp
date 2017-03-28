#include <image.h>

int Main(int argc, char* argv[])
{
  Image img;
  img = NewImg(1023, 1023, 255);
  int z, x, y;
  Show(ON, img);
  int i = 0;

  while (true)
    {
      z = Mouse(img, &x, &y);
      Printf("%d ", i++);

      if (z & M_LEFT_DOWN) Printf("L");
      else Printf("l");

      if (z & M_MIDDLE_DOWN) Printf("M");
      else Printf("m");

      if (z & M_RIGHT_DOWN) Printf("R ");
      else Printf("r ");

      if (z & M_LEFT_PRESSED) Printf("L");
      else Printf(".");

      if (z & M_MIDDLE_PRESSED) Printf("M");
      else Printf(".");

      if (z & M_RIGHT_PRESSED) Printf("R ");
      else Printf(". ");

      if (z & M_LEFT_RELEASED) Printf("L");
      else Printf(".");

      if (z & M_MIDDLE_RELEASED) Printf("M");
      else Printf(".");

      if (z & M_RIGHT_RELEASED) Printf("R ");
      else Printf(". ");

      Printf(" %d: (%d,%d)\n", z, x, y);
      usleep(100000);

      if (Inside(img, x, y)) PutVal(img, x, y, 255);
    }

  return OK;
}

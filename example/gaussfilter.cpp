#include <image.h>

int main(int argc, char** argv)
{
#if 0
  Image src = ReadImg("test_gray.jpg");
#else
  Image src = NewImg(800, 800, 255);
  ClearImg(src);

  for (int i = 0; i < 200; i++)
    {
      int x = Random(600);
      int y = Random(600);
      Circle c(x + 100, y + 100, 8);
      draw(c, src, 255, 255);
    }

  WriteImg(src, "Kreise.jpg");
#endif
  Image dest = NewImg(src);
  GaussImg(src, dest, 31, 5);
  Show(ON, src, "Source");
  Show(ON, dest, "Gefiltert");
  GetChar();
  Image maske = NewImg(src);
  InvConvolutionImg(src, dest, maske, 0.0, 0.1);
  Show(ON, maske);
  GetChar();
  return OK;
}

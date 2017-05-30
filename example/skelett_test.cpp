#include <image.h>

int main(int argc, char** argv)
{
  int xs, ys, mv, ch;
  InfImgFile("test_gray.jpg", xs, ys, mv, ch);

  Image src;
  src.create(xs, ys, mv);

  Show(ON, src, "source");

  ReadImg("test_gray.jpg", src);

  Image dst;
  dst.create(src);

  Show(ON, dst, "destination");
  GradImg(src, src);

  double ta = TimeD();

  for (int i = 0; i < 10; i++)
    {
      skeletonImg(src, dst, 2);
    }
  double tg = TimeD() - ta;
  Printf("Zeit: %4.2lf s\n", tg/10);

  GetChar();

  return 0;
}

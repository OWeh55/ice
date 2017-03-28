#include <image.h>

int main(int argc, char** argv)
{
  Image img = ReadImg("test_gray.jpg");
  Image dst = NewImg(img);
  Show(ON, img);
  Show(ON, dst);
  ImageFunction fnimg(img);
  int xc = img.xsize / 2;
  int yc = img.ysize / 2;

  Trafo tr;
  tr.Rotate(xc, yc, Arcus(5.0));
  Function2dParameterTransform trimg(fnimg, tr);

  Function2dValueTransform lintrimg(trimg, -1, 255);

  SetImg(dst, lintrimg);
  GetChar();
  return 0;
}

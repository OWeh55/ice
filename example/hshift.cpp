// Test zu Shift mit Hartley
#include <image.h>

int HNeg(int f, int n)
{
  if (f == 0) return 0;

  return n - f;
}

void HShift(ImageD& imgd, double lx, double ly)
{
  int xs = imgd->xsize;
  int ys = imgd->ysize;
  int xs2 = xs / 2;
  int ys2 = ys / 2;
  double kx = lx * M_PI * 2 / xs;
  double ky = ly * M_PI * 2 / ys;

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      {
        int xi = HNeg(x, xs);
        int yi = HNeg(y, ys);

        if ((xi > x) || ((xi == x) && (yi >= y)))
          {
            int fx = x - xs2;
            int fy = y - ys2;

            double v1 = GetValD(imgd, x, y);
            double v2 = GetValD(imgd, xi, yi);
            double fi = kx * fx + ky * fy;
            PutValD(imgd, x, y, v1 * cos(fi) + v2 * sin(fi));
            PutValD(imgd, xi, yi, v2 * cos(-fi) + v1 * sin(-fi));
          }
      }
}

void HDouble(const ImageD& src, ImageD& dst)
{
  int sx = src->xsize;
  int sy = src->ysize;
  dst = NewImgD(sx * 2, sy * 2);

}


int main(int argc, char** argv)
{
  string fn = "lorna.jpg";

  double lx = 5;
  double ly = -2.2;
  int rc;

  while ((rc = getopt(argc, argv, "f:x:y:")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          fn = optarg;
          break;
        case 'x':
          lx = atof(optarg);
          break;
        case 'y':
          ly = atof(optarg);
          break;
        }
    }

  Image img1 = ReadImg(fn);
  int xs = img1->xsize;
  int ys = img1->ysize;

  ImageD imgd1 = NewImgD(xs, ys);
  ConvImgImgD(img1, imgd1, NORMALIZED, SIGNED);
  ImageD hd1 = NewImgD(xs, ys);
  HartleyImgD(imgd1, hd1);

  //  Image dimg=NewImg(xs,ys,255);
  //  Show(ON,dimg);
  //  ConvImgDImg(hd1,dimg,ADAPTIVE);
  //  GetChar();

  HShift(hd1, lx, ly);

  //  ConvImgDImg(hd1,dimg,ADAPTIVE);
  //  GetChar();

  ImageD imgd2 = NewImgD(xs, ys);
  HartleyImgD(hd1, imgd2);
  Image img2 = NewImg(xs, ys, 255);
  ConvImgDImg(imgd2, img2, NORMALIZED, SIGNED);
  Show(ON, img2);
  GetChar();
  return 0;
}

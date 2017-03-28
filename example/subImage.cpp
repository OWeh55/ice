/**
 * This demo shows how to use the possibility
 * of creating subimages of a ColorImage
 * Niels Oertel - 2008
 */

#include <image.h>

#define i_min(x,y) (x)<(y)?(x):(y)
#define i_max(x,y) (x)>(y)?(x):(y)

int main(int argc, char** argv)
{
  ColorImage img;
  img.read("input/bug.jpg");
  Display(ON);
  Show(ON, img);
  printf("This demos presents how to easily work on a certain\npart of an image.\n");

  printf("Click at some point, to convert the area around to\nHSI colorspace\n");
  IPoint p = SelPoint(img);
  int px1 = i_min(i_max(0, (p.x - 100)), img.xsize);
  int py1 = i_min(i_max(0, (p.y - 100)), img.ysize);
  int px2 = i_min(i_max(0, (p.x + 100)), img.xsize - 1);
  int py2 = i_min(i_max(0, (p.y + 100)), img.ysize - 1);

  ColorImage fwindow(img, Window(px1, py1, px2, py2));
  fwindow.convertTo(csHSI);

  printf("Click to convert it back to RGB colorspace\n");
  SelPoint(img);
  fwindow.convertTo(csRGB);

  printf("Click to exit\n");
  SelPoint(img);
  Display(OFF);

  return 0;
}

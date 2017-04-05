/**
 * This demo shows how to renorm a
 * ColorImage to a larger size
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

int main(int argc, char** argv)
{
  ColorImage img;
  img.read("input/alperowitz.jpg");

  ColorImage result;
  result.create(2 * img.xsize, (int)(1.73 * img.ysize), 255, "Result");

  Display(ON);
  Show(ON, img);
  printf("click to renorm the image the image to 2*width and 1.73*height\n");
  SelPoint(img);

  double start = TimeD();
  renormImg(img, result);
  printf("time elapsed: %f\n", TimeD() - start);
  Show(ON, result);

  printf("click to exit...\n");
  SelPoint(result);

  Display(OFF);
  return 0;
}

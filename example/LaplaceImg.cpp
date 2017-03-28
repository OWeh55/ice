/**
 * This demo shows the usage of the laplace
 * filter on ColorImages
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

int main(int argc, char** argv)
{
  ColorImage src;
  src.read("input/church.jpg");
  ColorImage result;
  result.create(src.xsize, src.ysize, src.maxval);
  Display(ON);
  Show(ON, src);
  printf("click to apply the laplace filter in y-direction\n");
  SelPoint(src);

  double start = TimeD();
  LaplaceYImg(src, result, 3);
  printf("time elapsed: %f\n", TimeD() - start);
  Show(ON, result);

  printf("click to apply the laplace filter in x-direction\n");
  SelPoint(result);
  start = TimeD();
  LaplaceXImg(src, result, 1);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("click to apply the xy laplace filter\n");
  SelPoint(result);
  start = TimeD();
  LaplaceImg(src, result, 1);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("click to exit...\n");
  SelPoint(result);

  Display(OFF);
  return 0;
}

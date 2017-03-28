/**
 * This demo shows how to use the different
 * versions of the LSI filter on ColorImages
 * Niels Oertel - 2008
 */


#include <stdio.h>
#include <image.h>
#include <iostream>

int main(int argc, char** argv)
{
  ColorImage src;
  src.read("input/house.jpg");
  ColorImage resultLSI;
  resultLSI.create(src.xsize, src.ysize, src.maxval);

  Show(ON, src);

  printf("This demo shows the usage of the LSIImg filter, by applying\nthe 3x3 smear filter mask to an image. In addition it measures\nthe time and compares the original (optimized) SmearImg filter\nfilter to the different versions of LSIImg.\n");
  SelPoint(src);
  double start;

  //LSIImg Integer
  int maskI[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  start = TimeD();
  LSIImg(src, resultLSI, 3, 3, maskI, 9, 0);
  printf("LSIImg(int): time elapsed: %f\n", TimeD() - start);

  //LSIImg IMatrix
  IMatrix maskIM(3, 3, 0);

  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      maskIM[y][x] = 1;

  start = TimeD();
  LSIImg(src, resultLSI, maskIM, 9, 0);
  printf("LSIImg(IMatrix): time elapsed: %f\n", TimeD() - start);

  //LSIImg Double
  double maskD[] = {1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9};
  start = TimeD();
  LSIImg(src, resultLSI, 3, 3, maskD, 0);
  printf("LSIImg(double): time elapsed: %f\n", TimeD() - start);

  //LSIImg Matrix
  Matrix maskM(3, 3);

  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      maskM[y][x] = 1.0 / 9;

  start = TimeD();
  LSIImg(src, resultLSI, maskM, 0);
  printf("LSIImg(Matrix): time elapsed: %f\n", TimeD() - start);

  start = TimeD();
  SmearImg(src, resultLSI);
  printf("SmearImg: time elapsed: %f\n", TimeD() - start);
  //  src.setTitle((string)"result");
  Show(OFF, src);
  Show(ON, resultLSI);

  printf("click to exit...\n");
  SelPoint(resultLSI);

  Display(OFF);
  return 0;
}

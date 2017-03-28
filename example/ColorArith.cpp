/**
 * This demo shows how to use the basic arithmetic
 * operations (+,-,>,<) on a ColorImage
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>

int main(int argc, char** argv)
{
  ColorImage img1;
  ColorImage img2;
  ColorImage result;
  result.create(1024, 768, 255, "Ergebnis");
  img1.read("input/ocean.jpg");
  img2.read("input/house.jpg");

  Display(ON);
  Show(ON, img1);
  Show(ON, img2);
  printf("Click to add the images\n");
  SelPoint(img2);

  Show(ON, result);

  double start = TimeD();
  AddImg(img1, img2, result, MD_NORMALIZE);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("Click to sub the images\n");
  SelPoint(result);
  start = TimeD();
  SubImg(img1, img2, result, MD_NORMALIZE);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("Click to create the MinImg\n");
  SelPoint(result);
  start = TimeD();
  MinImg(img1, img2, result);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("Click to create the MaxImg\n");
  SelPoint(result);
  start = TimeD();
  MaxImg(img1, img2, result);
  printf("time elapsed: %f\n", TimeD() - start);

  printf("Click to exit\n");
  SelPoint(result);
  Display(OFF);

  return 0;
}

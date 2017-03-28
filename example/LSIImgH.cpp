/**
 * This demo shows the results of applying
 * the standard LSI filter and a special
 * version to a cyclic channel of a ColorImage
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

#define SRCIMG "input/church.jpg"

int main(int argc, char** argv)
{
  ColorImage srcStd, srcSpe;
  srcStd.read(SRCIMG);
  srcSpe.read(SRCIMG);
  Display(ON);
  Show(ON, srcStd, "standard filtering");
  Show(ON, srcSpe, "special filtering");

  printf("This demo shows, how the difference between LSI filtering\nan image in hsi colorspace with the normal LSI filter and\na special version, that keeps mind of the special structure\nof the hue channel\n");
  printf("click to apply the Mean filter to both images\n\n");
  SelPoint(srcStd);
  int maskI[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

  srcStd.convertTo(csHSI);
  double start = TimeD();
  LSIImg(srcStd.C1(), srcStd.C1(), 3, 3, maskI, 9, 0);
  LSIImg(srcStd.C2(), srcStd.C2(), 3, 3, maskI, 9, 0);
  LSIImg(srcStd.C3(), srcStd.C3(), 3, 3, maskI, 9, 0);
  printf("standard filtering: time elapsed: %f\n\n", TimeD() - start);
  srcStd.convertTo(csRGB);

  srcSpe.convertTo(csHSI);
  start = TimeD();
  LSIImgCyc(srcSpe.H(), srcSpe.H(), 3, 3, maskI, 9, 0);
  LSIImg(srcSpe.S(), srcSpe.S(), 3, 3, maskI, 9, 0);
  LSIImg(srcSpe.I(), srcSpe.I(), 3, 3, maskI, 9, 0);
  printf("special filtering: time elapsed: %f\n\n", TimeD() - start);
  srcSpe.convertTo(csRGB);

  printf("As shown, the standard LSI filtering does not reach an\nacceptable result, but the special filter version takes\nmuch more time\n");
  printf("click to exit...\n");
  SelPoint(srcStd);

  Display(OFF);
  return 0;
}

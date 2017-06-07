/**
 * This demo shows the results of applying
 * the standard LSI filter and a special
 * version to a cyclic channel of a ColorImage
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

#define DOUBLE 1
#define SRCIMG "test_rgb.jpg"

int main(int argc, char** argv)
{
  ColorImage src;
  src.read(SRCIMG);

  Show(ON, src, "original image");

  Printf("This demo shows, how the difference between LSI filtering\nan image in hsi colorspace with the normal LSI filter and\na special version, that keeps mind of the special structure\nof the hue channel\n");
  Printf("<ENTER> to apply the Mean filter to both images\n\n");

  GetChar();

#ifndef DOUBLE
  int maskI[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
#else
  double maskD[] = {1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9, 1.0 / 9};
#endif

  Image h, s, i;
  h.create(src.redImage());
  s.create(src.redImage());
  i.create(src.redImage());
  ColorImageToHsi(src, h, s, i);

  Image h1, s1, i1;
  h1.create(src.redImage());
  s1.create(src.redImage());
  i1.create(src.redImage());
  double start = TimeD();
#ifndef DOUBLE
  LSIImg(h, h1, 3, 3, maskI, 9, 0);
  LSIImg(s, s1, 3, 3, maskI, 9, 0);
  LSIImg(i, i1, 3, 3, maskI, 9, 0);
#else
  LSIImg(h, h1, 3, 3, maskD, 0);
  LSIImg(s, s1, 3, 3, maskD, 0);
  LSIImg(i, i1, 3, 3, maskD, 0);
#endif
  printf("standard filtering: time elapsed: %f\n\n", TimeD() - start);
  ColorImage dest1;
  dest1.create(src);
  HsiToColorImage(h1, s1, i1, dest1);
  Show(ON, dest1, "normal filtering");

  Image h2, s2, i2;
  h2.create(src.redImage());
  s2.create(src.redImage());
  i2.create(src.redImage());
  start = TimeD();
#ifndef DOUBLE
  LSIImgCyc(h, h2, 3, 3, maskI, 9, 0);
  LSIImg(s, s2, 3, 3, maskI, 9, 0);
  LSIImg(i, i2, 3, 3, maskI, 9, 0);
#else
  LSIImgCyc(h, h2, 3, 3, maskD, 0);
  LSIImg(s, s2, 3, 3, maskD, 0);
  LSIImg(i, i2, 3, 3, maskD, 0);
#endif
  printf("special filtering: time elapsed: %f\n\n", TimeD() - start);
  ColorImage dest2;
  dest2.create(src);
  HsiToColorImage(h2, s2, i2, dest2);
  Show(ON, dest2, "special filtering");

  GetChar();
  return 0;
}

/**
 * This demo shows how to use the special
 * version of the Median filter for ColorImages
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

int main(int argc, char** argv)
{
  if (argc < 3)
    {
      printf("usage: %s filtersize filename\n", argv[0]);
      return -1;
    }

  ColorImage src;
  src.read(argv[2]);
  ColorImage result = ColorImage::createColorImage(src.xsize, src.ysize, src.maxval);
  Display(ON);
  Show(ON, src);
  printf("click to apply the median filter\n");
  SelPoint(src);

  Show(ON, result);
  double start = TimeD();
  MedianImg(src, result, atoi(argv[1]));
  printf("time elapsed: %f\n", TimeD() - start);

  printf("click to exit...\n");
  SelPoint(result);

  Display(OFF);
  return 0;
}

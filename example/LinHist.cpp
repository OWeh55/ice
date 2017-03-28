/**
 * This demo shows how to linearize
 * the histogram of a ColorImage
 * Niels Oertel - 2008
 */

#include <stdio.h>
#include <image.h>

int main(int argc, char** argv)
{
  ColorImage img;
  img.read("input/bug.jpg");
  Show(ON, img);
  Display(ON);

  printf("Click to linearize the histogram\n");
  SelPoint(img);
  HistogramEqualization(img);

  printf("Click to exit\n");
  SelPoint(img);
  Display(OFF);

  return 0;
}

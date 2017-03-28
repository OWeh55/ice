/**
 * This demo shows how to apply the
 * Mexican filter to a ColorImage
 */

#include <stdio.h>
#include <image.h>
#include <iostream>

int main(int argc, char** argv)
{
  Image src;
  src = ReadImg("Kreise.jpg");
  Image result(src);

  Show(ON, src, "Original");
  Show(ON, result, "Gefiltert");

  MexicanHatImg(src, result, 13.0, 0);
  GetChar();

  return 0;
}

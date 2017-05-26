/*Erosion und Dilatation*/

#include <stdio.h>
#include <stdlib.h>
#include <image.h>

int main(int argc, char* argv[])
{
  Image img;
  int mask[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; /*strukturierenden Element*/
  img = ReadImg(argv[1]);                         /*Bild einlesen*/
  Show(ON, img);                                  /*Bild darstellen*/
  Display(ON);
  printf("Erosion");
  fflush(stdout);
  getchar();
  erodeImg(img, 3, (int*)mask, img);              /*Erosion*/
  printf("Dilatation");
  fflush(stdout);
  getchar();
  dilateImg(img, 3, (int*)mask, img);             /*Dilatation*/
  getchar();
  Display(OFF);
  return 0;
}

/* ic51221 */
/* Messen Zeitverhalten */

#include <stdio.h>
#include <math.h>

#include <image.h>

#define SIZE   512
#define MAXV1  255
#define MAXV2 1023

int main(int argc, char* argv[])
{
  Image img1, imgo1;
  Image img2, imgo2;
  double t1, t2;
  int i, j;
  double time1, time2, time5, time6;

  img1 = NewImg(SIZE, SIZE, MAXV1);
  imgo1 = NewImg(SIZE, SIZE, MAXV1);
  img2 = NewImg(SIZE, SIZE, MAXV2);
  imgo2 = NewImg(SIZE, SIZE, MAXV2);

  /* ENABLE, ON, 255 */
  Display(RESET);
  Display(ON);
  Show(ON, img1);
  GetChar();
  t1 = TimeD();
  for (int y = 0; y < img1.ysize; y++)
    for (int x = 0; x < img1.xsize; x++)
      {
        PutVal(img1, i, j, 255);
      }
  t2 = TimeD();
  time1 = t2 - t1;
  Alpha(ON);
  ClearAlpha();
  SetAlphaCursor(20, 15);
  Printf("1) - ENABLE, ON, 255");
  Delay(1.0);
  Alpha(OFF);

  Display(RESET);
  Display(ON);
  Show(OVERLAY, img1, imgo1);

  /* ENABLE, OVERLAY, 255 */
  GetChar();
  t1 = TimeD();
  wloop(img1, i, j)
  {
    PutVal(img1, i, j, 255);
  }
  t2 = TimeD();
  time2 = t2 - t1;
  Alpha(ON);
  ClearAlpha();
  SetAlphaCursor(20, 15);
  Printf("3) - Enable, OVERLAY, 255");
  Delay(1.0);
  Alpha(OFF);

  /* ENABLE, ON, 1023 */
  Display(RESET);
  Display(ON);
  Show(ON, img2);
  GetChar();
  t1 = TimeD();
  wloop(img2, i, j)
  {
    PutVal(img2, i, j, 255);
  }
  t2 = TimeD();
  time5 = t2 - t1;
  Alpha(ON);
  ClearAlpha();
  SetAlphaCursor(20, 15);
  Printf("5) - Enable, ON, 1023");
  Delay(1.0);
  Alpha(OFF);

  Display(RESET);
  Display(ON);
  Show(OVERLAY, img2, imgo2);

  /* ENABLE, OVERLAY, 1023 */
  GetChar();
  t1 = TimeD();
  wloop(img2, i, j)
  {
    PutVal(img2, i, j, 255);
  }
  t2 = TimeD();
  time6 = t2 - t1;
  Alpha(ON);
  ClearAlpha();
  SetAlphaCursor(20, 15);
  Printf("7) - Enable, OVERLAY, 1023");
  Delay(1.0);
  Alpha(OFF);


  ClearAlpha();
  Alpha(ON);
  SetAttribute(1, 7, -1, 1);
  ClearAlpha();
  SetAlphaCursor(20, 10);
  Printf("TEST - Zeitverhalten Bildpunktzugriff");
  SetAttribute(0, -1, -1, 1);

  SetAlphaCursor(10, 12);
  Printf("ENABLE,ON,       255:	%f 	pro Pixel %f\n", time1, time1 / (SIZE * SIZE));
  SetAlphaCursor(10, 13);
  Printf("ENABLE,OVERLAY,  255:	%f 	pro Pixel %f\n", time2, time2 / (SIZE * SIZE));

  SetAlphaCursor(10, 17);
  Printf("ENABLE,ON,      1023:	%f 	pro Pixel %f\n", time5, time5 / (SIZE * SIZE));
  SetAlphaCursor(10, 18);
  Printf("ENABLE,OVERLAY, 1023:	%f 	pro Pixel %f\n", time6, time6 / (SIZE * SIZE));

  GetChar();

  return 0;
}

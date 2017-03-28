/********************************************************************
base_2    Test Zeitverhalten des Bildpunktzugriffs
    bei eingeschalteter und ausgeschalteter Visualisierung
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <image.h>
#include <dtime.h>

#define sx 512
#define sy 256
#define mc 10
#define maxv1 127
#define maxv2 1023

double htime;

void t_start();
double t_stop();

using namespace ice;

int main(int argc, char* argv[])
{
  Image img1, img2;
  int x, y, ct;
  double  tim1, tim2, tim3, tim4;

  ClearAlpha();
  SetAlphaCursor(10, 5);
  Printf("Test Zeitverhalten des Bildpunktzugriffs");
  SetAlphaCursor(10, 6);
  Printf("bei eingeschalteter und ausgeschalteter Visualisierung");
  SetAlphaCursor(18, 18);
  Printf("Bitte warten !!!\n");
  Delay(5.0);
  img1 = NewImg(sx, sy, maxv1);
  img2 = NewImg(sx, sy, maxv2);

  Display(ON);
  Show(ON, img1);

  t_start();

  for (ct = 0; ct < mc; ct++)
    for (y = 0; y < sy; y++)
      for (x = 0; x < sx; x++)
        PutVal(img1, x, y, (x + y + ct)&maxv1);

  tim1 = t_stop() / mc / sy / sx * 1e6;

  Show(OFF, img1);

  Show(ON, img2);

  t_start();

  for (ct = 0; ct < mc; ct++)
    for (y = 0; y < sy; y++)
      for (x = 0; x < sx; x++)
        PutVal(img2, x, y, (x + y + ct)&maxv2);

  tim2 = t_stop() / mc / sy / sx * 1e6;

  Show(OFF, img2);
  Display(OFF);
  t_start();

  for (ct = 0; ct < mc; ct++)
    for (y = 0; y < sy; y++)
      for (x = 0; x < sx; x++)
        PutVal(img1, x, y, (x + y + ct)&maxv1);

  tim3 = t_stop() / mc / sy / sx * 1e6;

  t_start();

  for (ct = 0; ct < mc; ct++)
    for (y = 0; y < sy; y++)
      for (x = 0; x < sx; x++)
        PutVal(img2, x, y, (x + y + ct)&maxv2);

  tim4 = t_stop() / mc / sy / sx * 1e6;

  FreeImg(img1);
  FreeImg(img2);

  ClearAlpha();
  SetAlphaCursor(10, 5);
  Printf("Test Zeitverhalten des Bildpunktzugriffs");
  SetAlphaCursor(10, 10);
  Printf("Mit Visualisierung, 1 Byte pro Bildpunkt: %f æs\n", tim1);
  SetAlphaCursor(10, 11);
  Printf("Mit Visualisierung, 4 Byte pro Bildpunkt: %f æs\n", tim2);
  SetAlphaCursor(10, 12);
  Printf("Ohne Visualisierung, 1 Byte pro Bildpunkt: %f æs\n", tim3);
  SetAlphaCursor(10, 13);
  Printf("Ohne Visualisierung, 4 Byte pro Bildpunkt: %f æs\n", tim4);
  SetAlphaCursor(20, 20);
  Printf("Bitte warten !!!\n");
  Delay(5.0);

  GetChar();
  return 0;
}

void t_start()
{
  htime = TimeD();
}

double t_stop()
{
  return TimeD() - htime;
}

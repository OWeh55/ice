/********************************************************************
base_3    Test Visualisierung
********************************************************************/

#include <image.h>
// #include <visdrv.h> // nicht empfohlen!!

#include <stdio.h>

#define pzahl 19
#define last pzahl-1
#define FAK 1
#define TIME 4.0

using namespace ice;

Image n_image(int sx, int sy);

/*******************************************************************/
int main(int argc, char* argv[])
{
  Image p[pzahl], m[pzahl], r, g, b;
  int x, y, z;
  int code;

  ClearAlpha();
  SetAlphaCursor(10, 5);
  printf("Test Visualisierung von Bilddaten");
  SetAlphaCursor(18, 18);
  printf("Bitte warten !!!\n");
  Delay(5.0);

  for (z = 0; z < pzahl; z++)
    {
      p[z] = n_image(30 + z * 5, 30 + z * 5);
      Show(ON, p[z]);
    }

  m[last] = NewImg(p[last]);
  CopyImg(p[last], m[last]);
  Show(OFF, p[last]);
  Show(OVERLAY, p[last], m[last]);
  Delay(TIME);
  wloop(m[last], x, y) PutVal(m[last], x, y, (GetVal(m[last], x, y) / FAK / 8) & 3);
  Delay(TIME);
  wloop(p[last], x, y)
    PutVal(p[last], x, y, p[last]->maxval - GetVal(p[last], x, y));
  Delay(TIME);
  FreeImg(p[4]);
  p[4] = NewImg(p[3]);
  InvertImg(p[3], p[4]);
  wloop(p[4], x, y) PutVal(p[4], x, y, (GetVal(p[4], x, y) / 7) & 3);
  Show(OVERLAY, p[4]);
  Delay(TIME);
  r = NewImg(256, 256, 255);
  g = NewImg(256, 256, 255);
  b = NewImg(256, 256, 255);

  Show(_RGB, r, b, g);

  wloop(r, x, y)
    {
      PutVal(b, x, y, Min(255, Max(x, 255 - y)));
      PutVal(g, x, y, Min(255, Max(x, y)));
      PutVal(r, x, y, Min(255, Max(255 - x, y)));
    }
  Delay(TIME);
  Display(OFF);
  Delay(TIME);
  Display(ON); /*update RGB-Bild */
  Delay(TIME);

  Display(OFF);
  return 0;
}

/*******************************************************************/
Image n_image(int sx, int sy)
{
  Image ptr;
  int x, y, c, xm, ym, v;
  xm = sx;
  ym = sy;
  v = ice::max(xm, ym);
  ptr = NewImg(sx, sy, v * FAK);

  for (y = 0; y < sy; y++)
    for (x = 0; x < sx; x++)
      {
        c = x;

        if (y < c) c = y;

        if ((sy - y) < c) c = sy - y;

        if ((sx - x) < c) c = sx - x;

        PutVal(ptr, x, y, c * 2 * FAK);
      }

  return ptr;
}

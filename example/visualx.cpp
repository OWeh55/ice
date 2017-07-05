/**************************************************************/
/* Testprogramm zur Visualisierung von Bildern          */
/**************************************************************/

#include <image.h>
#include <stdio.h>

#define pzahl 5
#define last pzahl-1
#define FAK 1

using namespace ice;

Image MakeTestImage(int sx, int sy)
{
  Image ptr;
  int x, y, c, xm, ym, v;
  xm = sx;
  ym = sy;
  v = ice::max(xm, ym);
  ptr = NewImg(sx, sy, v * FAK, "Testbild " + NumberString(sx) + "*" + NumberString(sy));

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

int Main(int argc, char* argv[])
{
  Image p[pzahl], m[pzahl], r, g, b;
  int x, y, z;

  int  xs, ys;

  xs = InfVis(MAXX);
  ys = InfVis(MAXY);

  OpenAlpha("Visualisierungstest");
  Alpha(ON);
  ClearAlpha();
  Print("Hello, World\n");

  Display(ON);

  for (z = 0; z < pzahl; z++)
    {
      p[z] = MakeTestImage(100 + z * 25, 100 + z * 25);

      if (z & 1)
        Show(ON, p[z], "Testbild " + NumberString(z));
      else
        Show(ON, p[z]);

      Delay(1.0);
    }

  m[last] = NewImg(p[last]);
  CopyImg(p[last], m[last]);
  Show(OFF, p[last]);
  Show(OVERLAY, p[last], m[last], "Bild mit Overlay");
  Print(NumberString(pzahl) + " Bilder, das letzte mit einem Overlay \n");
  GetChar();

  for (int y = 0; y < m[last].ysize; y++)
    for (int x = 0; x < m[last].xsize; x++)
      PutVal(m[last], x, y, (GetVal(m[last], x, y) / FAK / 8) & 3);

  Print("Overlay veraendert \n");
  GetChar();

  Print("Bild 4 neu anlegen\n");
  p[4] = NewImg(p[3]);
  invertImg(p[3], p[4]);
  for (int y = 0; y < p[4].ysize; y++)
    for (int x = 0; x < p[4].xsize; x++)
      {
        PutVal(p[4], x, y, (GetVal(p[4], x, y) / 7) & 3);
      }
  Print("Bild 4 als reines OVERLAY darstellen\n");
  Show(OVERLAY, Image(), p[4], "Reines Overlay");
  GetChar();
  Print("Alle Bilder freigeben\n");

  r = NewImg(512, 512, 255);
  g = NewImg(512, 512, 255);
  b = NewImg(512, 512, 255);
  Show(_RGB, r, g, b, "Farbbild");
  Print("RGB-Bild erzeugen\n");
  for (int y = 0; y < r.ysize; y++)
    for (int x = 0; x < r.xsize; x++)
      {
        PutVal(r, x, y, Min(255, Max(511 - x, y) / 2));
        PutVal(g, x, y, Min(255, Max(x, y) / 2));
        PutVal(b, x, y, Min(255, Max(x, 511 - y) / 2));
      }
  GetChar();
  Print("RGB-Bild und Farbauszuege darstellen\n");
  Show(ON, r, "Rot");
  Show(ON, g, "Grün");
  Show(ON, b, "Blau");
  GetChar();
  Print("Rot-Bild ändern..\n");
  for (int y = 0; y < r.ysize; y++)
    for (int x = 0; x < r.xsize; x++)
      {
        PutVal(r, x, y, Min(255, Max(x, y) / 2));
      }
  GetChar();
  Print("Grün-Bild ändern..\n");
  for (int y = 0; y < g.ysize; y++)
    for (int x = 0; x < g.xsize; x++)
      {
        PutVal(g, x, y, Min(255, Max(x, 511 - y) / 2));
      }
  GetChar();
  Print("Blau-Bild ändern..\n");
  for (int y = 0; y < b.ysize; y++)
    for (int x = 0; x < b.xsize; x++)
      {
        PutVal(b, x, y, Min(255, Max(511 - x, y) / 2));
      }
  GetChar();
  Display(OFF);
  Alpha(OFF);
  return OK;
}

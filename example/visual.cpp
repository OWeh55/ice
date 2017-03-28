/**************************************************************/
/* Testprogramm zur Visualisierung von Bildern          */
/**************************************************************/

#include <image.h>
#include <stdio.h>

#define PZAHL 9
#define LAST (PZAHL-1)
#define FAK 1

using namespace ice;

Image n_image(int sx, int sy)
{
  int v = ice::max(sx, sy);

  Image newimg;
  newimg.create(sx, sy, v * FAK);

  for (int y = 0; y < sy; y++)
    for (int x = 0; x < sx; x++)
      {
        int c = x;

        if (y < c) c = y;

        if ((sy - y) < c) c = sy - y;

        if ((sx - x) < c) c = sx - x;

        newimg.setPixel(x, y, c * 2 * FAK);
      }

  return newimg;
}

int Main(int argc, char* argv[])
{
  vector<Image> p(PZAHL);

  //  visinfo(&xs,&ys,&valm,&palette,&zoom,&maxlut);
  OpenAlpha("Visualisierungstest");
  Alpha(ON);
  ClearAlpha();

  Print("Hello, World\n");

  for (int z = 0; z < (int)p.size(); ++z)
    {
      string bn = to_string(z);
      p[z] = n_image(60 + z * 70, 60 + z * 70);
      Show(GRAY, p[z], bn);
      Delay(0.11);
    }

  Image m = NewImg(p[LAST], true);

  Show(OFF, p[LAST]);
  Show(OVERLAY, p[LAST], m);
  Print(NumberString(PZAHL) + " Bilder, das letzte mit einem Overlay \n");
  GetChar();
  WindowWalker ww(m);
  for (ww.init(); !ww.ready(); ww.next())
    m.setPixel(ww, m.getPixel(ww) / FAK / 8);
  Print("Overlay verändert \n");
  GetChar();
  Print("Bild 4 freigeben\n");
//  Show(OFF,p[4]);
  FreeImg(p[4]);
  Print("Bild 4 neu anlegen\n");
  p[4] = NewImg(p[3]);
  InvertImg(p[3], p[4]);

  WindowWalker wwp(p[4]);
  for (wwp.init(); !wwp.ready(); wwp.next())
    p[4].setPixel(wwp, p[4].getPixel(wwp) / 7);

  Print("Bild 4 als OVERLAY darstellen\n");
  Show(OVERLAY, p[4]);

  GetChar();

  Print("Bild 6 mit Farbtabelle darstellen\n");

  Show(OFF, p[6]);
  Visual v = Show(ON, p[6], "Bild 6 mit Farbtabelle");

  for (int i = 0; i <= p[6].maxval; ++i)
    {
      int ii = i / 16;
      int cv = (i % 16) * 16;
      v->SetGreyColor(i, ii & 1 ? cv : 0, ii & 2 ? cv : 0, ii & 4 ? cv : 0);
    }

  GetChar();
  Print("Alle Bilder freigeben\n");

  for (int z = 0; z < PZAHL; z++)
    {
      FreeImg(p[z]);
      Delay(0.11);
    }

  FreeImg(m);

  ImageD imgd;
  imgd.create(777, 666, 100, 1000);

  WindowWalker wd(imgd);

  for (wd.init(); !wd.ready(); wd.next())
    imgd.setPixel(wd, (wd.x - wd.y));

  Show(GRAY, imgd);

  for (wd.init(); !wd.ready(); wd.next())
    imgd.setPixel(wd, (wd.x + wd.y));
  GetChar();
  Show(OFF, imgd);

  Image r = NewImg(1024, 1024, 255);
  Image g = NewImg(1024, 1024, 255);
  Image b = NewImg(1024, 1024, 255);
  Show(_RGB, r, b, g);
  Print("RGB-Bild erzeugen\n");
  WindowWalker wwc(r);
  for (wwc.init(); !wwc.ready(); wwc.next())
    {
      int xc = wwc.x / 4;
      int yc = wwc.y / 4;
      b.setPixel(wwc, Min(255, Max(xc, 255 - yc)));
      g.setPixel(wwc, Min(255, Max(xc, yc)));
      r.setPixel(wwc, Min(255, Max(255 - xc, yc)));
    }
  GetChar();
  Alpha(OFF);
  return OK;
}
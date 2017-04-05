#include <stdio.h>
#include <stdlib.h>
#include <image.h>

void Show(const Histogram& h, const Image& mark)
{
  int n;
  double xm, xs, sk;
  h.Statistic(n, xm, xs, sk);
  Printf(" n=%d  xm=%9.3f xs=%9.3f sk=%9.3f\n", n, xm, xs, sk);
  clearImg(mark);
  h.Vis(1, mark);
}

/* Testprogramm für Histogrammfunktionen */
int main(int argc, char* argv[])
{
  OpenAlpha("Histogramm");
  ClearAlpha();

  Image grw = NewImg(700, 400, 255);
  setImg(grw, 255);
  Image mark = NewImg(700, 400, 8);
  Show(OVERLAY, grw, mark);

  Printf("Ueberpruefe Histogrammfunktionen\n");
  Printf("Histogramm 1: 0 .. 99\n");
  Printf("Fuelle Histogramm mit Zufallszahlen -10...110 / 10000 Werte\n");

  Histogram h1(100);

  for (int i = 0; i < 10000; i++)
    {
      int r = rand() % 120 - 10;
      h1.addValue(r);
    }

  Show(h1, mark);
  GetChar();

  Printf("Histogramm 2: 0 .. 99\n");
  Printf("Fuelle Histogramm mit Zufallszahlen 10...30 / 10000 Werte\n");

  Histogram h2(100);

  for (int i = 0; i < 10000; i++)
    {
      int r = rand() % 20 + 10;
      if (r > 20)
        h2.addValue(r, 1);
      else
        h2.addValue(r, 2);
    }

  Show(h2, mark);
  GetChar();

  Image picture = ReadImg("test_gray.jpg");
  Show(ON, picture);

  h2.reset(picture.maxval + 1);
  h2.addImage(picture, 1);

  Show(h2, mark);
  GetChar();

  int pglh = RoundInt(CalcThreshold(h2));
  int pgli = CalcThreshold(picture);

  Printf("Pegel vom Histogramm %d \n", pglh);
  Printf("Pegel vom Bild %d \n", pgli);

  BinImg(picture, pglh, picture);
  GetChar();

  return (0);
};

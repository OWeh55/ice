#include <stdio.h>
#include <stdlib.h>
#include <image.h>

const int raster = 5;

/* Testprogramm für Histogrammfunktionen */
int main(int argc, char* argv[])
{
  int i;
  Hist h1, h2, h3;
  double xm, xs, sk;
  int n;
  int x, y;
  Image grw, mrk;

  //Alpha(ON);
  OpenAlpha("Histogramm");
  ClearAlpha();

  Printf("Ueberpruefe Histogrammfunktionen\n");
  Printf("Histogramm 1: -100.. 100 / 10 Klassen\n");
  Printf("Fuelle Histogramm mit Zufallszahlen -110...110 / 10000 Werte\n");

  h1 = Hist(10, 20, -100);

  for (i = 0; i < 10000; i++)
    h1.add((double)rand() * 220 / (double)RAND_MAX - 110);

  printHist(h1);
  h1.getStatistics(n, xm, xs, sk);
  Printf(" n=%d  xm=%9.3f xs=%9.3f sk=%9.3f\n", n, xm, xs, sk);
  GetChar();
  Printf("Histogramm 2: 0...1 / 20 Klassen\n");
  Printf("Fuelle Histogramm mit Zufallszahlen 0...1 / 10000 Werte\n");
  h2 = Hist(20, 0.05, 0.0);

  for (i = 0; i < 10000; i++)
    h2.add((double)rand() / (double)RAND_MAX);

  printHist(h2);
  GetChar();
  Printf("\n\ngrafische Darstellung mit Hist::Vis\n");
  grw = NewImg(700, 400, 255);
  mrk = NewImg(700, 400, 8);
  mrk.set(0);
  Show(OVERLAY, grw, mrk);
  Display(ON);
  h2.draw(mrk, 1);

  GetChar();
  Display(OFF);

  grw = ReadImg("test_gray.jpg");

  for (int y = 0; y < grw.ysize; y++)
    for (int x = 0; x < grw.xsize; x++)
      {
        int h = GetVal(grw, x, y) * 4 / 7;

        if (((x / raster) & 1) ^ ((y / raster) & 1)) h = h + grw->maxval / 3;

        PutVal(grw, x, y, h);
      }

  mrk = NewImg(grw->xsize, grw->ysize, 4);
  mrk.set(0);
  Show(OVERLAY, grw, mrk);
  Zoom(grw, 0, 0, 0);
  Display(ON);

  h3 = HistImg(grw, 1);
  h3.draw(mrk, 1);
  printHist(h3);

  GetChar();

  int pglh = RoundInt(CalcThreshold(h3));
  //  cout << pgl << endl;

  int pgli = CalcThreshold(grw);
  //  cout << pgl << endl;

  Printf("Pegel vom Histogramm %d \n", pglh);
  Printf("Pegel vom Bild %d \n", pgli);

  binImg(grw, pglh, grw);
  GetChar();
  return 0;
};

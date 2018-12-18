#include <image.h>

#include "graynormalize.h"

Polynom1o Image2Image(const Image &img1, const Image &img2)
{
  Polynom1o res;
  res.FitInit();
  for (int y = 0; y < img1->ysize; y++)
    for (int x = 0; x < img1->xsize; x++)
      {
        int v1 = 255 - GetVal(img1, x, y);
        int v2 = 255 - GetVal(img2, x, y);
        res.FitVal(v1, v2);
      }
  return res;
}

void NormalizeImages(Image3d<Image> &img)
{
  int xs, ys, zs;
  img.getSize(xs, ys, zs);
  // Trafo bestimmen
  // Koeffizienten einer linearen Grauwert-Transformation
  vector<double> m(zs);
  vector<double> n(zs);
  // Bild zs/2 ist "referenzbild"
  int zr = zs / 2;

  m[zr] = 1.0;
  n[zr] = 0.0;

  // da Grauwerttransformation immer nur zwischen den nachbarn bestimmt wird,
  // muss die Transformation akkumuliert werden.
  // "akkumulierte" Koeffizienten
  double ms = 1.0;
  double ns = 0.0;

  for (int i = zr - 1; i >= 0; i--)
    {
      Polynom1o res = Image2Image(img[i], img[i + 1]);
      double ma, na;
      res.getCoefficient(na, ma);
      // cout << " g_" << (i+1) <<"= " << ma << " * g_" << i << " + " << na << endl;
      ns += ms * na;
      ms *= ma;
      // cout << "g_" << zr <<"= " << ms << " * g_" << i << " + " << ns << endl;
      m[i] = ms;
      n[i] = ns;
    }

  ms = 1.0;
  ns = 0.0;
  for (int i = zr + 1; i < zs; i++)
    {
      Polynom1o res = Image2Image(img[i], img[i - 1]);
      double ma, na;
      res.getCoefficient(na, ma);
      // cout << " g_" << (i+1) <<"= " << ma << " * g_" << i << " + " << na << endl;
      ns += ms * na;
      ms *= ma;
      // cout << "g_" << zr <<"= " << ms << " * g_" << i << " + " << ns << endl;
      m[i] = ms;
      n[i] = ns;
    }

  for (int i = 0; i < zs; i++)
    {
      for (int y = 0; y < img[i]->ysize; y++)
        for (int x = 0; x < img[i]->xsize; x++)
          {
            double val = m[i] * (255 - GetVal(img[i], x, y)) + n[i];
            int vali = limited(255 - RoundInt(val), img[i]);
            PutVal(img[i], x, y, vali);
          }

    }
}

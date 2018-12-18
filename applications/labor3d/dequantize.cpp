#include <image.h>
#include "dequantize.h"

const double nada = 0.001;

void Dequantize(const Image& qimg, int step, Image& iimg)
{
  int xs = qimg->xsize;
  int ys = qimg->ysize;
  double step2 = step / 2;
  ImageD h = NewImgD(xs, ys);
  ImageD h1 = NewImgD(xs, ys);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      {
        PutValD(h, x, y, GetValUnchecked(qimg, x, y));
        PutValD(h1, x, y, GetValUnchecked(qimg, x, y));
      }

  double change;
  int istep = 0;

  do
    {
      change = 0;
#ifdef OPENMP
      #pragma omp parallel for schedule(dynamic,40)
#endif

      for (int y = 1; y < ys - 1; y++)
        {
          int y1 = y - 1;
          int y2 = y + 1;

          for (int x = 1; x < xs - 1; x++)
            {
              int x1 = x - 1;
              int x2 = x + 1;
              double sval = GetValUnchecked(qimg, x, y);

              double aval = GetValD(h, x, y);
              double val = (aval +
                            GetValD(h, x1, y) +
                            GetValD(h, x2, y) +
                            GetValD(h, x, y1) +
                            GetValD(h, x, y2)) / 5.0;

              //        cout << sval << " " << aval << " " << val << endl;


              // val += ((val-aval)/4);

              // Begrenzung auf original +- step
              double svalp = sval + step2;

              //        cout << "ul:" << svalp << endl;
              if (val > svalp)
                {
                  val = svalp;
                }
              else
                {
                  double svalm = sval - step2;

                  //  cout << "ll: " << svalm << endl;
                  if (val < svalm)
                    {
                      val = svalm;
                    }
                }

              PutValD(h1, x, y, val);
              double delta = fabs(val - aval);

              if (delta > change) change = delta;

            }
        }

      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          PutValD(h, x, y, GetValD(h1, x, y));

      istep++;
//      cout << istep << ": " << change << endl;
      //     GetChar();
    }
  while (change > nada);

  //  GetChar();
  iimg = NewImg(qimg, true);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(iimg, x, y, limited(RoundInt(GetValD(h, x, y)), iimg));

}

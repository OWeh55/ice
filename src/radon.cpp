/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/* radon transformation and inversion
 * Wolfgang Ortmann, 11
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef OPENMP
#include <omp.h>
#endif

#include "defs.h"
#include "IceException.h"
#include "macro.h"

#include "distance.h"
#include "darith.h"
#include "fourier.h"
#include "radon.h"

namespace ice
{
//-----------------------------------------------------
  static const double sres = 0.3;

  int RadonImg(Image& srcimg, Image& radonimg)
  {
    RadonCTrafo tr(srcimg->xsize, srcimg->ysize,
                   radonimg->xsize, radonimg->ysize);

    unsigned int xs = srcimg->xsize;
    unsigned int ys = srcimg->ysize;

    int imgtype = srcimg->ImageType();

    const void* datap = srcimg->getDataPtr();

    // ImageBase *imgp = srcimg.Img();
#ifdef OPENMP
    #pragma omp parallel shared(srcimg) firstprivate(tr)
#endif
    {
#ifdef OPENMP
      #pragma omp for schedule(dynamic,20)
#endif

      for (int y = 0; y < radonimg->ysize; y++)
        {
          tr.setYR(y);

          for (int x = 0; x < radonimg->xsize; x++)
            {
              int sum = 0;
              tr.setXR(x);
              // Parameter für optimierte Schleife entlang scan-linie
              double xtf, ytf; // Startpunkt
              tr.getXY0(xtf, ytf);
              double dxtf, dytf; // Schrittweite
              tr.getDXY(sres, dxtf, dytf);
              int snumber = tr.getStepNumber(sres); // Zahl der Schritte
#if 1

              switch (imgtype)
                {
                case 1:
                {
                  const PixelType1** sourcep = (const PixelType1**)datap;

                  for (int i = 0; i < snumber; i++)
                    {
                      int xt = RoundInt(xtf);
                      int yt = RoundInt(ytf);

                      if ((unsigned int)xt < xs && (unsigned int)yt < ys)
                        {
                          sum += sourcep[yt][xt];
                        }

                      xtf += dxtf;
                      ytf += dytf;
                    }
                }
                break;
                case 2:
                {
                  const PixelType2** sourcep = (const PixelType2**)datap;

                  for (int i = 0; i < snumber; i++)
                    {
                      int xt = RoundInt(xtf);
                      int yt = RoundInt(ytf);

                      if ((unsigned int)xt < xs && (unsigned int)yt < ys)
                        {
                          sum += sourcep[yt][xt];
                        }

                      xtf += dxtf;
                      ytf += dytf;
                    }
                }
                break;
                case 3:
                {
                  const PixelType3** sourcep = (const PixelType3**)datap;

                  for (int i = 0; i < snumber; i++)
                    {
                      int xt = RoundInt(xtf);
                      int yt = RoundInt(ytf);

                      if ((unsigned int)xt < xs && (unsigned int)yt < ys)
                        {
                          sum += sourcep[yt][xt];
                        }

                      xtf += dxtf;
                      ytf += dytf;
                    }
                }
                break;
                default:
                {
                  for (int i = 0; i < snumber; i++)
                    {
                      int xt = RoundInt(xtf);
                      int yt = RoundInt(ytf);

                      if ((unsigned int)xt < xs && (unsigned int)yt < ys)
                        {
                          sum += srcimg.getPixelUnchecked(xt, yt);
                        }

                      xtf += dxtf;
                      ytf += dytf;
                    }
                }
                }

#else

              for (double tp = tr.getMinPar(); tp < tr.getMaxPar(); tp += sres)
                {
                  int xt, yt;

                  if (tr.getXYS(tp, xt, yt))
                    {
                      // sum += GetValUnchecked(srcimg,xt,yt);
                      sum += imgp->getPixelUnchecked(xt, yt);
                    }
                }

#endif

              sum *= sres;

              if (sum > radonimg.maxval)
                {
                  sum = radonimg.maxval;
                }

              PutVal(radonimg, x, y, RoundInt(sum));
            }
        }
    }
    return 0;
  }

  int InvRadonImg(Image& radonimg, Image& resimg, int fmax)
  {
    // Invertierung Radon durch gefilterte Rückprojektion
    RadonCTrafo tr(resimg->xsize, resimg->ysize,
                   radonimg->xsize, radonimg->ysize);

    if (fmax < 0)
      {
        fmax = radonimg->xsize;
      }

    ImageD akku = NewImgD(resimg->xsize, resimg->ysize);
    setImgD(akku, 0.0);

    //#pragma omp parallel for firstprivate(tr) schedule(dynamic,20)
    for (int yr = 0; yr < radonimg->ysize; yr++)
      {
        tr.setYR(yr);
        Vector zeile(radonimg->xsize);

        for (int i = 0; i < radonimg->xsize; i++)
          {
            zeile[i] = GetVal(radonimg, i, yr);
          }

        //#pragma omp critical
        {
          Hartley(zeile);

          // Filterung der Zeile
          for (int i = 0; i < zeile.Size(); i++)
            {
              double f = zeile.size() - i;

              if (i < f)
                {
                  f = i;
                }

              if (f <= fmax)
                {
                  zeile[i] = zeile[i] * f;
                }
              else
                {
                  zeile[i] = 0.0;
                }
            }

          //      cout << endl;
          Hartley(zeile);
        }
        // Akkumulation
        // #pragma omp critical
        {
          for (int y = 0; y < resimg->ysize; y++)
            for (int x = 0; x < resimg->xsize; x++)
              {
                int xr = tr.getXR(x, y);

                if (xr >= 0 && xr < radonimg->xsize)
                  {
                    PutValD(akku, x, y, GetValD(akku, x, y) + zeile[xr]);
                  }
              }
        }
        // ConvImgDImg(akku,debug);
        // GetChar();
      }

    ConvImgDImg(akku, resimg);
    FreeImgD(akku);
    return 0;
  }
}

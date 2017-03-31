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
/*
 * Abgeleitete Darstellungen von
 * Fourier- und Hartley-Spektren (zweidimensional)
 * Wolfgang Ortmann, 97, 98
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "darith.h"

#include "analygeo.h"
#include "fourier.h"

//-----------------------------------------------------------------------
// Konvertierung von Fourier- und Hartley-Transformierten in abgeleitete
// spektrale Dartellungen
//-----------------------------------------------------------------------
namespace ice
{
// power spectra
#define FNAME "PowerSpectrumFImgD"
  int PowerSpectrumFImgD(ImageD re, ImageD im, ImageD nrm, int mode)
  {
    int x, y;
    int xs, ys;
    PixelFloatType r, i, p;

    RETURN_ERROR_IF_FAILED(MatchImgD(re, im, nrm, xs, ys));

    for (x = 0; x < xs; x++)
      for (y = 0; y < ys; y++)
        {
          r = GetValD(re, x, y);
          i = GetValD(im, x, y);
          p = r * r + i * i;

          switch (mode)
            {
            case MD_POWER:
              PutValD(nrm, x, y, p);
              break;
            case MD_MAGNITUDE:
              PutValD(nrm, x, y, sqrt(p));
              break;
            case MD_LOG:

              if (p != 0.0) p = log10(p);
              else p = -1e12;

              PutValD(nrm, x, y, p);
              break;
            }
        }

    return OK;
  }
#undef FNAME
//-----------------------------------------
#define FNAME "PowerSpectrumHImgD"
  int PowerSpectrumHImgD(ImageD im, ImageD nrm, int mode)
  {
    int xs, ys;

    PixelFloatType r, i, p;

    double sqrt21 = 1 / 2.0;

    RETURN_ERROR_IF_FAILED(MatchImgD(im, nrm, xs, ys));

    ImageD h = NewImgD(xs, ys);

    for (int y = 0; y < ys; y++)
      {
        int yn = negf(y, ys);

        for (int x = 0; x < xs; x++)
          {
            int xn = negf(x, xs);
            r = GetValD(im, x, y);
            i = GetValD(im, xn, yn);
            p = sqrt21 * (r * r + i * i);

            switch (mode)
              {
              case MD_POWER:
                ;
                break;
              case MD_MAGNITUDE:
                p = sqrt(p);
                break;
              case MD_LOG:

                if (p != 0.0) p = log10(p);
                else p = -1e12;

                break;
              }

            PutValD(h, x, y, p);
          }
      }

    for (int y = 0; y < h.ysize; ++y)
      for (int x = 0; x < h.xsize; ++x)
        PutValD(nrm, x, y, GetValD(h, x, y));

    FreeImgD(h);
    return OK;
  }
#undef FNAME
//
// Fourier spectra in magnitude/phase-representation
//
#define FNAME "MPSpectrumFImgD"
  int MPSpectrumFImgD(ImageD re, ImageD im, ImageD b, ImageD p)
  {
    int xs, ys;
    double cmp[2], db, dp;

    RETURN_ERROR_IF_FAILED(MatchImgD(re, im, b));
    RETURN_ERROR_IF_FAILED(MatchImgD(b, p, xs, ys));

    for (int y = 0; y < re.ysize; ++y)
      for (int x = 0; x < re.xsize; ++x)
        {
          cmp[0] = GetValD(re, x, y);
          cmp[1] = GetValD(im, x, y);
          ConvCartesPolar(cmp, &db, &dp);
          PutValD(b, x, y, db);
          PutValD(p, x, y, dp);
        }
    return OK;
  }
#undef FNAME

#define FNAME "MPSpectrumHImgD"
  int MPSpectrumHImgD(ImageD im, ImageD b, ImageD p)
  {
    int xs, ys;
    int xn, yn;
    int NEED_TEMP = false;
    ImageD source;
    double v1, v2, cmp[2], db, dp;

    RETURN_ERROR_IF_FAILED(MatchImgD(im, b, p, xs, ys));

    if ((im == p) || (im == b))
      {
        NEED_TEMP = true;
        source = NewImgD(xs, ys);
        for (int y = 0; y < ys; ++y)
          for (int x = 0; x < xs; ++x)
            PutValD(source, x, y, GetValD(im, x, y));
      }
    else source = im;

    for (int y = 0; y < ys; ++y)
      for (int x = 0; x < xs; ++x)
        {
          xn = negf(x, xs);
          yn = negf(y, ys);
          v1 = GetValD(source, x, y);
          v2 = GetValD(source, xn, yn);
          cmp[0] = (v1 + v2) / 2;
          cmp[1] = (-v1 + v2) / 2;
          ConvCartesPolar(cmp, &db, &dp);
          PutValD(b, x, y, db);
          PutValD(p, x, y, dp);
        }

    if (NEED_TEMP) FreeImgD(source);

    return OK;
  }
#undef FNAME
//
#define FNAME "FourierMPImgD"
  int FourierMPImgD(ImageD mag, ImageD phase, ImageD re, ImageD im)
  {
    int xs, ys;
    double m, p;
    double cmp[2];

    RETURN_ERROR_IF_FAILED(MatchImgD(mag, phase, re));
    RETURN_ERROR_IF_FAILED(MatchImgD(re, im, xs, ys));

    for (int y = 0; y < mag.ysize; ++y)
      for (int x = 0; x < mag.xsize; ++x)
        {
          p = GetValD(phase, x, y);
          m = GetValD(mag, x, y);
          ConvPolarCartes(m, p, cmp);
          PutValD(re, x, y, cmp[0]);
          PutValD(im, x, y, cmp[1]);
        }
    return OK;
  }
#undef FNAME
#define FNAME "HartleyMPImgD"
  int HartleyMPImgD(ImageD mag, ImageD phase, ImageD im)
  {
    int xs, ys;
    double m, p;
    double cmp[2];

    RETURN_ERROR_IF_FAILED(MatchImgD(mag, phase, im, xs, ys));

    for (int y = 0; y < mag.ysize; ++y)
      for (int x = 0; x < mag.xsize; ++x)
        {
          p = GetValD(phase, x, y);
          m = GetValD(mag, x, y);
          ConvPolarCartes(m, p, cmp);
          PutValD(im, x, y, cmp[0] - cmp[1]);
        }
    return OK;
  }
#undef FNAME
//-----------------------------------------------------
// direct calculation of spectra from (double)images
//-----------------------------------------------------
#define FNAME "PowerSpectrumImgD"
  int PowerSpectrumImgD(ImageD img, ImageD spec, int mode)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImgD(img, spec, xs, ys));
    HartleyImgD(img, spec);
    PowerSpectrumHImgD(spec, spec, mode);
    return OK;
  }
#undef FNAME
//---------------------------------------------------
#define FNAME "MPSpectrumImgD"
  int MPSpectrumImgD(ImageD img, ImageD mag, ImageD phase)
  {
    ImageD temp = NewImgD(img);
    RETURN_ERROR_IF_FAILED(HartleyImgD(img, temp));
    RETURN_ERROR_IF_FAILED(MPSpectrumHImgD(temp, mag, phase));
    FreeImgD(temp);
    return OK;
  }
#undef FNAME
//--------------------------------------------------------
#define FNAME "CepstrumImgD"
  int CepstrumImgD(ImageD img, ImageD ceps)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImgD(img, ceps, xs, ys));
    PowerSpectrumImgD(img, ceps, MD_LOG);
    PowerSpectrumImgD(ceps, ceps);
    return OK;
  }
#undef FNAME
}

/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-2017 FSU Jena, Digital Image Processing Group
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

#include <vector>
#include "macro.h"
#include "IceException.h"
#include "numbase.h"
#include "base.h"
#include "convolution_fft.h"
#include "trafo_img.h"
#include "geo.h"
#include "simplex.h"
#include "peakEvaluation.h"
#include "matchimg.h"

namespace ice
{
  using namespace std;
#define FNAME "Windowing"
  void Windowing(const ImageD& source, ImageD& dest,
                 int refValue)
  {
    try
      {
        int xs, ys;
        MatchImgD(source, dest, xs, ys);

        if (refValue < 0)
          {
            double sum = 0;
            for (int y = 0; y < ys; y++)
              for (int x = 0; x < xs; x++)
                sum += source.getPixelUnchecked(x, y);
            refValue = sum / xs / ys;
          }

        for (int y = 0; y < ys; y++)
          {
            double fy = 1 - cos(2 * M_PI * y / ys);
            for (int x = 0; x < xs; x++)
              {
                double fx = 1 - cos(2 * M_PI * x / xs);
                double value = source.getPixelUnchecked(x, y) - refValue;
                dest.setPixelUnchecked(x, y, value * fx * fy + refValue);
              }
          }
      }
    RETHROW;
  }

  void Windowing(const Image& source, ImageD& dest,
                 int refValue)
  {
    try
      {
        ImageD dsource;
        dsource.create(source.xsize, source.ysize);
        ConvImgImgD(source, dsource, RAW, UNSIGNED);
        Windowing(dsource, dest, refValue);
      }
    RETHROW;
  }

  void Windowing(const Image& source, Image& dest,
                 int refValue)
  {
    try
      {
        ImageD h;
        h.create(source.xsize, source.ysize);
        Windowing(source, h);
        ConvImgDImg(h, dest, RAW, UNSIGNED);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "DetectShift"
  double detectShift(const Image& img1, const Image& img2,
                     double& dx, double& dy, double beta)
  {
    try
      {
        int sx, sy;
        MatchImg(img1, img2, sx, sy);

        ImageD dimg1;
        dimg1.create(img1.xsize, img1.ysize);
        Windowing(img1, dimg1);

        ImageD dimg2;
        dimg2.create(img2.xsize, img2.ysize);
        Windowing(img2, dimg2);

        ImageD imgr;
        imgr.create(sx, sy);

        InvConvolution(img1, img2, imgr, beta, MD_IGNORE_BIAS);

        Point p;
        double val = peakEvaluation(imgr, p);

        dx = p.x - sx / 2;
        dy = p.y - sy / 2; // Peak coordinates -> Shift (of windows)
        return val;
      }
    RETHROW;
  }
#undef FNAME

  constexpr int MINVAL = 0.5;

#define FNAME "detectTrafo"

  int inside(const Image& img, const Trafo& tr, IPoint p)
  {
    Point dp = p;
    transform(tr, dp);
    return img.inside(p);
  }

  void GetWindow2Img(const Image& img1, Window& w)
  {
    int sx = img1->xsize;
    int sy = img1->ysize;

    // nächst-kleine zweierpotenz
    int sx2, sy2;

    for (sx2 = 1; sx2 * 2 <= sx; sx2 += sx2) /* nop */;

    for (sy2 = 1; sy2 * 2 <= sy; sy2 += sy2) /* nop */;

    int dx = sx - sx2;
    int dy = sy - sy2;

    w.p1.x = dx / 2;
    w.p1.y = dy / 2;
    w.p2.x = w.p1.x + sx2;
    w.p2.y = w.p1.y + sy2;
  }

  bool detectTrafo(const Image& img1, const Image& img2, const Image& himg,
                   Trafo& tr,
                   double beta, int ct,
                   int mode)
  {
    try
      {
        int sx, sy;
        MatchImg(img1, img2, himg, sx, sy);

        Trafo tri; // Inverse Transformation zu tr;
        Trafo trd; // "Korrektur-Transformation" während Iteration

        if ((mode & DT_REFINE) == 0)
          {
            tr.init();                   // new transformation

            Window w;
            GetWindow2Img(img1, w);

            double dx, dy, val;

            // Detection der globalen Verschiebung
            val = detectShift(img1(w), img2(w), dx, dy, beta * 3);

            if (val < MINVAL)
              {
                throw IceException(FNAME, "Can't detect global shift");
              }

            tr.shift(dx, dy);
          }

        int bs = std::min(sx, sy) / 5;

        int blocksize = 32; // kleinste Blockgröße

        while (blocksize * 2 <= bs)
          {
            blocksize = blocksize + blocksize;
          }

        int cycleCount = 0;

        while (cycleCount < ct)
          {
            Transform(tr, img1, himg, INTERPOL);
            tri = tr.inverse();

            int xi = sx / blocksize;
            int yi = sy / blocksize;

            if ((xi > 2) && (yi > 2))
              {
                // Verfeinerung
                vector<Point> pl1; // Punktlisten
                vector<Point> pl2;

                for (int j = 0; j < yi; j++)
                  for (int i = 0; i < xi; i++)
                    {
                      Window w(i * blocksize, j * blocksize,
                               (i + 1)*blocksize - 1, (j + 1)*blocksize - 1);

                      if (
                        inside(img1, tri, w.p1) &&
                        inside(img1, tri, w.p2)
                      )
                        {
                          double dx, dy, val;
                          val = detectShift(himg(w), img2(w), dx, dy, beta);

                          if (val > MINVAL)
                            {
                              double xm = (w.p1.x + w.p2.x) / 2.0;
                              double ym = (w.p1.y + w.p2.y) / 2.0;
                              pl1.push_back(Point(xm, ym));
                              pl2.push_back(Point(xm + dx, ym + dy));
                            }
                        }
                    }

                if (pl1.size() > 4)
                  {
                    if (cycleCount < 3)
                      {
                        trd = MatchPointlists(pl1, pl2);  // erste iterationen nur affin
                      }
                    else
                      {
                        trd = MatchPointlists(pl1, pl2, TRM_PROJECTIVE);  // ..dann projektiv zulassen
                      }

                    tr.append(trd);
                  }
                else
                  return false;
              }

            cycleCount++;
          }
        return true;
      }
    RETHROW;
  }

  bool detectTrafo(const Image& img1, const Image& img2, Trafo& tr,
                   double beta, int ct,
                   int mode)
  {
    Image himg;
    himg.create(img2.xsize, img2.ysize, img2.maxval);
    return detectTrafo(img1, img2, himg, tr, beta, ct, mode);
  }

#undef FNAME

}

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
#include "macro.h"
#include "IceException.h"
#include "numbase.h"
#include "base.h"
#include "convolution_fft.h"
#include "trafo_img.h"
#include "geo.h"
#include "simplex.h"
#include "PeakVal.h"
#include "matchimg.h"

namespace ice
{
#define FNAME "DetectShift"
  int DetectShift(const Image& img1, const Image& img2,
                  double& dx, double& dy, double& val,
                  double beta)
  {
    try
      {
        int sx, sy;
        MatchImg(img1, img2, sx, sy);

        Image imgr;
        imgr.create(sx, sy, 255);

        InvConvolutionImg(img1, img2, imgr, 0.0, beta, MD_IGNORE_BIAS);

        val = PeakValuation(imgr, Image(), dx, dy);

        dx -= sx / 2;
        dy -= sy / 2; // Peak coordinates -> Shift (of windows)
        return OK;
      }
    RETHROW;
  }
#undef FNAME

#define MINVAL (0.5)

#define FNAME "DetectTrafo"

  int inside(const Image& img, const Trafo& tr, IPoint p)
  {
    Point dp = p;
    Transform(tr, dp);
    return img.inside(p);
  }

  /*
  static void TransformWindow(const Trafo &tr,
            int wxi,int wyi,int wxa,int wya,
            int &wxit,int &wyit,int &wxat,int &wyat)
  {
    int xt,yt;
    TransformAndRound(tr,wxi,wyi,wxit,wyit);
    wxat=wxit; wyat=wyit;
    TransformAndRound(tr,wxi,wya,xt,yt);
    wxit=Min(wxit,xt); wxat=Max(wxat,xt);
    wyit=Min(wyit,yt); wyat=Max(wyat,yt);
    TransformAndRound(tr,wxa,wya,xt,yt);
    wxit=Min(wxit,xt); wxat=Max(wxat,xt);
    wyit=Min(wyit,yt); wyat=Max(wyat,yt);
    TransformAndRound(tr,wxa,wyi,xt,yt);
    wxit=Min(wxit,xt); wxat=Max(wxat,xt);
    wyit=Min(wyit,yt); wyat=Max(wyat,yt);
  }
  */
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

  int DetectTrafo(const Image& img1, const Image& img2, Image& himg,
                  Trafo& tr,
                  double beta, int ct,
                  int mode)
  {
    int sx, sy;

    RETURN_ERROR_IF_FAILED(MatchImg(img1, img2, himg, sx, sy));

    Trafo tri; // Inverse Transformation zu tr;
    Trafo trd; // "Korrektur-Transformation" während Iteration

    if ((mode & DT_REFINE) == 0)
      {
        tr = Trafo(); /* new transformation */

        Window w;
        GetWindow2Img(img1, w);

        double dx, dy, val;

        // Detection der globalen Verschiebung
        DetectShift(img1(w), img2(w), dx, dy, val, beta * 3);

        if (val < MINVAL)
          {
            throw IceException(FNAME, "Can't detect global shift");
          }

        tr.Shift(dx, dy);
      }

    int bs = Min(sx, sy) / 5;

    int blocksize = 32; // kleinste Blockgröße

    while (blocksize * 2 <= bs)
      {
        blocksize = blocksize + blocksize;
      }

    int count = 0;

    while (count < ct)
      {
        Transform(tr, img1, himg, INTERPOL);
        tri = tr;
        tri.Invert();
        int xi = sx / blocksize;
        int yi = sy / blocksize;

        if ((xi > 2) && (yi > 2))
          {
            // Verfeinerung
            Matrix pl1(0, 2); // Punktlisten
            Matrix pl2(0, 2);

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
                      DetectShift(himg(w), img2(w), dx, dy, val, beta);

                      if (val > MINVAL)
                        {
                          double xm = (w.p1.x + w.p2.x) / 2.0;
                          double ym = (w.p1.y + w.p2.y) / 2.0;
                          pl1 = pl1 && Vector(xm, ym);
                          pl2 = pl2 && Vector(xm + dx, ym + dy);
                        }
                    }
                }

            if (pl1.rows() > 4)
              {
                if (count < 3)
                  {
                    trd = MatchPointlists(pl1, pl2);  // erste iterationen nur affin
                  }
                else
                  {
                    trd = MatchPointlists(pl1, pl2, TRM_PROJECTIVE);  // ..dann projektiv zulassen
                  }

                tr = tr * trd;
              }
          }

        count++;
      }

    return OK;
  }

  int DetectTrafo(Image img1, Image img2, Trafo& tr,
                  double beta, int ct,
                  int mode)
  {
    Image himg = NewImg(img2);
    return DetectTrafo(img1, img2, himg, tr, beta, ct, mode);
  }

#undef FNAME

}

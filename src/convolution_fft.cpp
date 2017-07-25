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
// Faltung und "Ent-"Faltung auf der Basis der FT
#include <math.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "base.h"
#include "based.h"
#include "fouriertrafo.h"
#include "fourier.h"
#include "convolution_fft.h"

namespace ice
{
#define FNAME "Convolution"
  void Convolution(const Vector& s1, const Vector& s2, Vector& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);
        std::vector<double> h1(size);
        std::vector<double> h2(size);
        for (int i = 0; i < size; i++)
          h1[i] = s1[i];
        for (int i = 0; i < size; i++)
          h2[i] = s2[i];
        std::vector<double> dh;
        Convolution(s1, s2, dh);
        d.Resize(size);
        for (int i = 0; i < size; i++)
          d[i] = dh[i];
      }
    RETHROW;
  }

  void Convolution(const std::vector<double>& s1,
                   const std::vector<double>& s2, std::vector<double>& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);

        FourierTrafo ft(size, true, false);
        ft.setInput(s1, s2); // combined FT for two functions: fc = s1 + i * s2

        std::vector<double> r1(size), i1(size);
        std::vector<double> r2(size), i2(size);

        ft.getResultFromReal(r1, i1);
        ft.getResultFromImag(r2, i2);

        std::vector<double> rc(size), ic(size);
        double efac = sqrt((double)size);

        for (int i = 0; i < size; i++)
          {
            // zc = z1 * z2
            rc[i] = (r1[i] * r2[i] - i1[i] * i2[i]) * efac;
            ic[i] = (r1[i] * i2[i] + r2[i] * i1[i]) * efac;
          }

        FourierTrafo fti(size, false, false);
        fti.setInput(rc, ic);
        fti.getResult(d);
      }
    RETHROW;
  }

#undef FNAME
#define FNAME "InvConvolution"
  void InvConvolution(const Vector& s1, const Vector& s2,
                      double noise, Vector& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);
        std::vector<double> h1(size);
        std::vector<double> h2(size);
        for (int i = 0; i < size; i++)
          h1[i] = s1[i];
        for (int i = 0; i < size; i++)
          h2[i] = s2[i];
        std::vector<double> dh;
        InvConvolution(s1, s2, noise, dh);
        d.Resize(size);
        for (int i = 0; i < size; i++)
          d[i] = dh[i];
      }
    RETHROW;
  }

  void InvConvolution(const std::vector<double>& s1,
                      const std::vector<double>& s2,
                      double noise, std::vector<double>& d)
  {
    try
      {
        int size = s1.size();
        if (s2.size() != size)
          throw IceException(FNAME, M_SIZES_DIFFER);

        FourierTrafo ft(size, true, false);
        ft.setInput(s1, s2); // combined FT for two functions: fc = s1 + i * s2

        std::vector<double> r1(size), i1(size);
        std::vector<double> r2(size), i2(size);

        ft.getResultFromReal(r1, i1);
        ft.getResultFromImag(r2, i2);

        std::vector<double> rc(size), ic(size);

        double efac = 1.0 / sqrt((double)size);
        double noise2 = noise * noise;

        for (int i = 0; i < size; i++)
          {
            double r3;
            double i3;
            double b1 = r1[i] * r1[i] + i1[i] * i1[i];
            if (noise == 0)
              {
                if (b1 == 0)
                  {
                    r3 = i3 = 0;
                  }
                else
                  {
                    r3 = (r2[i] * r1[i] + i2[i] * i1[i]) / b1 * efac;
                    i3 = (r1[i] * i2[i] - r2[i] * i1[i]) / b1 * efac;
                  }
              }
            else
              {
                b1 += noise2;
                r3 = (r2[i] * r1[i] + i2[i] * i1[i]) / b1 * efac;
                i3 = (r1[i] * i2[i] - r2[i] * i1[i]) / b1 * efac;
              }

            rc[i] = r3;
            ic[i] = i3;
          }

        FourierTrafo fti(size, false, false);
        fti.setInput(rc, ic);
        fti.getResult(d);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "ConvolutionImgD"
  ImageD ConvolutionImgD(ImageD is1, ImageD is2,
                         ImageD id, int mode)
  {
    try
      {
        int xs, ys;

        ImageD rc;
        MatchImgD(is1, is2, xs, ys);

        // center = \alpha_{0,0}
        int x0 = xs / 2;
        int y0 = ys / 2;

        ImageD ds1;
        ds1.create(xs, ys, 0, 1);
        ImageD ds2;
        ds2.create(xs, ys, 0, 1);

        if (! id.isValid())
          {
            rc.create(xs, ys, 0, 1);
          }
        else
          {
            rc = id;
          }

        ImageD ddi;
        ddi.create(xs, ys, 0, 1);

        // effective factor for Convolution
        double efac = sqrt((double)xs * (double)ys);

        FourierImgD(is1, is2, NORMAL, ds1, ds2); // "mixed" FT for both images

        for (int y = 0; y < ys; y++)
          {
            int yq = negf(y, ys);

            for (int x = 0; x < xs; x++)
              {
                int xq = negf(x, xs);

                double rr = GetValD(ds1, x, y);
                double ir = GetValD(ds2, x, y);
                double rq = GetValD(ds1, xq, yq);
                double iq = GetValD(ds2, xq, yq);
                // calculate complex FT-parameter from result of mixed FT
                double r1 = (rr + rq) / 2;
                double i1 = (ir - iq) / 2;
                double r2 = (ir + iq) / 2;
                double i2 = (rq - rr) / 2;
                // complex multiplication
                PutValD(rc, x, y, (r1 * r2 - i1 * i2) * efac);
                PutValD(ddi, x, y, (r1 * i2 + r2 * i1) * efac);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(rc, x0, y0, 0.0);
            PutValD(ddi, x0, y0, 0.0);
          }

        // inverse transform
        FourierImgD(rc, ddi, INVERS, rc, ddi);

        return rc;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "ConvolutionImg"
// ID = IS1 (*) IS2
  int ConvolutionImg(const Image& is1, const Image& is2,
                     Image& id, double factor, int mode)
  {
    try
      {
        int xs, ys;
        MatchImg(is1, is2, id, xs, ys);

        ImageD ds1 = NewImgD(is1);
        ConvImgImgD(is1, ds1, NORMALIZED, SIGNED);
        ImageD ds2 = NewImgD(is2);
        ConvImgImgD(is2, ds2, NORMALIZED, SIGNED);
        ImageD dd = NewImgD(xs, ys, 0, 1);

        ConvolutionImgD(ds1, ds2, dd, mode);

        if (factor != 0)
          {
            if (factor != 1.0)
              for (int y = 0; y < dd.ysize; ++y)
                for (int x = 0; x < dd.xsize; ++x)
                  {
                    PutValD(dd, x, y, factor * GetValD(dd, x, y));
                  }

            ConvImgDImg(dd, id, NORMALIZED, SIGNED);
          }
        else
          {
            ConvImgDImg(dd, id, ADAPTIVE, SIGNED);
          }
        return OK;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "InvConvolutionImgD"
// IS2 = ID (*) IS1
  ImageD InvConvolutionImgD(ImageD is1, ImageD is2,
                            ImageD id,
                            double noise, int mode)
  {
    try
      {
        int xs, ys;

        double rr, rq, ir, iq;
        double r1, r2, im1, im2;
        double r3 = 0, im3 = 0;
        double b1;
        double efactor;

        if (noise < 0)

          throw IceException(FNAME, M_WRONG_PARAM);

        double noise2 = noise * noise;

        MatchImgD(is1, is2, xs, ys);

        int x0 = xs / 2;
        int y0 = ys / 2;

        ImageD rc;

        if (id.isValid())
          {
            MatchImgD(is1, id);
            rc = id;
          }
        else
          {
            rc = NewImgD(xs, ys, 0, 1);
          }

        efactor = sqrt((double)(xs * ys));

        ImageD ds1 = NewImgD(xs, ys, 0, 1);
        ImageD ds2 = NewImgD(xs, ys, 0, 1);
        //  ImageD ddi=NewImgD(xs,ys,0,1);

        FourierImgD(is1, is2, NORMAL, ds1, ds2);

        for (int y = 0; y < ys; y++)
          {
            int yq = negf(y, ys);

            for (int x = 0; x < xs; x++)
              {
                int xq = negf(x, xs);
                rr = GetValD(ds1, x, y);
                ir = GetValD(ds2, x, y);
                rq = GetValD(ds1, xq, yq);
                iq = GetValD(ds2, xq, yq);

                r1 = (rr + rq) / 2;
                im1 = (ir - iq) / 2;
                r2 = (ir + iq) / 2;
                im2 = (rq - rr) / 2;
                b1 = r1 * r1 + im1 * im1;

                if (noise == 0)
                  {
                    if (b1 == 0)
                      {
                        r3 = im3 = 0;
                      }
                    else
                      {
                        r3 = (r2 * r1 + im2 * im1) / b1 * efactor;
                        im3 = (r1 * im2 - r2 * im1) / b1 * efactor;
                      }
                  }
                else
                  {
                    b1 += noise2;
                    r3 = (r2 * r1 + im2 * im1) / b1 * efactor;
                    im3 = (r1 * im2 - r2 * im1) / b1 * efactor;
                  }

                PutValD(rc, x, y, r3 - im3);
                //    PutValD(ddi,x,y,im3);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(rc, x0, y0, 0.0);
          }

        HartleyImgD(rc, rc);

        return rc;
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "InvConvolutionImg"
  int InvConvolutionImg(const Image& is1, const Image& is2,
                        Image& id,
                        double factor, double noise, int mode)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(MatchImg(is1, is2, id, xs, ys));

    ImageD ds1 = NewImgD(xs, ys);
    ConvImgImgD(is1, ds1, NORMALIZED, SIGNED);
    ImageD ds2 = NewImgD(xs, ys);
    ConvImgImgD(is2, ds2, NORMALIZED, SIGNED);
    ImageD dd = NewImgD(xs, ys);

    InvConvolutionImgD(ds1, ds2, dd, noise, mode);

    if (factor != 0.0)
      {
        if (factor != 1.0)
          for (int y = 0; y < dd.ysize; ++y)
            for (int x = 0; x < dd.xsize; ++x)
              {
                PutValD(dd, x, y, factor * GetValD(dd, x, y));
              }

        ConvImgDImg(dd, id, NORMALIZED, SIGNED);
      }
    else
      {
        ConvImgDImg(dd, id, ADAPTIVE, SIGNED);
      }

    return OK;
  }

  int InvConvolutionImg(const Image& is1, const Image& is2,
                        ImageD dd,
                        double noise, int mode)
  {
    int xs, ys;

    RETURN_ERROR_IF_FAILED(MatchImg(is1, is2, xs, ys));

    ImageD ds1 = NewImgD(xs, ys);
    ConvImgImgD(is1, ds1, NORMALIZED, SIGNED);
    ImageD ds2 = NewImgD(xs, ys);
    ConvImgImgD(is2, ds2, NORMALIZED, SIGNED);

    if (! dd.isValid())
      {
        ImageD dd = NewImgD(xs, ys);
      }
    else
      {
        if (xs != dd.xsize || ys != dd.ysize)
          throw IceException(FNAME, M_WRONG_IMGSIZE);
      }

    InvConvolutionImgD(ds1, ds2, dd, noise, mode);

    return OK;
  }
#undef FNAME
}

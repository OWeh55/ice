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
  void Convolution(const ImageD& is1, const ImageD& is2,
                   ImageD& id, int mode)
  {
    try
      {
        int xs, ys;

        MatchImgD(is1, is2, id, xs, ys);

        // center = \alpha_{0,0}
        int x0 = xs / 2;
        int y0 = ys / 2;

        ImageD ds1;
        ds1.create(xs, ys, 0, 1);
        ImageD ds2;
        ds2.create(xs, ys, 0, 1);

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
                PutValD(id, x, y, (r1 * r2 - i1 * i2) * efac);
                PutValD(ddi, x, y, (r1 * i2 + r2 * i1) * efac);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(id, x0, y0, 0.0);
            PutValD(ddi, x0, y0, 0.0);
          }

        // inverse transform
        FourierImgD(id, ddi, INVERS, id, ddi);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "Convolution"
// ID = IS1 (*) IS2
  void Convolution(const Image& is1, const Image& is2,
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

        Convolution(ds1, ds2, dd, mode);

        if (factor != 0) // gray value scaling given
          {
            if (factor != 1.0)
              for (int y = 0; y < dd.ysize; ++y)
                for (int x = 0; x < dd.xsize; ++x)
                  {
                    dd.setPixel(x, y, factor * dd.getPixel(x, y));
                  }

            ConvImgDImg(dd, id, NORMALIZED, SIGNED);
          }
        else
          {
            ConvImgDImg(dd, id, ADAPTIVE, SIGNED);
          }
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "InvConvolution"
// IS2 = ID (*) IS1
  void InvConvolution(const ImageD& is1, const ImageD& is2, ImageD& id,
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
          throw IceException(FNAME, M_WRONG_PARAMETER);

        double noise2 = noise * noise;

        MatchImgD(is1, is2, id, xs, ys);

        int x0 = xs / 2;
        int y0 = ys / 2;

        efactor = sqrt((double)(xs * ys));

        ImageD ds1;
        ds1.create(xs, ys, 0, 1);
        ImageD ds2;
        ds2.create(xs, ys, 0, 1);

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

                PutValD(id, x, y, r3 - im3);
                //    PutValD(ddi,x,y,im3);
              }
          }

        if ((mode & MD_BIAS) == MD_IGNORE_BIAS)
          {
            PutValD(id, x0, y0, 0.0);
          }

        HartleyImgD(id, id);
      }
    RETHROW;
  }
#undef FNAME

#define FNAME "InvConvolution"
  void InvConvolution(const Image& is1, const Image& is2, Image& id,
                      double factor, double noise, int mode)
  {
    try
      {
        int xs, ys;
        MatchImg(is1, is2, id, xs, ys);

        ImageD ds1;
        ds1.create(xs, ys);
        ConvImgImgD(is1, ds1, NORMALIZED, SIGNED);
        ImageD ds2;
        ds2.create(xs, ys);
        ConvImgImgD(is2, ds2, NORMALIZED, SIGNED);

        ImageD dd;
        dd.create(xs, ys);

        InvConvolution(ds1, ds2, dd, noise, mode);

        if (factor != 0.0)
          {
            if (factor != 1.0)
              for (int y = 0; y < dd.ysize; ++y)
                for (int x = 0; x < dd.xsize; ++x)
                  {
                    dd.setPixel(x, y, factor * dd.getPixel(x, y));
                  }

            ConvImgDImg(dd, id, NORMALIZED, SIGNED);
          }
        else
          {
            ConvImgDImg(dd, id, ADAPTIVE, SIGNED);
          }
      }
    RETHROW;
  }

  void InvConvolution(const Image& is1, const Image& is2,
                      ImageD& dd, double noise, int mode)
  {
    try
      {
        int xs, ys;

        MatchImg(is1, is2, xs, ys);
        if (dd.xsize != xs || dd.ysize != ys)
          throw IceException(FNAME, M_SIZES_DIFFER);

        ImageD ds1;
        ds1.create(xs, ys);
        ConvImgImgD(is1, ds1, NORMALIZED, SIGNED);
        ImageD ds2;
        ds2.create(xs, ys);
        ConvImgImgD(is2, ds2, NORMALIZED, SIGNED);

        InvConvolution(ds1, ds2, dd, noise, mode);
      }
    RETHROW;
  }
#undef FNAME
}

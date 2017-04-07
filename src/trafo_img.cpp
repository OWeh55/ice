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

#include "defs.h"
#include "IceException.h"

#include "base.h"
#include "Trafo.h"

#include "trafo_img.h"

//--------------------------------------------
namespace ice
{
#define FNAME "Transform"
  void Transform(const Trafo& tr,
                 const Image& hsimg, const Image& dimg,
                 int mode,
                 const Image& mark, int val)
  {
    int x, y;
    int mv1, mv2;
    double xf, yf;
    double fval;
    int xfi, yfi;

    Image tsimg;

    Trafo tri;

    if ((!IsImg(hsimg)) || (!IsImg(dimg)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
      throw IceException(FNAME, M_WRONG_DIM);

    try
      {
        // invert trafo
        tri = tr;
        tri.invert();

        // if source = dest then temporary Image needed
        if (hsimg == dimg)
          {
            tsimg = NewImg(hsimg, true);
          }
        else
          {
            tsimg = hsimg;
          }

        bool projective = true;

        double m00 = tri.Tmatrix()[0][0];
        double m01 = tri.Tmatrix()[0][1];
        double m02 = tri.Tmatrix()[0][2];
        double m10 = tri.Tmatrix()[1][0];
        double m11 = tri.Tmatrix()[1][1];
        double m12 = tri.Tmatrix()[1][2];
        double m20 = tri.Tmatrix()[2][0];
        double m21 = tri.Tmatrix()[2][1];
        double m22 = tri.Tmatrix()[2][2];
        double nn;

        if ((m20 == 0.0) && (m21 == 0.0) && (m22 == 1.0))
          {
            projective = false;
          }

        // max. values for grayvalue
        mv1 = tsimg->maxval + 1;
        mv2 = dimg->maxval + 1;

        if (mode != INTERPOL)
          {
            if (mv1 == mv2)
              {
                for (y = 0; y < dimg.ysize; y++)
                  for (x = 0; x < dimg.xsize; x++)
                    {
//      Transform(tri,x,y,xf,yf);
                      if (projective)
                        {
                          nn = m20 * x + m21 * y + m22;
                          xfi = RoundInt((m00 * x + m01 * y + m02) / nn);
                          yfi = RoundInt((m10 * x + m11 * y + m12) / nn);
                        }
                      else
                        {
                          xfi = RoundInt(m00 * x + m01 * y + m02);
                          yfi = RoundInt(m10 * x + m11 * y + m12);
                        }

                      if (tsimg.inside(xfi, yfi))
                        {
                          PutVal(dimg, x, y, GetValUnchecked(tsimg, xfi, yfi));
                        }
                      else
                        {
                          PutVal(dimg, x, y, 0);

                          if (IsImg(mark))
                            {
                              PutVal(mark, x, y, val);
                            }
                        }
                    }
              }
            else
              {
                for (y = 0; y < dimg.ysize; y++)
                  for (x = 0; x < dimg.xsize; x++)
                    {
//      Transform(tri,x,y,xf,yf);
                      if (projective)
                        {
                          nn = m20 * x + m21 * y + m22;
                          xfi = RoundInt((m00 * x + m01 * y + m02) / nn);
                          yfi = RoundInt((m10 * x + m11 * y + m12) / nn);
                        }
                      else
                        {
                          xfi = RoundInt(m00 * x + m01 * y + m02);
                          yfi = RoundInt(m10 * x + m11 * y + m12);
                        }

#if 0
                      PutVal(dimg, x, y,
                             GetValClipped(tsimg, xfi, yfi)*mv2 / mv1);
#else

                      if (tsimg.inside(xfi, yfi))
                        {
                          PutValUnchecked(dimg, x, y, GetValUnchecked(tsimg, xfi, yfi)*mv2 / mv1);
                        }
                      else
                        {
                          PutValUnchecked(dimg, x, y, 0);

                          if (IsImg(mark))
                            {
                              PutValUnchecked(mark, x, y, val);
                            }
                        }

#endif

                    }
              }
          }
        else     // if (mode!=INTERPOL)
          {
            if (mv1 == mv2)
              {
                for (y = 0; y < dimg.ysize; y++)
                  for (x = 0; x < dimg.xsize; x++)
                    {
//      Transform(tri,x,y,xf,yf);
                      if (projective)
                        {
                          nn = m20 * x + m21 * y + m22;
                          xf = (m00 * x + m01 * y + m02) / nn;
                          yf = (m10 * x + m11 * y + m12) / nn;
                        }
                      else
                        {
                          xf = m00 * x + m01 * y + m02;
                          yf = m10 * x + m11 * y + m12;
                        }

                      if (GetInterpolVal(tsimg, xf, yf, fval))
                        {
                          PutValUnchecked(dimg, x, y, RoundInt(fval));
                        }
                      else
                        {
                          PutValUnchecked(dimg, x, y, 0);

                          if (IsImg(mark))
                            {
                              PutValUnchecked(mark, x, y, val);
                            }
                        }
                    }
              }
            else
              {
                double gfak = ((double)dimg->maxval) / ((double)tsimg->maxval);
                for (y = 0; y < dimg.ysize; y++)
                  for (x = 0; x < dimg.xsize; x++)
                    {
//      Transform(tri,x,y,xf,yf);
                      if (projective)
                        {
                          nn = m20 * x + m21 * y + m22;
                          xf = (m00 * x + m01 * y + m02) / nn;
                          yf = (m10 * x + m11 * y + m12) / nn;
                        }
                      else
                        {
                          xf = m00 * x + m01 * y + m02;
                          yf = m10 * x + m11 * y + m12;
                        }

                      if (GetInterpolVal(tsimg, xf, yf, fval))
                        {
                          PutValUnchecked(dimg, x, y, RoundInt(fval * gfak));
                        }
                      else
                        {
                          PutValUnchecked(dimg, x, y, 0);

                          if (IsImg(mark))
                            {
                              PutValUnchecked(mark, x, y, val);
                            }
                        }
                    }
              }
          }
      }
    RETHROW;
  }

  void Transform(const Trafo& tr,
                 const Image& hsimg, const Image& dimg, int mode)
  {
    Image i;
    Transform(tr, hsimg, dimg, mode, i, 0);
  }
#undef FNAME
}

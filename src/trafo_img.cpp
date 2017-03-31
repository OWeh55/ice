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
#include "macro.h"
#include "message.h"

#include "base.h"
#include "Trafo.h"

#include "trafo_img.h"

//--------------------------------------------
namespace ice
{
#define FNAME "Transform"
  int Transform(const Trafo& tr,
                const Image& hsimg, const Image& dimg, int mode, const Image& mark, int val)
  {
    int x, y;
    int mv1, mv2;
    double xf, yf;
    double fval;
    int xfi, yfi;
    int has_temp = false;
    Image tsimg;

    Trafo tri;

    if ((!IsImg(hsimg)) || (!IsImg(dimg)))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
      {
        Message(FNAME, M_WRONG_DIM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // invert trafo
    tri = tr;
    RETURN_ERROR_IF_FAILED(tri.Invert());

    // if source=dest then temporary Image needed
    if (hsimg == dimg)
      {
        tsimg = NewImg(hsimg, true);
        has_temp = true;
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

//    if ((tri.m[2][0]==0.0)&&(tri.m[2][1]==0.0)&&(tri.m[2][2]==1.0))
//      projective=false;
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
            wloop(dimg, x, y)
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
                     GetValClipped(tsimg, xfi, yfi));
#else

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

#endif
            }
          }
        else
          {
            wloop(dimg, x, y)
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
            wloop(dimg, x, y)
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
            wloop(dimg, x, y)
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

    if (has_temp)
      {
        FreeImg(tsimg);
      }

    return OK;
  }

  int Transform(const Trafo& tr,
                const Image& hsimg, const Image& dimg, int mode)
  {
    Image i;
    return Transform(tr, hsimg, dimg, mode, i, 0);
  }
#undef FNAME
}

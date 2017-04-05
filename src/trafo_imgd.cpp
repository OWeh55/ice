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

#include "based.h"
#include "Trafo.h"

#include "trafo_imgd.h"

//--------------------------------------------
namespace ice
{
#define FNAME "Transform"
  void Transform(const Trafo& tr,
                 const ImageD& hsimg, ImageD& dimg, int mode, Image& mark, int val)
  {
    double xf, yf;
    double fval;
    int xfi, yfi;

    ImageD tsimg;

    Trafo tri;

    if ((!IsImgD(hsimg)) || (!IsImgD(dimg)))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((tr.DimSource() != 2) || (tr.DimTarget() != 2))
      throw IceException(FNAME, M_WRONG_DIM);

    try
      {
        // invert trafo
        tri = tr;
        tri.Invert();

        // if source=dest then temporary Image needed
        if (hsimg == dimg)
          {
            tsimg = NewImgD(hsimg, true);
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

        if (mode != INTERPOL)
          {
            for (int y = 0; y < tsimg.ysize; ++y)
              for (int x = 0; x < tsimg.xsize; ++x)
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

                  if (Inside(tsimg, xfi, yfi))
                    {
                      PutValD(dimg, x, y, GetValD(tsimg, xfi, yfi));
                    }
                  else
                    {
                      PutValD(dimg, x, y, 0);

                      if (IsImg(mark))
                        {
                          PutVal(mark, x, y, val);
                        }
                    }
                }
          }

        else   // if (mode!=INTERPOL)
          {
            for (int y = 0; y < dimg.ysize; ++y)
              for (int x = 0; x < dimg.xsize; ++x)
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

                  if (GetInterpolValD(tsimg, xf, yf, fval))
                    {
                      PutValD(dimg, x, y, fval);
                    }
                  else
                    {
                      PutValD(dimg, x, y, 0.0);

                      if (IsImg(mark))
                        {
                          PutValUnchecked(mark, x, y, val);
                        }
                    }
                }
          }
      }
    RETHROW;
  }

  void Transform(const Trafo& tr,
                 const ImageD& hsimg, ImageD& dimg, int mode)
  {
    Image dummy;
    Transform(tr, hsimg, dimg, mode, dummy, 1);
  }

#undef FNAME
}

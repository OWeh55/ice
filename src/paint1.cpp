/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002..2013 FSU Jena, Digital Image Processing Group
 * Contact: ice@inf.uni-jena.de
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
/********************************************************************
Programmmodul zu Zeichnen geometrischer Primitive
  Marker

  Line
  drawCircle
  drawEllipse
*********************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "IceException.h"
#include "defs.h"
#include "macro.h"
#include "contools.h"
#include "contfeat.h"
#include "paint.h"
#include "drawline.h"

namespace ice
{
  /*******************************************************************/
#define FNAME "Marker"
  int Marker(int mode, const Point& xy, int val, int size, const Image& img)
  {
    return Marker(mode, RoundInt(xy.x), RoundInt(xy.y), val, size, img);
  }

  int Marker(int mode, const IPoint& xy, int val, int size, const Image& img)
  {
    return Marker(mode, xy.x, xy.y, val, size, img);
  }

  int Marker(int mode, const IVector& xy, int val, int size, const Image& img)
  {
    return Marker(mode, xy[0], xy[1], val, size, img);
  }

  int Marker(int mode, int x, int y, int val, int size, const Image& img)
  {
    int i, h;
    Contur c;
    double par[3];

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((val < 0) || (val > img.maxval))
      throw IceException(FNAME, M_WRONG_VALUE);

    if (size < 2)
      throw IceException(FNAME, M_WRONG_SIZE);

    if ((x < 0) || (y < 0) || (x >= img->xsize) || (y >= img->ysize))
      {
        // ignore points outside here
        return OK;
      }

    h = size / 2;

    if ((mode == 2) && (size < 6))
      {
        mode = 4;
      }

    if ((mode == 3) && (size < 6))
      {
        mode = 5;
      }

    par[0] = x;
    par[1] = y;
    par[2] = h;

    switch (mode)
      {
      case DEFAULT: /* achsparalleles Kreuz */
        Line(x - h, y, x + h, y, val, DEFAULT, img);
        Line(x, y - h, x, y + h, val, DEFAULT, img);
        break;
      case 1: /* schraeges Kreuz */
        Line(x - h, y - h, x + h, y + h, val, DEFAULT, img);
        Line(x + h + 1, y - h - 1, x - h, y + h, val, DEFAULT, img);
        break;
      case 2: /* leerer Kreis */
        drawCircle(par, val, 0, NOFILL, img);
        break;
      case 3: /* voller Kreis */
        drawCircle(par, val, val, DEFAULT, img);
        break;
      case 4: /* leeres achsparalleles Quadrat */
        Line(x - h, y - h, x + h, y - h, val, DEFAULT, img);
        Line(x + h, y - h, x + h, y + h, val, DEFAULT, img);
        Line(x + h, y + h, x - h, y + h, val, DEFAULT, img);
        Line(x - h, y + h, x - h, y - h, val, DEFAULT, img);
        break;
      case 5: /* volles achsparalleles Quadrat */

        for (i = y - h; i < y + h; i++)
          {
            Line(x - h, i, x + h, i, val, DEFAULT, img);
          }

        break;
      case 6: /* leeres schraeges Quadrat */
        Line(x - h, y, x, y - h, val, DEFAULT, img);
        Line(x, y - h, x + h, y, val, DEFAULT, img);
        Line(x + h, y, x, y + h, val, DEFAULT, img);
        Line(x, y + h, x - h, y, val, DEFAULT, img);
        break;
      case 7: /* volles schraeges Quadrat */
        c.setStart(x - h, y);
        c.add(x, y - h);
        c.add(x + h, y);
        c.add(x, y + h);
        c.add(x - h, y);

        FillRegion(c, val, img);
        break;
      default:
        throw IceException(FNAME, M_WRONG_MODE);
      }

    return OK;
  }
#undef FNAME

  /********************************************************************/
#define FNAME "drawCircle"
  int drawCircle(double* par, int val1, int val2, int mode, const Image& img)
  {
    Contur c;

    if (
      (val1 < 0) || (val2 > img.maxval) ||
      (val2 < 0) || (val2 > img.maxval)
    )
      throw IceException(FNAME, M_WRONG_VALUE);

    if (mode != DEFAULT && mode != NOFILL)
      throw IceException(FNAME, M_WRONG_MODE);

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    RETURN_ERROR_IF_FAILED(c = CircleContur(par));

    if (mode == DEFAULT)
      {
        FillRegion(c, val2, img);
      }

    MarkContur(c, val1, img);

    return OK;
  }
#undef FNAME
  /*********************************************************************/
#define FNAME "drawEllipse"

  int drawEllipse(double* par, int val1, int val2, int mode, const Image& img)
  {
    Contur c;

    if ((val1 < 0) || (val2 < 0) || (val2 > img.maxval))
      throw IceException(FNAME, M_WRONG_VALUE);

    if (mode != DEFAULT && mode != NOFILL)
      throw IceException(FNAME, M_WRONG_MODE);

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    RETURN_ERROR_IF_FAILED(c = EllipseContur(par));

    if (c.isClosed())
      {
        if (mode != NOFILL)
          {
            RETURN_ERROR_IF_FAILED(FillRegion(c, val2, img));
          }

        MarkContur(c, val1, img);
      }

    return OK;
  }
#undef FNAME

#define FNAME "drawSuperEllipse"
  int drawSuperEllipse(double c, double tr[3][3], int val, const Image& pic)
  {
    int dimx, dimy;
    dimx = pic->xsize;
    dimy = pic->ysize;
    int i, j;
    double xs, ys, xs1, ys1;
    double z1u, z1o;

    if ((val < 0) || val > pic.maxval)
      throw IceException(FNAME, M_WRONG_VALUE);

    for (j = 0; j < dimy; ++j)
      for (i = 0; i < dimx; ++i)
        {
          xs = tr[0][0] * (double)i + tr[0][1] * (double)j + tr[0][2];
          ys = tr[1][0] * (double)i + tr[1][1] * (double)j + tr[1][2];
          xs1 = tr[0][0] * ((double)i + 1.0) + tr[0][1] * (double)j + tr[0][2];
          ys1 = tr[1][0] * ((double)i + 1.0) + tr[1][1] * (double)j + tr[1][2];
          xs = fabs(xs);
          ys = fabs(ys);
          xs1 = fabs(xs1);
          ys1 = fabs(ys1);
          z1u = pow(xs, c) + pow(ys, c) - 1.0;
          z1o = pow(xs1, c) + pow(ys1, c) - 1.0;

          if (z1u <= 0 && z1o >= 0)
            {
              PutVal(pic, i, j, val);
            }

          if (z1u >= 0 && z1o <= 0)
            {
              PutVal(pic, i, j, val);
            }
        }

    for (i = 0; i < dimx; ++i)
      for (j = 0; j < dimy; ++j)
        {
          xs = tr[0][0] * (double)i + tr[0][1] * (double)j + tr[0][2];
          ys = tr[1][0] * (double)i + tr[1][1] * (double)j + tr[1][2];
          xs1 = tr[0][0] * (double)i + tr[0][1] * ((double)j + 1.0) + tr[0][2];
          ys1 = tr[1][0] * (double)i + tr[1][1] * ((double)j + 1.0) + tr[1][2];
          xs = fabs(xs);
          ys = fabs(ys);
          xs1 = fabs(xs1);
          ys1 = fabs(ys1);
          z1u = pow(xs, c) + pow(ys, c) - 1.0;
          z1o = pow(xs1, c) + pow(ys1, c) - 1.0;

          if (z1u <= 0 && z1o >= 0)
            {
              PutVal(pic, i, j, val);
            }

          if (z1u >= 0 && z1o <= 0)
            {
              PutVal(pic, i, j, val);
            }
        }

    return 0;
  }
#undef FNAME
}

/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "macro.h"
#include "paint.h"
#include "draw.h"
#include "drawline.h"

using namespace std;

namespace ice
{
#define FNAME "draw"
  int draw(const Circle& c, const Image& img, int val, int fval)
  {
    double par[3];
    par[0] = c.getPos().x;
    par[1] = c.getPos().y;
    par[2] = c.getR();
    int mode = DEFAULT;

    if (fval == -1)
      {
        mode = NOFILL;
        fval = 0;
      }

    RETURN_ERROR_IF_FAILED(drawCircle(par, val, fval, mode, img));
    return OK;
  }

  int draw(const CircleSeg& cs, const Image& img, int val, int fval)
  {
    double par[5];
    Point p = cs.getPos();
    double rad = cs.getR();
    double phi1 = cs.getPhi1();
    double phi2 = cs.getPhi2();
    par[0] = p.x;
    par[1] = p.y;
    par[2] = rad;
    par[3] = phi1;
    par[4] = phi2;

    int mode = DEFAULT;

    if (fval == -1)
      {
        mode = NOFILL;
        fval = 0;
      }

    RETURN_ERROR_IF_FAILED(drawCircleSegment(par, val, fval, mode, img));
    RETURN_ERROR_IF_FAILED(draw(LineSeg(p, Point(p.x + rad * cos(phi1), p.y + rad * sin(phi1))), img, val));
    RETURN_ERROR_IF_FAILED(draw(LineSeg(p, Point(p.x + rad * cos(phi2), p.y + rad * sin(phi2))), img, val));
    return OK;
  }

  int draw(const Ellipse& e, const Image& img, int val, int fval)
  {
    double par[5];
    par[0] = e.getPos().x;
    par[1] = e.getPos().y;
    par[2] = e.getR();
    par[3] = e.getR2();
    par[4] = e.getPhi();
    int mode = DEFAULT;

    if (fval == -1)
      {
        mode = NOFILL;
        fval = 0;
      }

    RETURN_ERROR_IF_FAILED(drawEllipse(par, val, fval, mode, img));
    return OK;
  }

  int draw(const EllipseSeg& es, const Image& img, int val, int fval)
  {
    int mode = DEFAULT;

    if (fval == -1)
      {
        mode = NOFILL;
      }

    double par[7];
    par[0] = es.getPos().x;
    par[1] = es.getPos().y;
    par[2] = es.getR();
    par[3] = es.getR2();
    par[4] = es.getPhi();
    par[5] = es.getPhi1();
    par[6] = es.getPhi2();
    RETURN_ERROR_IF_FAILED(drawEllipseSegment(par, val, fval, mode, img));
    return OK;
  }

//=======================================================

  int draw(const LineSeg& ls, const Image& img, int val, int fval)
  {
    /* Parametertestung */
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((val < 0) || (val > img.maxval))
      throw IceException(FNAME, M_VALTOOSMALL);

    double mya = 0.0; // Rel. Anfangspunkt
    double mye = 1.0; // Rel. Endpunkt

    Point dummy;

    Point p = ls.getP1();
    Point dp = ls.getP2() - p;

    //  cout << p.x <<"," << p.y << ".." << dp.x << "," << dp.y << endl;

    bool ou = false;

    const double eps = 1e-12;

    // test linker und rechter Rand
    if (fabs(dp.x) > eps)
      {
        mya = (0 - p.x) / dp.x;
        mye = (img->xsize - 1 - p.x) / dp.x;

        if (!ls.limitMys(mya, mye))
          {
            return NO_PAINT;
          }

        ou = true;
      }
    else
      {
        if ((p.x < 0) || (p.x >= img->xsize))
          {
            return NO_PAINT;
          }
      }

    // test oberer und unterer Rand
    if (fabs(dp.y) > eps)
      {
        double my1, my2;
        my1 = (0 - p.y) / dp.y;
        my2 = (img->ysize - 1 - p.y) / dp.y;

        if (!ls.limitMys(my1, my2))
          {
            return NO_PAINT;
          }

        if (ou)
          {
            if (my1 > mya)
              {
                mya = my1;
              }

            if (my2 < mye)
              {
                mye = my2;
              }
          }
        else
          {
            mya = my1;
            mye = my2;
          }
      }
    else
      {
        if ((p.y < 0) || (p.y >= img->ysize))
          {
            return NO_PAINT;
          }
      }

    Point p1 = ls.RelPoint(mya);
    Point p2 = ls.RelPoint(mye);

    //cout << p1.x <<"," << p1.y << ".." << p2.x << "," << p2.y << endl;

    Line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, val, img);
    return OK;
  }

  int draw(const PolygonalCurve& poly, const Image& img, int val, int fval)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (poly.isClosed() && fval >= 0)
      {
        IPoint p;
        for (p.y = 0; p.y < img->ysize; p.y++)
          for (p.x = 0; p.x < img->xsize; p.x++)
            if (poly.Inside(p))
              {
                PutVal(img, p, fval);
              }
      }

    unsigned int last = poly.size() - 1;

    if (poly.isClosed())
      {
        last = poly.size();
      }

    if (val >= 0)
      {
        for (unsigned int i = 0; i < last; i++)
          {
            draw(poly.Edge(i), img, val);
          }
      }
    return OK;
  }

  int draw(const Triangle& triangle, const Image& img, int val, int fval)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);
    if (fval >= 0)
      {
        Region region;
        Triangle2Region(triangle, region);
        region.draw(img, fval);
      }
    if (val >= 0 && val != fval)
      {
        Line(triangle.P1(), triangle.P2(), val, img);
        Line(triangle.P2(), triangle.P3(), val, img);
        Line(triangle.P3(), triangle.P1(), val, img);
      }
    return OK;
  }

  int draw(const Forest& f, const std::vector<Point>& pl,
           const Image& img, int val)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (f.Size() != (int)pl.size())
      throw IceException(FNAME, M_WRONG_DIMENSION);

    for (int i = 0; i < f.Size(); i++)
      {
        if (!f.isRoot(i))
          {
            int idx = f.Father(i);
            Line(pl[i], pl[idx], val, img);
          }
      }
    return OK;
  }

#undef FNAME
}

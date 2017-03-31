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
 * Programmmodul zu Zeichnen von Linien
 *
 * Line
 * HesseLine
 */

#include <algorithm>

#include "macro.h"
#include "defs.h"
#include "message.h"
#include "numbase.h"
#include "drawline.h"
#include "lineseg.h"
#include "draw.h"

/*
 * Line - neubauer 9/93
 *        ortmann  8/02
 *        ortmann  5/04
 *        ortmann  11/10
 *
 *  Zeichnen einer Linie im Bild "img" mit dem Wert "val":
 *
 *  mode=DEFAULT -  Zeichnen des Liniensegmentes
 *      von (x1,y1) bis (x2,y2)
 *  mode=1       -  Zeichnen der Gerade durch (x1,y1) bzw. (x2,y2)
 *  mode=2       -  Zeichnen des Strahls von (x1,y1) durch (x2,y2)
 *
 * e_code: OK,
 *    WRONG_POINTER,
 *    WRONG_PARAM,
 *    NO_PAINT,
 *    ERROR
 */
namespace ice
{

//=======================================================
  /*
   * Hilfsfunktionen: Zeichnen einer Linie
   * hier keine Testung der Parameter
   *
   */
  static void _LineX(int x1, int x2, int y, int val, const Image& img)
  {
    for (int x = x1; x <= x2; x++)
      {
        img.setPixelClipped(x, y, val);
      }
  }

  static void _LineY(int x, int y1, int y2, int val, const Image& img)
  {
    for (int y = y1; y <= y2; y++)
      {
        img.setPixelClipped(x, y, val);
      }
  }

  void _Line(int x1, int y1, int x2, int y2, int val, const Image& img)
  {
    // cout << x1 << "," << y1 << "  --- " << x2 << "," << y2 << endl;
    int dy = y2 - y1;

    if (dy == 0)
      {
        _LineX(x1, x2, y1, val, img);
      }

    int dx = x2 - x1;

    if (dx == 0)
      {
        _LineY(x1, y1, y2, val, img);
      }

    if (abs(dy) <= abs(dx))
      {
        // Fortschreiten in X-Richtung
        if (dx < 0)
          {
            // Richtung umkehren
            std::swap(x1, x2);
            std::swap(y1, y2);
            dy = -dy;
            dx = -dx;
          }

        int y = y1;

        if (dy >= 0)
          {
            int d = dx / 2; // == 0.5

            for (int i = x1; i <= x2; i++)
              {
                img.setPixelClipped(i, y, val);
                d += dy;

                if (d > dx)
                  {
                    y++;
                    d -= dx;
                  }
              }
          }
        else
          {
            int d = -dx / 2;

            for (int i = x1; i < x2; i++)
              {
                img.setPixelClipped(i, y, val);
                d -= dy;

                if (d > dx)
                  {
                    y--;
                    d -= dx;
                  }
              }
          }
      }
    else
      {
        if (dy < 0)
          {
            // Richtung umkehren
            std::swap(x1, x2);
            std::swap(y1, y2);
            dy = -dy;
            dx = -dx;
          }

        int x = x1;

        if (dx >= 0)
          {
            int d = dx / 2;

            for (int i = y1; i < y2; i++)
              {
                img.setPixelClipped(x, i, val);
                d += dx;

                if (d > dy)
                  {
                    x++;
                    d -= dy;
                  }
              }
          }
        else
          {
            int d = -dx / 2;

            for (int i = y1; i < y2; i++)
              {
                img.setPixelClipped(x, i, val);
                d -= dx;

                if (d > dy)
                  {
                    x--;
                    d -= dy;
                  }
              }
          }
      }
  }

#define FNAME "Line"
  int Line(int x1, int y1, int x2, int y2,
           int val, const Image& img)
  {
    /* Parametertestung */
    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return WRONG_POINTER;
      }

    if ((val < 0) || (val > img->maxval))
      {
        Message(FNAME, M_VALTOOSMALL, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (img.inside(x1, y1) && img.inside(x2, y2))   // keine weitere Umrechnungen
      {
        _Line(x1, y1, x2, y2, val, img);
        return OK;
      }

    double mya = 0.0; // Rel. Anfangspunkt
    double mye = 1.0; // Rel. Endpunkt

    Point dummy;

    Point p1(x1, y1);
    Point p2(x2, y2);
    Point dp = p2 - p1;
    LineSeg ls(p1, p2);
    //  cout << p.x <<"," << p.y << ".." << dp.x << "," << dp.y << endl;

    bool ou = false;

    const double eps = 1e-12;

    // test linker und rechter Rand
    if (fabs(dp.x) > eps)
      {
        mya = (0 - p1.x) / dp.x;
        mye = (img->xsize - 1 - p1.x) / dp.x;

        if (!ls.limitMys(mya, mye))
          {
            return NO_PAINT;
          }

        ou = true;
      }
    else
      {
        if ((p1.x < 0) || (p1.x >= img->xsize))
          {
            return NO_PAINT;
          }
      }

    // test oberer und unterer Rand
    if (fabs(dp.y) > eps)
      {
        double my1, my2;
        my1 = (0 - p1.y) / dp.y;
        my2 = (img->ysize - 1 - p1.y) / dp.y;

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
        if ((p1.y < 0) || (p1.y >= img->ysize))
          {
            return NO_PAINT;
          }
      }

    p1 = ls.RelPoint(mya);
    p2 = ls.RelPoint(mye);

    _Line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, val, img);
    return OK;
  }

// int parameter, no mode
  int Line(const IVector& p1, const IVector& p2, int val, const Image& img)
  {
    if ((p1.Size() != 2) || (p2.Size() != 2))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return Line(p1[0], p1[1], p2[0], p2[1], val, img);
  }

  int Line(IPoint p1, IPoint p2, int val, const Image& img)
  {
    return Line(p1.x, p1.y, p2.x, p2.y, val, img);
  }

// double parameter, no mode
  int Line(Point p1, Point p2,
           int val, const Image& img)
  {
    return Line((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, val, img);
  }

// mode parameter requires LineSeg

  int Line(Point p1, Point p2,
           int val, int mode, const Image& img)
  {
    int lmode;

    switch (mode)
      {
      case DEFAULT:
        lmode = LineSeg::segment;
        break;
      case 1:
        lmode = LineSeg::line;
        break;
      case 2:
        lmode = LineSeg::ray;
        break;
      default:
      {
        Message(FNAME, M_WRONG_MODE, WRONG_PARAM);
        return WRONG_PARAM;
      }
      }

    if (lmode == LineSeg::segment)
      {
        return Line(p1, p2, val, img);
      }
    else
      {
        return draw(LineSeg(p1, p2, lmode), img, val);
      }
  }

  int Line(const IVector& p1, const IVector& p2, int val, int mode, const Image& img)
  {
    if ((p1.Size() != 2) || (p2.Size() != 2))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return Line(Point(p1[0], p1[1]), Point(p2[0], p2[1]), val, mode, img);
  }

  int Line(double x1, double y1, double x2, double y2, int val, int mode, const Image& img)
  {
    return Line(Point(x1, y1), Point(x2, y2), val, mode, img);
  }

  int Line(double x1, double y1, double x2, double y2, int val, const Image& img)
  {
    return Line((int)x1, (int)y1, (int)x2, (int)y2, val, img);
  }
#undef FNAME

  /*******************************************************************/
#define FNAME "HesseLine"
  int HesseLine(double p, double phi, int val, const Image& img)
  {
    return draw(LineSeg(p, phi), img, val);
  }

  int HesseLine(const Vector& p, int val, const Image& img)
  {
    if (p.Size() != 2)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    return draw(LineSeg(p[0], p[1]), img, val);
  }
#undef FNAME

}

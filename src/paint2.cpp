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
 * Programmmodul zu Zeichnen geometrischer Primitive
 *  CircleSegment
 *  EllipseSegment
 *  PaintFunc
 */

#include <cstdio>
#include <cmath>
#include <cstdlib>

#include "IceException.h"
#include "defs.h"
#include "macro.h"

#include "Contur.h"
#include "contools.h"
#include "contfeat.h"

#include "paint.h"

using namespace std;

namespace ice
{
  /* Nichtoeffentliche Funktionen */
  int first(const Image& imgg, const Image& imgo,
            double(*func)(double*, int, int),
            double* parray, int xc, int yc, int* xs, int* ys);

  bool decision(double(*func)(double*, int, int),
                double* parr,
                int x, int y,
                int xsize, int ysize);

  int calc_function(const Image& img, const Image& imgh,
                    double(*func)(double*, int, int),
                    double* parray, int xs, int ys, int val);

  /*********************************************************************/
#define FNAME "CircleSegment"
  int drawCircleSegment(double* par, int val1, int val2, int mode, const Image& img)
  {
    Contur c;

    if ((val1 < 0) || (val2 < 0) || (val1 > img.maxval) || (val2 > img.maxval))
      throw IceException(FNAME, M_WRONG_VAL);

    if (mode != DEFAULT && mode != NOFILL)
      throw IceException(FNAME, M_WRONG_MODE);

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    // offene Contur des Kreisbogensegments
    RETURN_ERROR_IF_FAILED(c = CircleSegmentContur(par));

    if (mode != NOFILL)
      {
        // das innere fuellen
        Contur c1(c);
        c1.add(RoundInt(par[0]), RoundInt(par[1]));
        c1.add(c1.StartX(), c1.StartY());
        RETURN_ERROR_IF_FAILED(FillRegion(c1, val2, img));
      }

    RETURN_ERROR_IF_FAILED(MarkContur(c, val1, img));

    return OK;
  }
#undef FNAME
  /*********************************************************************/
#define FNAME "EllipseSegment"
  int drawEllipseSegment(double* par, int val1, int val2, int mode, const Image& img)
  {
    Contur c;

    if ((val1 < 0) || (val2 < 0) ||
        (val1 > img.maxval) || (val2 > img.maxval))
      throw IceException(FNAME, M_WRONG_VAL);

    if (mode != DEFAULT && mode != NOFILL)
      throw IceException(FNAME, M_WRONG_MODE);

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    RETURN_ERROR_IF_FAILED(c = EllipseSegmentContur(par));

    if (mode != NOFILL)
      {
        Contur c1(c);
        c.add(RoundInt(par[0]), RoundInt(par[1]));
        c.add(c.StartX(), c.StartY());

        RETURN_ERROR_IF_FAILED(FillRegion(c, val2, img));
      }

    RETURN_ERROR_IF_FAILED(MarkContur(c, val1, img));
    return OK;
  }
#undef FNAME
  /*******************************************************************/
#define FNAME "PaintFunc"
  int PaintFunc(double(*func)(double*, int, int), double* parray,
                int val, const Image& img)
  {
    Image imgh;
    int xc, yc;
    int xs, ys;
    int ast;

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    imgh = NewImg(img->xsize, img->ysize, 255);
    imgh.set(0);
    ast = 0;
    xc = 1;
    yc = 1;

    while (first(img, imgh, func, parray, xc, yc, &xs, &ys) == EXIST)
      {
        xc = xs + 1;

        if (xc >= img->xsize)
          {
            xc = 0;
            yc++;
          }

        if (calc_function(img, imgh, func, parray, xs, ys, val) == EXIST)
          {
            ast++;
          }
      }

    if (ast == 0)
      {
        return NO_EXIST;
      }

    return OK;
  }
#undef FNAME
#define FNAME "first"
  /*******************************************************************/
  int first(const Image& img, const Image& imgh,
            double(*func)(double*, int, int), double* parray,
            int xc, int yc, int* xs, int* ys)
  {
    int x, y, dx, dy;
    int diff = 20;
    int result;

    RETURN_ERROR_IF_FAILED(MatchImg(img, imgh, dx, dy));

    if ((dx < 4) || (dy < 4))
      {
        throw IceException(FNAME, M_WRONG_WINDOW2);
        *xs = *ys = 0;
        return WRONG_WINDOW;
      }

    if (! Inside(img, xc, yc))
      {
        *xs = *ys = 0;
        throw IceException(FNAME, M_WRONG_STARTPOINT);
      }

    for (y = yc; y < img->ysize; y += diff)
      {
        for (x = (y == yc) ? xc : 0; x < img->xsize; x++)
          {
            if (decision(func, parray, x, y, dx, dy))
              {
                /*moeglicher Objektpunkt gefunden*/
                /*Testen aller Nachbarn in 4er Umgebung*/
                result = 0;
                if (decision(func, parray, x, y + 1, dx, dy))
                  {
                    result++;
                  }
                if (decision(func, parray, x, y - 1, dx, dy))
                  {
                    result++;
                  }
                if (decision(func, parray, x + 1, y, dx, dy))
                  {
                    result++;
                  }
                if (decision(func, parray, x - 1, y, dx, dy))
                  {
                    result++;
                  }

                if (GetVal(imgh, x - 1, y - 1) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x - 1, y) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x - 1, y + 1) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x, y + 1) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x, y - 1) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x + 1, y - 1) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x + 1, y) > 0)
                  {
                    continue;
                  }

                if (GetVal(imgh, x + 1, y + 1) > 0)
                  {
                    continue;
                  }

                /*
                       printf("result: %d\n",result);
                       getch();
                */
                if ((result != 0) && (result != 4) &&
                    (x != 0) && (y != 0) && (x != dx - 1) && (y != dy - 1))
                  {
                    *xs = x;
                    *ys = y;
                    return EXIST;
                  }
              }
          }
      }

    return NO_EXIST;
  }
#undef FNAME
  /*******************************************************************/
  bool decision(double(*func)(double*, int, int), double* parray, int x, int y, \
                int sx, int sy)
  {
    return x >= 0 && y >= 0 && x < sx && y < sy && func(parray, x, y) >= 0;
  }

  /*******************************************************************/
#define FNAME "calcfunction"
  int calc_function(const Image& img, const Image& imgh, double(*func)(double*, int, int),
                    double* parray, int xs, int ys, int val)
  {
    int dx, dy;
    int xxo, yyo;
    int exist;

    RETURN_ERROR_IF_FAILED(MatchImg(img, imgh, dx, dy));

    if ((dx < 1) || (dy < 1))
      throw IceException(FNAME, M_WRONG_WINDOW2);

    if (decision(func, parray, xs, ys, dx, dy) == 0)
      throw IceException(FNAME, M_WRONG_STARTPOINT);

    int xx = xs;
    int yy = ys;

    int ct = 0;
    Freeman Direction(-2);

    do
      {
        Direction += 2;
        Direction.move(xs, ys, xx, yy);
        ct++;
      }
    while (ct < 4 && decision(func, parray, xx, yy, dx, dy) == 1);

    if (ct >= 4)
      throw IceException(FNAME, M_WRONG_STARTPOINT3);

    ct = 0;    /* Zaehler der untersuchten Richtungen */

    do
      {
        Direction += 2;
        Direction.move(xs, ys, xx, yy);
        ct++;
      }
    while (ct < 4 && decision(func, parray, xx, yy, dx, dy) == 0);

    if (ct >= 4)
      {
        throw IceException("calc_function", M_WRONG_STARTPOINT2);
      }

    /* Hauptrichtung gefunden, jetzt noch Nebenrichtung testen */
    Freeman dirneu;

    Direction = Direction + 7;
    Direction.move(xs, ys, xx, yy);

    if (decision(func, parray, xx, yy, dx, dy) == 1)
      /* Nebenrichtung gltig */
      {
        dirneu = Direction + 7;  /* neue Hauptrichtung festlegen */
      }
    else
      {
        dirneu = Direction = Direction + 1;  /* alte Hauptrichtung wieder herstellen */
      }

    /* Startrichtung gefunden und merken */
    Freeman startdir = Direction;
    xx = xs;
    yy = ys;
    exist = 0;

    /* Beginn der Konturfolge */
    do
      {
        Direction.move(xx, yy);
        dirneu = dirneu + 6;
        PutVal(imgh, xx, yy, 255);

        /*
          Delay(0.1);
        */
        if (xx > 0 && xx < dx - 1 && yy > 0 && yy < dy - 1)
          {
            PutVal(img, xx, yy, val);
            exist = 1;
          }
        else
          {
            if (xx == 0)
              if (
                func(parray, xx, yy) >= 0 &&
                (func(parray, xx - 1, yy) < 0 ||
                 func(parray, xx + 1, yy) < 0 ||
                 func(parray, xx, yy + 1) < 0 ||
                 func(parray, xx, yy - 1) < 0)
              )
                {
                  PutVal(img, xx, yy, val);
                  exist = 1;
                }

            if (yy == 0)
              if (
                func(parray, xx, yy) >= 0 &&
                (func(parray, xx - 1, yy) < 0 ||
                 func(parray, xx + 1, yy) < 0 ||
                 func(parray, xx, yy + 1) < 0 ||
                 func(parray, xx, yy - 1) < 0)
              )
                {
                  PutVal(img, xx, yy, val);
                  exist = 1;
                }

            if (xx == dx - 1)
              if (
                func(parray, xx, yy) >= 0 &&
                (func(parray, xx - 1, yy) < 0 ||
                 func(parray, xx + 1, yy) < 0 ||
                 func(parray, xx, yy + 1) < 0 ||
                 func(parray, xx, yy - 1) < 0)
              )
                {
                  PutVal(img, xx, yy, val);
                  exist = 1;
                }

            if (yy == dy - 1)
              if (
                func(parray, xx, yy) >= 0 &&
                (func(parray, xx - 1, yy) < 0 ||
                 func(parray, xx + 1, yy) < 0 ||
                 func(parray, xx, yy + 1) < 0 ||
                 func(parray, xx, yy - 1) < 0)
              )
                {
                  PutVal(img, xx, yy, val);
                  exist = 1;
                }
          }

        /*
          printf("h1: xx %d yy %d Direction %d, dirneu %d\n",xx,yy,Direction,dirneu);
          printf("xx: %d yy: %d  f(x,y): %f\n",xx,yy,
          func(parray,xx,yy));
        */
        /*
          getch();
        */
        do
          {
            dirneu = dirneu + 2;
            dirneu.move(xx, yy, xxo, yyo);
            /*
              printf(">>>dec: %d xx0: %d yy0: %d\n",
              decision(func, parray, xxo, yyo, wmx1, wmy1, wmx2, wmy2),xxo,yyo);
              getch();
            */
          }
        while (decision(func, parray, xxo, yyo, dx, dy) == 0);

        /*
          printf("h2: xxo %d yyo %d Direction %d, dirneu %d\n", xxo, yyo, Direction, dirneu);
          getch();
        */
        /* vorhergehende Nebenrichtung testen */
        Direction = dirneu - 1;
        Direction.move(xx, yy, xxo, yyo);

        if (decision(func, parray, xxo, yyo, dx, dy) == 1)
          {
            /* Nebenrichtung gltig */
            dirneu = Direction - 1; /* neue zu untersuchende Hauptrichtung */
          }
        else
          {
            Direction = dirneu;  /* zurck zur Hauptrichtung */
          }

        /*
          printf("h3::: xx %d yy %d Direction %d, dirneu %d\n",xx,yy,Direction,dirneu);
          getch();
          printf("ende schleife\n");
        */
      }
    while (!(xx == xs && yy == ys && Direction == startdir));

    if (exist == 1)
      {
        return EXIST;
      }
    else
      {
        return NO_EXIST;
      }
  }
#undef FNAME
}

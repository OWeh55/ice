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
//
//  Histogramme
//  Abraham 04/93
//  Baumbach 10/98
//  Ortmann 11/99
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string>

#include "macro.h"
#include "defs.h"
#include "IceException.h"

#include "paint.h"
#include "drawline.h"

#include "hist.h"

namespace ice
{

#define FNAME "Hist::Hist"
  Hist::Hist() : klasse(nullptr)
  {
    reset();
  }

  Hist::Hist(const Hist& h) : klasse(nullptr)
  {
    int i;
    reset(h.nClasses, h.classwidth, h.lowerLimit); // initialization as in h
    min = h.min;
    max = h.max;
    sum = h.sum;

    for (i = 0; i < nClasses + 2; i++)
      {
        klasse[i] = h.klasse[i];
      }
  }

  Hist::Hist(int number, double diff, double minval):
    klasse(nullptr)
  {
    reset(number, diff, minval);
  }

  Hist::~Hist()
  {
    delete [] klasse;
  }
#undef FNAME
#define FNAME "Hist::reset"
  int Hist::reset()
  {
    delete [] klasse;
    isInit = false;
    return OK;
  }

  int Hist::reset(int number, double diff, double minval)
  {
    if ((number < 1) || (diff <= 0))
      throw IceException(FNAME, M_WRONG_PARAM);

    delete [] klasse;
    nClasses = number;
    klasse = new unsigned int[number + 2];

    for (int i = 0; i < nClasses + 2; i++)
      {
        klasse[i] = 0;
      }

    lowerLimit = minval;
    classwidth = diff;
    min = DBL_MAX;
    max = DBL_MIN;
    sum = 0;
    isInit = true;
    return OK;
  }
#undef FNAME
#define FNAME "Hist::add"
  int Hist::add(double val, int count)
  {
    int kl;

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    kl = (int)floor((val - lowerLimit) / classwidth) + 1;

    if (kl < 0)
      {
        kl = 0;
      }

    if (kl > nClasses)
      {
        kl = nClasses + 1;
      }

    klasse[kl] += count;

    if (val > max)
      {
        max = val;
      }

    if (val < min)
      {
        min = val;
      }

    sum += count;
    return OK;
  };
#undef FNAME

#define FNAME "Hist::operator="
  Hist& Hist::operator=(const Hist& h)
  {
    int i;

    if (this == &h)
      {
        return *this;
      }

    reset(h.nClasses, h.classwidth, h.lowerLimit); // reisInitialization as in h
    min = h.min;
    max = h.max;
    sum = h.sum;

    for (i = 0; i < nClasses + 2; i++)
      {
        klasse[i] = h.klasse[i];
      }

    return *this;
  }
#undef FNAME
#define FNAME "Hist::Count"
  int Hist::count(int index) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((index < 0) || (index > nClasses + 1))
      throw IceException(FNAME, M_WRONG_PARAM);

    return klasse[index];
  }
#undef FNAME
#define FNAME "Hist::Rel"
  double Hist::Rel(int index) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (index < 0 || index > nClasses + 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (sum == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    return (double)klasse[index] / (double)sum;
  }
#undef FNAME

#define FNAME "Hist::Limits"
  int Hist::Limits(double& minv, double& maxv) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    minv = min;
    maxv = max;
    return OK;
  }

  int Hist::Limits(double& minv, double& maxv, double q) const
  {
    int i;
    int ct;
    int qabs;

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((q < 0.0) || (q > 0.5))
      throw IceException(FNAME, M_WRONG_PARAM);

    if (sum == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    qabs = RoundInt(sum * q);

    if (q == 0.0)
      {
        minv = min;
        maxv = max;
        return OK;
      }

    ct = 0;
    i = 0; // Start bei erster "normaler" Klasse

    do
      {
        i++;
        ct += klasse[i];
      }
    while (ct <= qabs);

    minv = ClassValue(i);

    ct = 0;
    i = nClasses + 1; // Start bei letzter "normaler" Klasse

    do
      {
        i--;
        ct += klasse[i];
      }
    while (ct <= qabs);

    maxv = ClassValue(i); // Klassenmitte als Repr�entant

    return OK;
  }

#undef FNAME
#define FNAME "Hist::ClassValue"
  double Hist::ClassValue(int nr) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nr < 0 || nr > nClasses + 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    return (nr - 1 + 0.5) * classwidth + lowerLimit; // Klassenmitte als Repr�entant
  }

  double Hist::ClassValue(int nr, double& min, double& max) const
  {
    // hier nur "normale" Klassen
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nr < 1 || nr > nClasses)
      throw IceException(FNAME, M_WRONG_PARAM);

    min = (nr - 1) * classwidth + lowerLimit;
    max = min + classwidth;
    return ClassValue(nr);
  }
#undef FNAME

#define FNAME "Hist::Statistic"
  void Hist::getStatistics(int& n, double& xm, double& xs, double& skew) const
  {
    double sumx, sumx2, sumx3, aktw;
    int i;

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (sum == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    sumx = sumx2 = sumx3 = 0;
    n = sum;

    for (i = 1; i <= nClasses; i++)
      {
        aktw = ClassValue(i);
        sumx += klasse[i] * aktw;
        sumx2 += klasse[i] * aktw * aktw;
        sumx3 += klasse[i] * aktw * aktw * aktw;
      }

    xm = sumx / sum;
    xs = sqrt(sumx2 / sum - xm * xm);
    skew = sumx3 - 3 * xm * sumx2 + xm * xm * 3 * sumx - xm * xm * xm * sum;
  };

  void Hist::getStatistics(int& n, double& xm, double& xs) const
  {
    double dummy;
    getStatistics(n, xm, xs, dummy);
  };

  void Hist::getStatistics(int& n) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    n = sum;
  };
#undef FNAME

//
//    Visualization of histogramm
//    Steffen Abraham
//    25.09.92/13.03.93
//    Wolfgang Ortmann, 11/99, 04/17
//

#define _GETX(x) RoundInt(((double)(x))*(double)dx/100)
#define _GETY(y) RoundInt(((double)(y))*(double)dy/100)

#define FNAME "Hist::Vis"
  void Hist::draw(const Image& b, int grw) const
  {
    std::string fmt;
    unsigned int maxn;
    int i, txtsize;
    int rxa, ry, rx, rya;
    char zahlstr[80];

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (!IsImg(b))
      throw IceException(FNAME, M_WRONG_IMAGE);

    int dx = b.xsize - 1;
    int dy = b.ysize - 1;

    Line(_GETX(9), _GETY(90), _GETX(96), _GETY(90), grw, 0, b);
    Line(_GETX(10), _GETY(1), _GETX(10), _GETY(91), grw, 0, b);
    Line(_GETX(85), _GETY(88), _GETX(85), _GETY(92), grw, 0, b);
    Line(_GETX(8), _GETY(10), _GETX(12), _GETY(10), grw, 0, b);

    for (i = 0, maxn = 0; i < nClasses + 1; i++)
      if (klasse[i] > maxn)
        {
          maxn = klasse[i];
        }

    rxa = _GETX(9);
    rya = _GETY(7);
    ry = _GETY(1);
    rx = _GETX(11);

    for (; rxa <= rx; rxa++)
      {
        Line(_GETX(10), ry, rxa, rya, grw, 0, b);
      }

    rxa = _GETX(96);
    rya = _GETY(88);
    ry = _GETY(92);
    rx = _GETX(90);

    for (; rya <= ry; rya++)
      {
        Line(rx, rya, rxa, _GETY(90), grw, 0, b);
      }

    txtsize = dx / 600;

    if (dx >= 100 && txtsize <= 5)
      {
        sprintf(zahlstr, "%d", maxn);
        Text(zahlstr, _GETX(0), _GETY(8), grw, txtsize, b);

        if (classwidth * nClasses < 0.1)
          {
            fmt = "%9.3e";
          }
        else
          {
            if ((fabs(lowerLimit) > 1000) || (fabs(lowerLimit + classwidth * nClasses) > 1000))
              {
                fmt = "%9.3e";
              }
            else
              {
                fmt = "%7.3f";
              }
          }

        sprintf(zahlstr, fmt.c_str(), lowerLimit);
        Text(zahlstr, _GETX(4), _GETY(92), grw, txtsize, b);
        sprintf(zahlstr, fmt.c_str(), lowerLimit + classwidth * nClasses);
        Text(zahlstr, _GETX(75), _GETY(92), grw, txtsize, b);
      }

    for (i = 0, rxa = _GETX(10); i < nClasses + 1; i++)
      {
        rx = _GETX(i * 75.0 / nClasses + 10);
        ry = _GETY(90.0 - klasse[i] * 80.0 / maxn);

        for (; rxa <= rx; rxa++)
          {
            Line(rxa, _GETY(90), rxa, ry, grw, 0, b);
          }
      }

  }
#undef FNAME
//
//    Wolfgang Ortmann 11/99
//    Syntax: Hist HistImg(Image b,int diff);
//    Beschreibung: Berechnung eines Grauwerthistogrammes
//    uebergabeparameter:
//          b -Bild fuer Berechnung
//          diff - Pixelabstand der zur Berechnung verwendeten
//                 Punkte
//    Rueckgabe:  Histogramm
//         (bei Fehler: ungueltiges histogramm !valid )
//
#define FNAME "HistImg"
  Hist HistImg(const Image& b, int diff, int klbr)
  {
    int x, y;
    Hist h;

    if (!IsImg(b))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if ((diff > b->xsize) || (diff > (b->ysize)) || diff < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    h.reset((b.maxval + 1 + (klbr - 1)) / klbr, klbr, 0);

    for (y = 0; y < b->ysize; y += diff)
      for (x = 0; x < b->xsize; x += diff)
        {
          h.add(GetVal(b, x, y));
        }

    return h;
  }
#undef FNAME
}

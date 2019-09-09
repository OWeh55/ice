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
//  Ortmann 6/2004, 9/2019
//
#include <vector>

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
#include "vectordistance.h"
#include "histogram.h"

using namespace std;

namespace ice
{
#define FNAME "Histogram::Histogram"
  Histogram::Histogram(): isInit(false), nclasses(0), nValues(0)
  {
  }

  Histogram::Histogram(int number)
  {
    reset(number);
  }

  void Histogram::addImg0(const Image& b, int diff)
  {
    for (int y = 0; y < b->ysize; y += diff)
      for (int x = 0; x < b->xsize; x += diff)
        {
          addValueUnchecked(b.getPixel(x, y));
        }
  }

  template<class T>
  void Histogram::addImg(const Image& b, int diff)
  {
    T** bp = (T**)b->getDataPtr();

    for (int y = 0; y < b->ysize; y += diff)
      for (int x = 0; x < b->xsize; x += diff)
        {
          addValueUnchecked(bp[y][x]);
        }
  }

  Histogram::Histogram(const Image& b, int diff): isInit(false), nclasses(0), nValues(0)
  {
    if (!IsImg(b))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (diff < 1)
      throw IceException(FNAME, M_WRONG_PARAM);
    reset(b.maxval + 1);
    addImage(b, diff);
  }

  int Histogram::addImage(const Image& b, int diff)
  {
    if (!IsImg(b))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (diff < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    int typ = b->ImageType();

    switch (typ)
      {
      case 1:
        addImg<PixelType1>(b, diff);
        break;
      case 2:
        addImg<PixelType2>(b, diff);
        break;
      case 3:
        addImg<PixelType3>(b, diff);
        break;
      default:
        addImg0(b, diff);
        break;
      }
    return OK;
  }
#undef FNAME

  int Histogram::operator [](int n) const
  {
    if ((unsigned int)n >= (unsigned int)nclasses)
      {
        return 0;
      }
    return classes[n];
  }

#undef FNAME
#define FNAME "Histogram::reset"
  int Histogram::reset(int number)
  {
    int i;

    if (number < 1)
      throw IceException(FNAME, M_WRONG_PARAM);

    nclasses = number;

    classes.resize(nclasses);

    for (i = 0; i < nclasses; i++)
      {
        classes[i] = 0;
      }

    nValues = 0;
    isInit = true;
    return OK;
  }
#undef FNAME
#define FNAME "Histogram::Add"
  int Histogram::addValue(int val, int count)
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((unsigned int)val < (unsigned int)nclasses)
      {
        classes[val] += count;
        nValues += count;
      }
    return OK;
  };

  int Histogram::addValueUnchecked(int val, int count)
  {
    classes[val] += count;
    nValues += count;
    return OK;
  };
#undef FNAME

#define FNAME "Histogram::getCount"
  int Histogram::getCount(int index) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((index < 0) || (index >= nclasses))
      throw IceException(FNAME, M_WRONG_PARAM);

    return classes[index];
  }

  vector<int> Histogram::getCount() const
  {
    vector<int> v(nclasses);

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    for (int i = 0; i < nclasses; i++)
      {
        v[i] = classes[i];
      }

    return v;
  }

  void Histogram::getCount(vector<int>& v) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    v.resize(nclasses);

    for (int i = 0; i < nclasses; i++)
      {
        v[i] = classes[i];
      }
  }

#undef FNAME
#define FNAME "Histogram::Rel"
  double Histogram::getRelative(int index) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (index < 0 || index >= nclasses)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    return (double)classes[index] / (double)nValues;
  }

  vector<double> Histogram::getRelative() const
  {
    vector<double> res(nclasses);

    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    for (int i = 0; i < nclasses; i++)
      {
        res[i] = (double)classes[i] / (double)nValues;
      }

    return res;
  }

  void Histogram::getRelative(vector<double>& v) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    v.resize(nclasses);

    for (int i = 0; i < nclasses; i++)
      {
        v[i] = (double)classes[i] / (double)nValues;
      }

  }
#undef FNAME

#define FNAME "Histogram::Limits"
  int Histogram::getLimits(int& minv, int& maxv) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    int i = 0;

    while (classes[i] == 0)
      {
        i++;
      }

    minv = i;
    i = nclasses - 1;

    while (classes[i] == 0)
      {
        i--;
      }

    maxv = i;

    return OK;
  }

  int Histogram::getLimits(int& minv, int& maxv, double q) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if ((q < 0.0) || (q > 0.5))
      throw IceException(FNAME, M_WRONG_PARAM);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    int qabs = RoundInt(nValues * q); // number of values to ignore

    int ct = 0;

    for (minv = 0; ct <= qabs && minv < nclasses; minv++)
      {
        ct += classes[minv];
      }

    ct = 0;

    for (maxv = nclasses - 1; ct <= qabs && maxv > 0; maxv--)
      {
        ct += classes[maxv];
      }

    return OK;
  }

#undef FNAME

#define FNAME "Histogram::getStatistics"
  void Histogram::getStatistics(int& n, double& xm, double& xs, double& skew) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    double sumx = 0;
    double sumx2 = 0;
    double sumx3 = 0;

    for (int i = 0; i < nclasses; i++)
      {
        double aktw  = i;
        sumx  += classes[i] * aktw;
        sumx2 += classes[i] * aktw * aktw;
        sumx3 += classes[i] * aktw * aktw * aktw;
      }

    n = nValues;
    xm = sumx / nValues;
    xs = sqrt(sumx2 / nValues - xm * xm);
    skew = sumx3 - 3 * xm * sumx2 + xm * xm * 3 * sumx - xm * xm * xm * nValues;
  };

  void Histogram::getStatistics(int& n, double& xm, double& xs) const
  {
    double dummy;
    getStatistics(n, xm, xs, dummy);
  };

  void Histogram::getStatistics(int& n) const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    n = nValues;
  };
#undef FNAME

#define FNAME "Histogram::computeAverage"
  double Histogram::computeAverage() const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);
    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    double vSum = 0.0;
    for (int i = 0; i < nclasses; i++)
      vSum += i * classes[i];
    return vSum / nValues;
  }
#undef FNAME
#define FNAME "Histogram::computeMedian"
  int Histogram::computeMedian() const
  {
    if (!isInit)
      throw IceException(FNAME, M_NOT_INITIALIZED);
    if (nValues == 0)
      throw IceException(FNAME, M_HIST_EMPTY);

    int nV = classes[0];
    int i = 0;
    for (i = 1; i < nclasses && nV * 2 < nValues; i++)
      nV += classes[i];
    return i;
  }
#undef FNAME
  /*
  int Histogram::computeMinimum() const;
  int Histogram::computeMaximum() const;
  int Histogram::computeFrequent() const;
  */
#define FNAME "Distance"
  double Distance(const Histogram& h1, const Histogram& h2)
  {
    if (!h1.isValid() || !h2.isValid() || h1.nClasses() != h2.nClasses())
      throw IceException(FNAME, M_WRONG_PARAM);

    EarthMoverVectorDistance emd;
    return emd(h1.getRelative(), h2.getRelative());
  }
#undef FNAME
//
//    Visualization of histogram
//    Steffen Abraham
//    25.09.92/13.03.93
//    Wolfgang Ortmann, 11/99, 2004, 04/12, 04/17
//

#define _GETX(x) RoundInt(((double)(x))*(double)dx/100)
#define _GETY(y) RoundInt(((double)(y))*(double)dy/100)

#define FNAME "Histogram::draw"
  void Histogram::draw(const Image& b, int grw) const
  {
    std::string fmt;
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

    int maxn = 0;
    for (int i = 0; i < nclasses; i++)
      if (classes[i] > maxn)
        {
          maxn = classes[i];
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

    int txtsize = dx / 600;

    if (txtsize < 1)
      {
        txtsize = 1;
      }

    if (dx >= 100 && txtsize <= 5)
      {
        sprintf(zahlstr, "%d", maxn);
        Text(zahlstr, _GETX(0), _GETY(8), grw, -txtsize, b);
        Text("0", _GETX(4), _GETY(92), grw, -txtsize, b);
        sprintf(zahlstr, "%d", nclasses - 1);
        Text(zahlstr, _GETX(75), _GETY(92), grw, -txtsize, b);
      }

    for (int i = 0, rxa = _GETX(10); i < nclasses; i++)
      {
        rx = _GETX(i * 75.0 / nclasses + 10);
        ry = _GETY(90.0 - classes[i] * 80.0 / maxn);

        for (; rxa <= rx; rxa++)
          {
            Line(rxa, _GETY(90), rxa, ry, grw, 0, b);
          }
      }
  }
#undef FNAME
}

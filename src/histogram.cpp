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
//  Ortmann 6/2004
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
#include "message.h"
#include "paint.h"
#include "drawline.h"
#include "vectordistance.h"
#include "histogram.h"

using namespace std;

namespace ice
{
#define FNAME "Histogram::Histogram"
  Histogram::Histogram(): isInit(false), nclasses(0), sum(0)
  {
  }

  Histogram::Histogram(int number)
  {
    reset(number);
  }

  void Histogram::AddImg0(const Image& b, int diff)
  {
    for (int y = 0; y < b->ysize; y += diff)
      for (int x = 0; x < b->xsize; x += diff)
        addValueUnchecked(b.getPixel(x, y));
  }

  template<class T>
  void Histogram::AddImg(const Image& b, int diff)
  {
    T** bp = (T**)b->getDataPtr();

    for (int y = 0; y < b->ysize; y += diff)
      for (int x = 0; x < b->xsize; x += diff)
        addValueUnchecked(bp[y][x]);
  }

  Histogram::Histogram(const Image& b, int diff): isInit(false), nclasses(0), sum(0)
  {
    if (!IsImg(b))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return;
      }

    if (diff < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return;
      }
    reset(b->maxval + 1);
    addImage(b, diff);
  }

  int Histogram::addImage(const Image& b, int diff)
  {
    if (!IsImg(b))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return WRONG_POINTER;
      }

    if (diff < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    int typ = b->ImageType();

    switch (typ)
      {
      case 1:
        AddImg<PixelType1>(b, diff);
        break;
      case 2:
        AddImg<PixelType2>(b, diff);
        break;
      case 3:
        AddImg<PixelType3>(b, diff);
        break;
      default:
        AddImg0(b, diff);
        break;
      }
    return OK;
  }
#undef FNAME

  int Histogram::operator [](int n) const
  {
    if ((unsigned int)n >= (unsigned int)nclasses)
      return 0;
    return classes[n];
  }

#undef FNAME
#define FNAME "Histogram::reset"
  int Histogram::reset(int number)
  {
    int i;

    if (number < 1)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    nclasses = number;

    classes.resize(nclasses);

    for (i = 0; i < nclasses; i++)
      classes[i] = 0;

    sum = 0;
    isInit = true;
    return OK;
  }
#undef FNAME
#define FNAME "Histogram::Add"
  int Histogram::addValue(int val, int count)
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    if ((unsigned int)val < (unsigned int)nclasses)
      {
        classes[val] += count;
        sum += count;
      }
    return OK;
  };

  int Histogram::addValueUnchecked(int val, int count)
  {
    classes[val] += count;
    sum += count;
    return OK;
  };
#undef FNAME

#define FNAME "Histogram::getCount"
  int Histogram::getCount(int index) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return -1;
      }

    if ((index < 0) || (index >= nclasses))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return -1;
      }

    return classes[index];
  }

  vector<int> Histogram::getCount() const
  {
    vector<int> v(nclasses);

    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return v;
      }

    for (int i = 0; i < nclasses; i++)
      v[i] = classes[i];

    return v;
  }

  void Histogram::getCount(vector<int>& v) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return;
      }

    v.resize(nclasses);

    for (int i = 0; i < nclasses; i++)
      v[i] = classes[i];
  }

#undef FNAME
#define FNAME "Histogram::Rel"
  double Histogram::getRelative(int index) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return -1.0;
      }

    if (index < 0 || index >= nclasses)
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return -1.0;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return -1.0;
      }

    return (double)classes[index] / (double)sum;
  }

  vector<double> Histogram::getRelative() const
  {
    vector<double> res(nclasses);

    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return res;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return res;
      }

    for (int i = 0; i < nclasses; i++)
      res[i] = (double)classes[i] / (double)sum;

    return res;
  }

  void Histogram::getRelative(vector<double>& v) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return;
      }

    v.resize(nclasses);

    for (int i = 0; i < nclasses; i++)
      v[i] = (double)classes[i] / (double)sum;

  }
#undef FNAME

#define FNAME "Histogram::Limits"
  int Histogram::getLimits(int& minv, int& maxv) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return ERROR;
      }

    int i = 0;

    while (classes[i] == 0) i++;

    minv = i;
    i = nclasses - 1;

    while (classes[i] == 0) i--;

    maxv = i;

    return OK;
  }

  int Histogram::getLimits(int& minv, int& maxv, double q) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    if ((q < 0.0) || (q > 0.5))
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return ERROR;
      }

    int qabs = RoundInt(sum * q); // number of values to ignore

    int ct = 0;

    for (minv = 0; ct <= qabs && minv < nclasses; minv++)
      ct += classes[minv];

    ct = 0;

    for (maxv = nclasses - 1; ct <= qabs && maxv > 0; maxv--)
      ct += classes[maxv];

    return OK;
  }

#undef FNAME

#define FNAME "Histogram::Statistic"
  int Histogram::Statistic(int& n, double& xm, double& xs, double& skew) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    if (sum == 0)
      {
        Message(FNAME, M_HIST_EMPTY, ERROR);
        return ERROR;
      }

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

    n = sum;
    xm = sumx / sum;
    xs = sqrt(sumx2 / sum - xm * xm);
    skew = sumx3 - 3 * xm * sumx2 + xm * xm * 3 * sumx - xm * xm * xm * sum;
    return OK;
  };

  int Histogram::Statistic(int& n, double& xm, double& xs) const
  {
    double dummy;
    RETURN_ERROR_IF_FAILED(Statistic(n, xm, xs, dummy));
    return OK;
  };

  int Histogram::Statistic(int& n) const
  {
    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    n = sum;
    return OK;
  };
#undef FNAME
#define FNAME "Distance"
  double Distance(const Histogram& h1, const Histogram& h2)
  {
    if (!h1.isValid() || !h2.isValid() || h1.nClasses() != h2.nClasses())
      {
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return 0;
      }

    EarthMoverVectorDistance emd;
    return emd(h1.getRelative(), h2.getRelative());
  }
#undef FNAME
//
//    Visualization of histogramm
//    Steffen Abraham
//    25.09.92/13.03.93
//    Wolfgang Ortmann, 11/99,2004,04/12
//

#define _GETX(x) RoundInt(((double)(x))*(double)dx/100+(double)x1)
#define _GETY(y) RoundInt(((double)(y))*(double)dy/100+(double)y1)

#define FNAME "Vis"
  int Histogram::Vis(int grw, const Image& b) const
  {
    int dx, dy;
    int x1, y1, x2, y2;
    std::string fmt;
    int txtsize;
    int rxa, ry, rx, rya;
    char zahlstr[80];

    if (!isInit)
      {
        Message(FNAME, M_NOT_INITIALISED, ERROR);
        return ERROR;
      }

    if (!IsImg(b))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return WRONG_POINTER;
      }

    x1 = 0;
    y1 = 0;
    x2 = b->xsize - 1;
    y2 = b->ysize - 1;

    dx = x2 - x1;
    dy = y2 - y1;

    Line(_GETX(9), _GETY(90), _GETX(96), _GETY(90), grw, 0, b);
    Line(_GETX(10), _GETY(1), _GETX(10), _GETY(91), grw, 0, b);
    Line(_GETX(85), _GETY(88), _GETX(85), _GETY(92), grw, 0, b);
    Line(_GETX(8), _GETY(10), _GETX(12), _GETY(10), grw, 0, b);

    int maxn = 0;
    for (int i = 0; i < nclasses; i++)
      if (classes[i] > maxn)
        maxn = classes[i];

    rxa = _GETX(9);
    rya = _GETY(7);
    ry = _GETY(1);
    rx = _GETX(11);

    for (; rxa <= rx; rxa++)
      Line(_GETX(10), ry, rxa, rya, grw, 0, b);

    rxa = _GETX(96);
    rya = _GETY(88);
    ry = _GETY(92);
    rx = _GETX(90);

    for (; rya <= ry; rya++)
      Line(rx, rya, rxa, _GETY(90), grw, 0, b);

    txtsize = dx / 600;

    if (txtsize < 1)
      txtsize = 1;

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

        for (; rxa <= rx; rxa++) Line(rxa, _GETY(90), rxa, ry, grw, 0, b);
      }

    return OK;
  }
#undef FNAME
}

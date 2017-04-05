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
//  Konturen
//
//
//   Ortmann 10/99,05/12
//   m.schubert 9.93
//

#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>

#include <algorithm>

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "numbase.h"
#include "contools.h"
#include "contfeat.h"

#include "Contur.h"

namespace ice
{
#define FNAME "Contur::Contur"
  Contur::Contur()
  {
    Init();
  }

  Contur::Contur(IPoint p)
  {
    Init(p);
  }

  Contur::Contur(int x, int y)
  {
    Init(IPoint(x, y));
  }

  Contur::Contur(const Contur& c):
    isvalid(c.isvalid),
    nDirectionCodes(c.nDirectionCodes),
    length(c.length),

    start(c.start),
    endp(c.endp),

    ishole(c.ishole),
    isclosed(c.isclosed),

    xmin(c.xmin),
    xmax(c.xmax),
    ymin(c.ymin),
    ymax(c.ymax),

    curv(c.curv),
    data(c.data),
    cachedIndex(c.cachedIndex),
    cachedPoint(c.cachedPoint)
  {
  }
#undef FNAME

  Contur::~Contur()
  {
  }

  int Contur::SetStart(IPoint p)
  {
    if (nDirectionCodes == 0)
      {
        xmin = xmax = p.x;
        ymin = ymax = p.y;
        endp = start = p;
        isvalid = true;
        isclosed = true;
      }
    else
      {
        // Startpunkt einer vorhandenen Contur wird umgesetzt
        int dx = p.x - start.x;
        int dy = p.y - start.y;
        endp.x += dx;
        endp.y += dy;
        xmax += dx;
        xmin += dx;
        ymax += dy;
        ymin += dy;
        start = p;
      }

    cachedIndex = 0;
    cachedPoint = p;
    return OK;
  }

#define FNAME "Contur::Add"
  int DirDiff(Freeman d1, Freeman d2)
  {
    int diff = d1.Int() - d2.Int();
    if (diff > 4)
      {
        diff -= 8;
      }
    else if (diff <= -4)
      {
        diff += 8;
      }
    return diff;
  }

  int Contur::Add(Freeman dir)
  {
    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    data.push_back(dir);
    dir.move(endp);

    UpdateLimits(xmin, xmax, endp.x); /* umschr. Rechteck aktualisieren*/
    UpdateLimits(ymin, ymax, endp.y);

    // globale Kruemmung aktualisieren
    if (nDirectionCodes > 0)
      {
        curv += DirDiff(dir, data[nDirectionCodes - 1]);
      }
    else
      {
        curv = 0;
      }

    nDirectionCodes++;

    if ((dir.Int() & 1) > 0)
      {
        length += M_SQRT2;
      }
    else
      {
        length += 1.0;
      }

    isclosed = (start == endp);

    if (isclosed)
      {
        curv += DirDiff(data[0], dir);
        ishole = curv < 0;
      }

    cachedIndex = 0;
    cachedPoint = start;
    return OK;
  }

  int Contur::Add(IPoint p)
  {
    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    //    std::cout << "OK" << std::endl;

    int x1, y1, x2, y2;
    int xd, yd;
    int dir1, dir2;
    int i, count;
    int lng;
    int step1, step2;

    // Umbenennen der Werte -> line von x1,y1 nach x2,y2
    x1 = endp.x;
    y1 = endp.y;
    x2 = p.x;
    y2 = p.y;

    if ((x1 == x2) && (y1 == y2))
      {
        return OK;  // nothing to do
      }

    if (x1 < x2)
      {
        xd = x2 - x1;
        dir1 = 0;
      }
    else
      {
        xd = x1 - x2;
        dir1 = 4;
      }

    if (y1 < y2)
      {
        yd = y2 - y1;
        dir2 = 2;
      }
    else
      {
        yd = y1 - y2;
        dir2 = 6;
      }

    if (yd > xd)
      {
        std::swap(dir1, dir2);
        step1 = yd;
        step2 = xd;
      }
    else
      {
        step1 = xd;
        step2 = yd;
      }

    if (((dir1 - dir2) & 7) == 2)
      {
        dir2++;
      }
    else
      {
        dir2 = (dir2 - 1) & 7;
      }

    if (step2 == 0)                           /*achsenparallele Linie*/
      {
        for (i = 0; i < step1; i++)
          {
            Add(dir1);
          }
      }
    else
      {
        count = step1;
        lng = step1;
        step1 <<= 1;
        step2 <<= 1;

        if (dir1 < 3)
          for (i = 0; i < lng; i++)
            {
              count = count - step2;

              if (count <= 0)
                {
                  Add(dir2);
                  count += step1;
                }
              else
                {
                  Add(dir1);
                }
            }
        else

          for (i = 0; i < lng; i++)
            {
              count = count - step2;

              if (count < 0)
                {
                  Add(dir2);
                  count += step1;
                }
              else
                {
                  Add(dir1);
                }
            }
      }

    cachedIndex = 0;
    cachedPoint = p;
    return OK;
  }

  int Contur::Add(const Contur& c)
  {
    int i;

    if (!c.isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    RETURN_ERROR_IF_FAILED(Add(c.start));

    for (i = 0; i < c.nDirectionCodes; i++)
      {
        RETURN_ERROR_IF_FAILED(Add(c.data[i]));
      }

    cachedIndex = 0;
    cachedPoint = start;
    return OK;
  }
#undef FNAME
#define FNAME "operator+(Contur,Contur)"
  Contur operator+(const Contur& c1, const Contur& c2)
  {
    Contur res = c1;
    res.Add(c2);
    return res;
  }
#undef FNAME
#define FNAME "Contur::operator="
  Contur& Contur::operator=(const Contur& c)
  {
    if (&c == this)
      {
        return *this;
      }

    Reset();

    if (c.isvalid)
      {
        SetStart(c.start);
        Add(c);
      }
    else
      {
        isvalid = false;
      }

    return *this;
  }
#undef FNAME

  int Contur::Reset()
  {
    data.clear();
    nDirectionCodes = 0;
    Init();
    return OK;
  }

  int Contur::Reset(IPoint p)
  {
    data.clear();
    nDirectionCodes = 0;
    Init(p);
    return OK;
  }

  int Contur::Reset(int x, int y)
  {
    data.clear();
    nDirectionCodes = 0;
    Init(IPoint(x, y));
    return OK;
  }

  // [re-]initialize all data
  int Contur::Init(IPoint p)
  {
    Init();
    start = endp = p;
    xmin = xmax = p.x;
    ymin = ymax = p.y;
    cachedIndex = 0;
    cachedPoint = start;
    isvalid = true;
    isclosed = true;
    return OK;
  }

  int Contur::Init()
  {
    nDirectionCodes = 0;
    length = 0.0;

    isvalid = false;

    curv = 0;
    ishole = false;
    isclosed = false;

    data.clear();

    cachedIndex = INT_MAX; // no cached value
    return OK;
  }

#define FNAME "Contur::InvDir"
  int Contur::InvDir()
  {
    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    if (nDirectionCodes == 0)
      {
        return OK;  // keine Aktion nötig
      }

    for (int i = 0; i < nDirectionCodes / 2; i++)
      {
        // reihenfolge umkehren
        Freeman h = data[i];
        data[i] = data[nDirectionCodes - i - 1];
        data[nDirectionCodes - i - 1] = h;
      }

    for (int i = 0; i < nDirectionCodes; i++)   // richtungscodes invertieren
      {
        data[i] = data[i].Inverse();
      }

    std::swap(start, endp); // Start- und Endpunkt vertauschen

    curv = -curv;

    if (isclosed)
      {
        ishole = !ishole;
      }

    return OK;
  }
#undef FNAME
#define FNAME "Contur::getRect"
  int Contur::getRect(int& xi, int& yi, int& xa, int& ya) const
  {
    if (!isvalid)
      {
        return INVALID;
      }

    xi = xmin;
    xa = xmax;
    yi = ymin;
    ya = ymax;
    return OK;
  }

  int Contur::getRect(Window& win) const
  {
    if (!isvalid)
      {
        return INVALID;
      }

    win = Window(xmin, ymin, xmax, ymax);
    return OK;
  }
#undef FNAME

#define FNAME "Contur::getPoint"
  IPoint Contur::getPoint(int wantedIndex) const
  {
    IPoint res;

    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    int currentIndex = 0;
    res = start;

    if (wantedIndex == 0)
      {
        return res;
      }

    if (isclosed)
      {
        wantedIndex = wantedIndex % nDirectionCodes;  // bei geschlossener Kontur zyklisch arbeiten
      }
    else if (wantedIndex > nDirectionCodes)
      throw IceException(FNAME, M_WRONG_INDEX);

    if ((cachedIndex != INT_MAX) && (cachedIndex < wantedIndex))
      {
        currentIndex = cachedIndex;
        res = cachedPoint;
      }

    for (int i = currentIndex; i < wantedIndex; i++)
      {
        data[i].move(res);
      }

    cachedIndex = wantedIndex;
    cachedPoint = res;

    return res;
  }

  int Contur::getPoint(int wantedIndex, int& x, int& y) const
  {
    IPoint res;
    RETURN_ERROR_IF_FAILED(res = getPoint(wantedIndex));
    x = res.x;
    y = res.y;
    return OK;
  }
#undef FNAME

#define FNAME "Contur::getPairs"
  int Contur::getPairs(std::vector<IPoint>& pl, std::vector<Freeman>& fc) const
  {
    pl.clear();
    fc.clear();

    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    if (!isclosed)
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);

    if (data.empty())
      {
        // single point object
        for (int i = 0; i < 8; i += 2)
          {
            pl.push_back(start);
            fc.push_back(Freeman(i));
          }
      }
    else
      {
        Freeman lastc;
        Freeman currentc = data.back();
        IPoint currentp = start;

        for (unsigned int i = 0; i < data.size(); i++)
          {
            lastc = currentc;
            currentc = data[i];
            int first = (lastc.Int() + 6) & 6 ;
            int last = (currentc.Int() + 7) & 6;
            if (last < first)
              {
                last += 8;
              }
            for (int i = first; i <= last; i++)
              {
                pl.push_back(currentp);
                fc.push_back(Freeman(i));
              }
            currentc.move(currentp);
          }
      }
    return OK;
  }

  int Contur::getPairs(std::vector<IPoint>& opl, std::vector<IPoint>& upl) const
  {
    opl.clear();
    upl.clear();

    if (!isvalid)
      throw IceException(FNAME, M_NOT_INITIALISED);

    std::vector<Freeman> cd;

    RETURN_ERROR_IF_FAILED(getPairs(opl, cd));

    int npairs = opl.size();
    upl.resize(npairs);

    for (int i = 0; i < npairs; i++)
      {
        IPoint p2 = opl[i];
        cd[i].move(p2);
        upl[i] = p2;
      }
    return OK;
  }

#undef FNAME
}

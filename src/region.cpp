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
//   Ortmann 10/99, 05/13
//

#include "macro.h"
#include "IceException.h"
#include "conturfunctions.h"

#include "region.h"

using namespace std;

namespace ice
{
#define FNAME "Region::Region"
  Region::Region(const Contur& c)
  {
    int i;
    IMatrix segl;

    if (!c.isValid())
      throw IceException(FNAME, M_NOT_INITIALIZED);

    segl = ConturSegmentlist(c);

    for (i = 0; i < segl.rows(); i++)
      {
        add(segl[i][1], segl[i][0], segl[i][2], segl[i][0]);
      }
  }
#undef FNAME

  int Region::newY(int y)
  {
    //    std::cout << "newY: "<< y << "(" << sl.size() << ")" << endl;
    if (sl.empty())
      {
        // first row
        y0 = y;
      }

    while (y >= y0 + (int)sl.size())
      {
        // extend sl to higher y values
        sl.push_back(RangeSet());
      }

    while (y < y0)
      {
        // extend sl to lower y values
        sl.push_front(RangeSet());
        y0--;
      }

    return OK;
  }

  int Region::trimY()
  {
    if (sl.size() == 0)
      {
        return OK;
      }

    while ((sl.size() > 0) && (sl[0].empty()))
      {
        sl.pop_front();
        y0++;
      }

    while ((sl.size() > 0) && (sl.back().empty()))
      {
        sl.pop_back();
      }

    return OK;
  }

  int Region::getMinY() const
  {
    return y0;
  }

  int Region::getMaxY() const
  {
    return y0 + sl.size() - 1;
  }

  int Region::getMaxX() const
  {
    int xa = sl.begin()->getMax();
    for (deque<RangeSet>::const_iterator i = sl.begin();
         i != sl.end();
         i++)
      {
        int axa = i->getMax();
        if (xa < axa)
          {
            xa = axa;
          }
      }
    return xa;
  }

  int Region::getMinX() const
  {
    int xi = sl.begin()->getMin();
    for (deque<RangeSet>::const_iterator i = sl.begin();
         i != sl.end();
         i++)
      {
        int axi = i->getMin();
        if (xi < axi)
          {
            xi = axi;
          }
      }
    return xi;
  }

#define FNAME "Region::getRect"
  void Region::getRect(int& xi, int& yi, int& xa, int& ya) const
  {
    yi = y0;
    ya = y0 + sl.size() - 1;
    xa = getMaxX();
    xi = getMinX();
  }
#undef FNAME

#define FNAME "Region::getWidth"
  int Region::getWidth() const
  {
    return getMaxX() - getMinX() + 1;
  }
#undef FNAME

#define FNAME "Region::getHeight"
  int Region::getHeight() const
  {
    return sl.size();
  }
#undef FNAME

#define FNAME "Region::add"
  void Region::add(int x, int y)
  {
    newY(y);
    sl[y - y0].add(x);
  }

  void Region::add(const Region& r)
  {
    for (int y = 0; y < (int)r.sl.size(); y++)
      {
        newY(y + r.y0);
        sl[y + r.y0 - y0].add(r.sl[y]);
      }
  }

  void Region::add(int x1, int y1, int x2, int y2)
  {
    for (int y = y1; y <= y2; y++)
      {
        newY(y);
        sl[y - y0].add(x1, x2);
      }
  }

  void Region::add(const Window& w)
  {
    add(w.p1.x, w.p1.y, w.p2.x, w.p2.y);
  }

#undef FNAME
#define FNAME "Region::del"
  void Region::del(int x, int y)
  {
    int yh = y - y0;

    if (yh < 0)
      {
        return;
      }

    if (yh >= (int)sl.size())
      {
        return;
      }

    sl[yh].del(x);

    if (sl[yh].empty())
      {
        trimY();
      }
  }

  void Region::del(const Region& r)
  {
    for (int y = r.y0; y < r.y0 + (int)r.sl.size(); y++)
      {
        if (inside(y))
          {
            sl[y - y0].del(r.sl[y - r.y0]);
          }
      }

    trimY();
  }

  void Region::del(int x1, int y1, int x2, int y2)
  {
    for (int y = y1; y <= y2; y++)
      {
        int yh = y - y0;

        if ((yh >= 0) && (yh < (int)sl.size()))
          {
            sl[yh].del(x1, x2);
          }
      }

    trimY();
  }
#undef FNAME
#define FNAME "Region::intersect"
  void Region::intersect(const Region& r)
  {
    for (int y = 0; y < (int)sl.size(); y++)
      {
        if (r.inside(y + y0))
          {
            if (!sl[y].empty())
              {
                sl[y].intersect(r.sl[y + y0 - r.y0]);
              }
          }
        else
          {
            sl[y].clear();
          }
      }
    trimY();
  }
#undef FNAME
#define FNAME "Region::CalcMoments"
  int Region::calcMoments(Moments& m) const
  {
    for (int y = 0; y < (int)sl.size(); y++)
      {
        sl[y].calcMoments(m, y + y0);
      }

    return OK;
  }
#undef FNAME
#define FNAME "Region::Draw"
  int Region::draw(const Image& img, int val) const
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (val == -1)
      {
        return OK;
      }

    for (int y = 0; y < (int)sl.size(); y++)
      {
        sl[y].draw(y + y0, img, val);
      }

    return OK;
  }
#undef FNAME
  Point Region::getCenter()const
  {
    vector<IPoint> points;
    getPoints(points);
    Point sum(0, 0);
    for (int i = 0; i < (int)points.size(); i++)
      {
        sum += points[i];
      }
    return 1.0 / points.size() * sum;
  }

  void Region::getPoints(vector<IPoint>& points) const
  {
    for (int y = 0; y < (int)sl.size(); y++)
      {
        sl[y].getPoints(points, y + y0);
      }
  }

  void Region::getSegments(vector<RowSegment>& esl) const
  {
    for (int y = 0; y < (int)sl.size(); y++)
      {
        sl[y].getSegments(esl, y + y0);
      }
  }

  std::ostream& operator<<(std::ostream& out, const Region& s)
  {
    for (int i = 0; i < (int)s.sl.size(); i++)
      {
        out << i + s.y0 << ": " << s.sl[i];
      }

    out << endl;
    return out;
  }
}


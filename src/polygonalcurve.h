#ifndef POLYGONALCURVE_H
#define POLYGONALCURVE_H
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

#include <vector>
#include "geo_ob.h"
#include "Contur.h"
#include "lineseg.h"

namespace ice
{
  class PolygonalCurve: public GeoObject
  {
    // describes a polygonal curve
    // described by of a list of points

  public:
    virtual int size() const
    {
      return (int)pl.size();
    }

    virtual bool isClosed() const
    {
      return closed;
    }

    virtual double Length() const;

    PolygonalCurve(bool close = false): GeoObject(), pl(), closed(close) {}
    PolygonalCurve(Point p, bool close = false): GeoObject(p), pl(), closed(close)
    {
      pl.push_back(p);
    }

    explicit PolygonalCurve(const std::vector<Point>& ppl, bool close = false): pl(ppl), closed(close) {}
    explicit PolygonalCurve(const Matrix& im, bool close = false);
    explicit PolygonalCurve(const Contur& c, bool close = false);

    virtual void Reset()
    {
      Reset(Point(0, 0));
    }
    virtual void Reset(Point s)
    {
      // std::cout << "reset" << std::endl;
      p = s;
      pl.clear();
      pl.push_back(p);
    }

    virtual void setPos(Point pp);

    virtual void add(Point p)
    {
      pl.push_back(p);
    }

    virtual void add(double x, double y)
    {
      pl.push_back(Point(x, y));
    }

    virtual const Point& operator[](int i) const
    {
      return pl[i % pl.size()];
    }
    virtual Point& operator[](int i)
    {
      return pl[i % pl.size()];
    }

    virtual LineSeg Edge(int i) const
    {
      return LineSeg(pl[i], pl[(i + 1) % pl.size()]);
    };
    virtual void Edge(int i, LineSeg& l) const
    {
      l.setPos(pl[i]);
      l.setP2(pl[(i + 1) % pl.size()]);
    };

    using GeoObject::Distance;

    virtual double getArea() const
    {
      if (!closed)
        {
          return 0;
        }
      double area2 = 0.0;
      for (int i = 0; i < (int)pl.size(); i++)
        {
          int i1 = i + 1;
          if (i1 > (int)pl.size())
            {
              i1 = 0;
            }
          area2 += (pl[i].x - pl[i1].x) * (pl[i].y + pl[i1].y);
        }
      return fabs(area2 * 0.5);
    }

    virtual Contur makeContur() const;

    virtual int getClosestCorner(Point p) const;
    virtual int getClosestEdge(Point p) const;

    PolygonalCurve Reduced(int nr, int mode = 1) const;
    void Reduced(int nr, PolygonalCurve& p, int mode = 1) const;

    PolygonalCurve ReducedToPrecision(double prec, int mode = 1) const;
    void ReducedToPrecision(double prec, PolygonalCurve& p, int mode = 1) const;

    PolygonalCurve& operator=(const PolygonalCurve& sec)
    {
      pl = sec.pl;
      return *this;
    }

    const std::vector<Point>& PointList() const
    {
      return pl;
    }

  protected:

    virtual bool inside_(Point p) const;
    virtual double distance_(Point p) const;

    std::vector<Point> pl;
    const bool closed;

  };
}
#endif

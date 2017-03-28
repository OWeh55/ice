/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002-1014 FSU Jena, Digital Image Processing Group
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
#ifndef OMATCH_H
#define OMATCH_H

#include <vector>
#include <utility>

#include "geo.h"
#include "assignment.h"
#include "vectordistance.h"

namespace ice
{
#define MM_LINEAR 1
#define MM_SQUARE 2

  class ObjectMatcher
  {
  public:
    ObjectMatcher(int trafomode = TRM_AFFINE):
      trmode(trafomode), state(0), distance_measure(D_EUCLID)
    {
      reflection = HAS_REFLECTION(trmode);

      if (trmode == TRM_AFFINE_NOR)
        trmode = TRM_AFFINE;

      if (trmode == TRM_PROJECTIVE_NOR)
        trmode = TRM_PROJECTIVE;
    }
    virtual ~ObjectMatcher() {};

    virtual void setObject(int which, const Contur& c);
    virtual void setObject(int which, const IMatrix& m);
    virtual void setObject(int which, const Matrix& m);
    virtual void setObject(int which, const std::vector<Point>& m);
    virtual void setObject(int which, const std::vector<IPoint>& m);

    virtual void interpolObject(int which, double distance = 1.0);

    virtual int getSize(int obj) const
    {
      return obj == 1 ? pl2.size() : pl1.size();
    }
    virtual Point getPoint(int obj, int pointnr) const
    {
      return obj == 1 ? pl2[pointnr] : pl1[pointnr];
    }

    virtual void setDistanceMeasure(int dm)
    {
      distance_measure = dm;
    };

    virtual void calcReferences() const;
    virtual void calcDistances(matrix<double>& dm) const;
    virtual void getReferences(std::vector<int>& i1, std::vector<int>& i2) const;
    virtual void getOrderedLists(std::vector<Point>& pl1, std::vector<Point>& pl2) const;
    virtual Trafo getTrafo(int optmode = MM_SQUARE) const;

    virtual double getEvaluation() const
    {
      return eval;
    }

  private:
    virtual void calcReferences(const std::vector<Point>& p1,
                                const std::vector<Point>& p2) const;
    void TimeWarping(const matrix<double>& hu1,
                     const matrix<double>& hu2,
                     bool both_directions) const;

    int trmode;
    mutable int state;
    bool reflection;
    int distance_measure;

    static const int st_first = 1;
    static const int st_second = 2;
    static const int st_references = 4;

    std::vector<Point> pl1;
    std::vector<Point> pl2;
    mutable matrix<int> references;
    mutable double eval;
  };

  template<typename Tobj1, typename Tobj2>
  Trafo MatchObject(const Tobj1& obj1, const Tobj2& obj2,
                    int tmode, double& eval,
                    int mmode = MM_SQUARE, int anz = 0)
  {
    ObjectMatcher om(tmode);
    om.setObject(0, obj1);
    om.setObject(1, obj2);
    Trafo res = om.getTrafo(mmode);
    eval = om.getEvaluation();
    return res;
  }

  template<typename Tobj1, typename Tobj2>
  Trafo MatchObject(const Tobj1& obj1, const Tobj2& obj2,
                    int tmode = TRM_AFFINE)
  {
    ObjectMatcher om(tmode);
    om.setObject(0, obj1);
    om.setObject(1, obj2);
    return om.getTrafo();
  }

  int ObjectPointFeatures(const std::vector<Point>& pl,
                          matrix<double>& feat,
                          int tmode = TRM_AFFINE);

}
#endif

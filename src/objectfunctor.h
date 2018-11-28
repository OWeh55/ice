/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef OBJECTFUNCTOR_H
#define OBJECTFUNCTOR_H

#include "geo_ob.h"
#include "objectfunction.h"

namespace ice
{

  class ObjectFunctor
  {
  public:
    virtual object_rc operator()(IPoint p) const = 0;
    virtual ~ObjectFunctor() {}
  };

  class SingleImageObjectFunctor: public ObjectFunctor
  {
  private:
    Image img;
    OFunc object;
    int threshold;
  public:
    SingleImageObjectFunctor(const Image& timg, OFunc obj = NULL, int thr = 1): img(timg), object(obj), threshold(thr)
    {
      if (object == NULL)
        {
          object = ObjectThr;
        }
    }
    SingleImageObjectFunctor(const Image& timg, int thr = 1): img(timg), object(ObjectThr), threshold(thr)
    {
    }
    virtual object_rc operator()(IPoint p) const override
    {
      if (!img.inside(p))
        {
          return isunknown;
        }

      return object(img, p.x, p.y, threshold);
    }
  };

  class MarkedImageObjectFunctor: public ObjectFunctor
  {
  private:
    Image img;
    Image mrk;
    OFunc object;
    int threshold;
  public:
    MarkedImageObjectFunctor(const Image& timg, const Image& tmrk, OFunc obj = NULL, int thr = 1): img(timg), mrk(tmrk), object(obj), threshold(thr)
    {
      if (object == NULL)
        {
          object = ObjectThr;
        }
    }
    MarkedImageObjectFunctor(const Image& timg, const Image& tmrk, int thr = 1): img(timg), mrk(tmrk), object(ObjectThr), threshold(thr)
    {
    }
    virtual object_rc operator()(IPoint p) const override
    {
      if (!img.inside(p))
        {
          return isunknown;
        }

      if (GetValUnchecked(mrk, p) != 0)
        {
          return isunknown;
        }

      return object(img, p.x, p.y, threshold);
    }
  };

  class GeoObjectObjectFunctor: public ObjectFunctor
  {
  private:
    const GeoObject& ob;
  public:
    GeoObjectObjectFunctor(const GeoObject& obj): ob(obj) {}
    virtual object_rc operator()(IPoint p) const override
    {
      return ob.Inside(p) ? isobject : isunderground;
    }
  };


}
#endif

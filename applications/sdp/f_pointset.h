#ifndef IM_POINT_SET_H
#define IM_POINT_SET_H

#include "filterfactory.h"
#include "f_filter.h"
#include "f_ifilter.h"

class Image2PointSet: public Filter
{
public:
  Image2PointSet(): Filter("x2pointset", 2, 1,
                             "create list of points - "
                             "<image> <threshold*> => <pointlist>") {}
  FFUNC(Image2PointSet)

protected:
  virtual void get_data();
};

FF(Image2PointSet)

class PointSet2Image: public ImageFilter
{
public:
  PointSet2Image(): ImageFilter("pointset2image", 4, 5,
                                  "create image from pointset"
                                  "<pointlist> <xsize*> <ysize*> <maxval*> => <image+>") {}
  FFUNC(PointSet2Image)

protected:
  virtual void get_data();
};
FF(PointSet2Image)
#endif

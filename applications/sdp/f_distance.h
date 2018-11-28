#ifndef SDP_DISTANCE_TRAFO_H
#define SDP_DISTANCE_TRAFO_H

#include "f_ifilter.h"

class DistanceTrafo: public ImageFilter
{
public:
  DistanceTrafo(): ImageFilter("distancetransform", 4, 5,
                                 "distance transform for binary source images - <image> <mode*> <threshold*> <maxval*> => <image+>") {};
  FFUNC(DistanceTrafo);
protected:
  virtual void get_data()
  {
    const Image& src = getInput<Image, GImage, DType::image>(0);
    calcSize(src);
    vmax = getInputInt(3, vmax);
    int mode = getInputInt(1, 1);
    int threshold = getInputInt(2, 1);
    GImage* tresult = createResult();
    DistanceTransform(src, (*tresult)[0], mode, 0.0, threshold);
  }
};

FF(DistanceTrafo);
#endif

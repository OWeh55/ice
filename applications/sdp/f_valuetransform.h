#ifndef IM_TRANSFORM_H
#define IM_TRANSFORM_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class TransformImage: public FilterAllChannelsOneInput
{
public:
  TransformImage():
    FilterAllChannelsOneInput("grayvaluetransform", 4, 5,
                              "linear transform of pixel values - "
                              "<image> <a1> <a0> => <image>") {}
  FFUNC(TransformImage);

protected:

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    vmax = getInputInt(3, vmax);
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image &img1, const Image &img2)
  {
    double a1 = getInputFloat(1, 1.0);
    double a0 = getInputFloat(2, 0.0);
    GrayTransform(img1, img2, a1, a0);
  }
};

FF(TransformImage)
#endif

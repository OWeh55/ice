#ifndef IM_BINARY_H
#define IM_BINARY_H

#include "f_filterallchannels1.h"

class BinaryImage: public FilterAllChannelsOneInput
{
public:
  BinaryImage(): FilterAllChannelsOneInput("binary", 2, 5, "binarize image with threshold - "
        "<image> <threshold> => <image>")  {}

  FFUNC(BinaryImage)

protected:
  virtual void filterOneChannel(const Image& img1, const Image& img2)
  {
    int threshold = getInputInt(1, 1);
    binImg(img1, threshold, img2);
  }
};

FF(BinaryImage)

#endif

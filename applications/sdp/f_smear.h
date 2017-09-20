#ifndef IM_SMEAR_H
#define IM_SMEAR_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class SmearImage: public FilterAllChannelsOneInput
{
public:
  SmearImage(): FilterAllChannelsOneInput("smear", 3, 5, "apply a box average filter - "
                                            "<image> <sizex> <sizey> => <image+>") {}
  FFUNC(SmearImage)

protected:
  void filterOneChannel(const Image &img1, const Image &img2)
  {
    int xs = getInputInt(1, 3) | 1;
    int ys = getInputInt(2, xs) | 1;
    smearImg(img1, img2, xs, ys);
  }
};

FF(SmearImage);

class GaussImage: public FilterAllChannelsOneInput
{
public:
  GaussImage(): FilterAllChannelsOneInput("Gauss", 2, 5, "apply a Gauss filter - "
                                            "<image> <sigma> => <image+>") {}
  FFUNC(GaussImage)

protected:
  void filterOneChannel(const Image &img1, const Image &img2)
  {
    int sigma = getInputInt(1, 3);
    int size = 3 * sigma | 1;
    GaussImg(img1, img2, size, sigma);
  }
};

FF(GaussImage);
#endif

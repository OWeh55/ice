#ifndef IM_DIRECTION_H
#define IM_DIRECTION_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class DirectionImage: public FilterAllChannelsOneInput
{
public:
  DirectionImage(): FilterAllChannelsOneInput("direction", 3, 5,
        "calculate direction from gradient - "
        "<image> <size*> <maxval*> => <image+>") {}
  FFUNC(DirectionImage)
protected:

  virtual GImage* createResult(int idx = 0, bool withParameters = true)
  {
    vmax = getInputInt(2, vmax);
    if (vmax < 1)
      throw SdpException(name, "wrong maximum value");
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }

  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int size = getInputInt(1, 7);
    ice::CalcDirectionImg(img1, img2, size);
  }
};

FF(DirectionImage)

class DirectionStructImage: public FilterAllChannelsOneInput
{
public:
  DirectionStructImage(): FilterAllChannelsOneInput("directionstruct", 3, 5,
        "calculate direction from structure tensor - "
        "<image> <size*> <maxval*> => <image+>") {}
  FFUNC(DirectionStructImage)

protected:
  virtual GImage* createResult(int idx = 0, bool withParameters = true)
  {
    vmax = getInputInt(2, vmax);
    if (vmax < 1)
      throw SdpException(name, "wrong maximum value");
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }

  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int size = getInputInt(1, 7);
    ice::CalcDirectionStructImg(img1, img2, size);
  }
};
FF(DirectionStructImage)
#endif

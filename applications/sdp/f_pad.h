#ifndef IM_PAD_H
#define IM_PAD_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class Pad: public FilterAllChannelsOneInput
{
public:
  Pad(): FilterAllChannelsOneInput("pad", 6, 5, "pad image to image with given size - "
                                     "<image> <xsize> <ysize> <xpos> <ypos> => <bigimage>") {}

  FFUNC(Pad)

protected:
  //  int xsnew, ysnew, xp, yp;
  //  int paddingValue;

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    int xs = getInputInt(1);
    int ys = getInputInt(2);
    xp = getInputInt(3, (xs - xsize) / 2);
    yp = getInputInt(4, (ys - ysize) / 2);
    if (xs < xsize || ys < ysize)
      throw SdpException(name, "wrong sizes");
    if (xp < 0 || yp < 0 || yp + ysize > ys || xp + xsize > xs)
      throw SdpException(name, "wrong offsets");
    xsize = xs;
    ysize = ys;
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image &img1, const Image &img2)
  {
    int paddingValue = getInputInt(5, 0);
    img2.set(paddingValue);

    for (int y = 0; y < img1.ysize; ++y)
      for (int x = 0; x < img1.xsize; ++x)
        img2.setPixel(x + xp, y + yp, img1.getPixel(x, y));
  }

  int xp, yp;
};

FF(Pad)
#endif

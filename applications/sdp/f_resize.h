#ifndef IM_RESIZE_H
#define IM_RESIZE_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class Resize: public FilterAllChannelsOneInput
{
public:
  Resize(): FilterAllChannelsOneInput("resize", 3, 5, "resize image - "
                                        "<image> <xsize> <ysize> <maxval> => <image>") { }

  FFUNC(Resize)

protected:

  virtual void filterOneChannel(const Image &img1, const Image &img2)
  {
    renormImg(img1, img2);
  }

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    int xs = getInputInt(1);
    int ys = getInputInt(2, 0);
    if (ys == 0)
      {
        ys = ysize * xs / xsize;
      }
    if (xs <= 0 || ys <= 0)
      throw SdpException(name, "wrong sizes");

    xsize = xs;
    ysize = ys;
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }
};

FF(Resize)
#endif

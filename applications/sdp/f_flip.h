#ifndef IM_FLIP_H
#define IM_FLIP_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class FlipImage: public FilterAllChannelsOneInput
{
public:
  FlipImage(): FilterAllChannelsOneInput("flip", 2, 5, "mirror image at x axis (mode=1) or y axis (mode=2) or both (mode=3) - "
                                           "<image> <mode> => <image>") {}
  FFUNC(FlipImage);

protected:
  void filterOneChannel(const Image &img1, const Image &img2)
  {
    int mode = getInputInt(1, 1);
    switch (mode)
      {
      case 1:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            img2.setPixel(x, y, img1.getPixel(xsize - 1 - x, y));
        break;
      case 2:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            img2.setPixel(x, y, img1.getPixel(x, ysize - 1 - y));
        break;
      case 3:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            img2.setPixel(x, y, img1.getPixel(xsize - 1 - x, ysize - 1 - y));
        break;
      default:
        throw SdpException(name, "wrong mode");
      }
  }
};
FF(FlipImage)
#endif

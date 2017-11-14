#ifndef IM_SUB_H
#define IM_SUB_H

#include "filterfactory.h"
#include "f_filterallchannels2.h"

class SubImage: public FilterAllChannelsTwoInputs
{
public:
  SubImage(): FilterAllChannelsTwoInputs("subtract", 3, 5, "subtract images pixelwise - "
                                           "<image1> <image2> => <image+>") {}
  FFUNC(SubImage)

protected:
  virtual void filterOneChannel(const Image &img1, const Image &img2, const Image &img3)
  {
    int mode = getInputInt(2, 2);
    for (int y = 0; y < img3.ysize; ++y)
      for (int x = 0; x < img3.xsize; ++x)
        {
          int val = img1.getPixel(x, y) - img2.getPixel(x, y);
          if (mode == 1)
            val = abs(val);
          else if (mode == 2)
            val = val + img3.maxval / 2;
          img3.setPixelLimited(x, y, val);
        }
  }
};
FF(SubImage)
#endif

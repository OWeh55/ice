#ifndef IM_STACKED_H
#define IM_STACKED_H

#include "filterfactory.h"
#include "f_filterallchannels2.h"

class Stack: public FilterAllChannelsTwoInputs
{
public:
  Stack(): FilterAllChannelsTwoInputs("stack", 2, 5,
                                        "arrange one image above the other - <top> <bottom> => <stackimage+>",
                                        mm_value | mm_channels) {}
  FFUNC(Stack)

protected:
  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    ysize = inp1->ySize() + inp2->ySize();
    xsize = ice::max(inp1->xSize(), inp2->xSize());
    return FilterAllChannelsTwoInputs::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image &img1, const Image &img2, const Image &img3)
  {

    img3.set(0);
    for (int y = 0; y < img1.ysize; ++y)
      for (int x = 0; x < img1.xsize; ++x)
        img3.setPixel(x, y, img1.getPixel(x, y));
    for (int y = 0; y < img2.ysize; ++y)
      for (int x = 0; x < img2.xsize; ++x)
        img3.setPixel(x, y + img1.ysize, img2.getPixel(x, y));
  }
};

FF(Stack)
#endif

#ifndef IM_SIDE_BY_SIDE_H
#define IM_SIDE_BY_SIDE_H

#include "f_filterallchannels2.h"

class SideBySide: public FilterAllChannelsTwoInputs
{
public:
  SideBySide(): FilterAllChannelsTwoInputs("sidebyside", 2, 5,
        "arrange 2 images sid by side - "
        "<left> <right> => <combinedimage+>",
        mm_value | mm_channels) {}
  FFUNC(SideBySide)
protected:

  virtual GImage* createResult(int idx = 0, bool withParameters = true)
  {
    xsize = inp1->xSize() + inp2->xSize();
    ysize = ice::max(inp1->ySize(), inp2->ySize());
    return FilterAllChannelsTwoInputs::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image& img1, const Image& img2, const Image& img3)
  {
    img3.set(0);
    for (int y = 0; y < img1.ysize; ++y)
      for (int x = 0; x < img1.xsize; ++x)
        img3.setPixel(x, y, img1.getPixel(x, y));
    for (int y = 0; y < img2.ysize; ++y)
      for (int x = 0; x < img2.xsize; ++x)
        img3.setPixel(x + img1.xsize, y, img2.getPixel(x, y));
  }
};

FF(SideBySide)

#endif

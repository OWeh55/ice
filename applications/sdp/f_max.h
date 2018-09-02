#ifndef IM_MAX_MIN_H
#define IM_MAX_MIN_H

#include "filterfactory.h"
#include "f_filterallchannels2.h"

class Maximum: public FilterAllChannelsTwoInputs
{
public:
  Maximum(): FilterAllChannelsTwoInputs("maximum", 2, 5, "pixelwise find maximum - "
                                          "<img1> <img2> => <maximg>") {}

  FFUNC(Maximum)

protected:

  virtual void filterOneChannel(const Image& img1, const Image& img2, const Image& img3)
  {
    for (int y = 0; y < img3.ysize; ++y)
      for (int x = 0; x < img3.xsize; ++x)
        img3.setPixel(x, y, std::max(img1.getPixel(x, y), img2.getPixel(x, y)));
  }
};

FF(Maximum)

class Minimum: public FilterAllChannelsTwoInputs
{
public:
  Minimum(): FilterAllChannelsTwoInputs("minimum", 2, 5, "pixelwise find minimum - "
                                          "<img1> <img2> => <minimg>") {}
  FFUNC(Minimum)
protected:
  virtual void filterOneChannel(const Image& img1, const Image& img2, const Image& img3)
  {
    for (int y = 0; y < img3.ysize; ++y)
      for (int x = 0; x < img3.xsize; ++x)
        img3.setPixel(x, y, std::min(img1.getPixel(x, y), img2.getPixel(x, y)));
  }
};

FF(Minimum)
#endif



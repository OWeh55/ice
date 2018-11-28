#ifndef IM_ADD_H
#define IM_ADD_H

#include "filterfactory.h"
#include "f_filterallchannels2.h"

class AddImage: public FilterAllChannelsTwoInputs
{
public:
  AddImage(): FilterAllChannelsTwoInputs("add", 2, 5, "add two images pixelwise - "
                                           "<image1> <image2> => <image+>") {}
  FFUNC(AddImage)
protected:

  virtual void filterOneChannel(const Image& img1, const Image& img2, const Image& img3)
  {
    for (int y = 0; y < img3.ysize; ++y)
      for (int x = 0; x < img3.xsize; ++x)
        img3.setPixel(x, y, (img1.getPixel(x, y) + img2.getPixel(x, y)) / 2);
  }
};
FF(AddImage)
#endif

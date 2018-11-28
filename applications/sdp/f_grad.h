#ifndef IM_GRAD_H
#define IM_GRAD_H

#include "filterfactory.h"
#include "f_filterallchannels.h"

class GradImage: public FilterAllChannelsOneInput
{
public:
  GradImage(): FilterAllChannelsOneInput("gradient", 2, 5, "calc gradient of image - "
                                           "<image> <factor> => <image>") {}

  FFUNC(GradImage)
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int norm = getInputInt(1, 1);
    ice::GradImg(img1, img2, norm);
  }
};

FF(GradImage)

class GradDirImage: public FilterAllChannelsOneInput
{
public:
  GradDirImage(): FilterAllChannelsOneInput("gradientdirection", 2, 5,
        "calculate direction of gradient - "
        "<image> <factor*> => <image+>") {}

  FFUNC(GradDirImage)

protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int factor = getInputInt(1, 1);
    ice::GradDirImg(img1, img2);
    if (factor != 1)
      for (int y = 0; y < img2.ysize; ++y)
        for (int x = 0; x < img2.xsize; ++x)
          img2.setPixelLimited(x, y, factor * img2.getPixel(x, y));
  }
};

FF(GradDirImage)
#endif

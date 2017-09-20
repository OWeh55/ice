#ifndef IM_CLIP_H
#define IM_CLIP_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class Clip: public FilterAllChannelsOneInput
{
public:
  Clip():
    FilterAllChannelsOneInput("clip", 5, 5, "clip window to image - "
                              "<image> <xmin> <ymin> <xmax> <ymax> => <clipped image>")
  {
  }
  FFUNC(Clip)

protected:

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    int x1 = getInputInt(1);
    int y1 = getInputInt(2);
    int x2 = getInputInt(3);
    int y2 = getInputInt(4);
    if (x1 < 0 || x1 > x2 || x2 >= xsize)
      throw SdpException(name, "wrong x coordinates");
    if (y1 < 0 || y1 > y2 || y2 >= ysize)
      throw SdpException(name, "wrong y coordinates");
    xsize = x2 - x1 + 1;
    ysize = y2 - y1 + 1;
    return FilterAllChannelsOneInput::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image &img1, const Image &img2)
  {
    int x1 = getInputInt(1);
    int y1 = getInputInt(2);
    for (int y = 0; y < img2.ysize; ++y)
      for (int x = 0; x < img2.xsize; ++x)
        {
          img2.setPixel(x, y, img1.getPixel(x + x1, y + y1));
        }
  }
};

FF(Clip)
#endif

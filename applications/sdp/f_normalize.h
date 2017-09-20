#ifndef IM_NORMALIZE_H
#define IM_NORMALIZE_H

#include "filterfactory.h"
#include "f_filterallchannels.h"

class NormalizeImage: public FilterAllChannels
{
public:
  NormalizeImage():
    FilterAllChannels("normalize", 4, 5,
                      "normalize gray values based on min and max "
                      "given for each pixel - "
                      "<image> <minimage> <maximage> <maxval+> => <image+>") { }

  FFUNC(NormalizeImage)

protected:

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    vmax = getInputInt(3, vmax);
    return FilterAllChannels::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const vector<Image> &imgsrc,
                                const Image &imgd)
  {
    if (imgsrc.size() != 3)
      throw SdpException(name, "expected 3 images");

    Image img1 = imgsrc[0];
    Image img2 = imgsrc[1];
    Image img3 = imgsrc[2];

    for (int y = 0; y < ysize; ++y)
      for (int x = 0; x < xsize; ++x)
        {
          int val1 = img1.getPixel(x, y);
          int val_low = img2.getPixel(x, y);
          int val_high = img3.getPixel(x, y);
          double val_dif = val_high - val_low;
          if (val_dif == 0.0)
            val_dif = 1.0;

          int vald = RoundInt(vmax * (val1 - val_low) / val_dif);
          imgd.setPixelLimited(x, y, vald);
        }
  }
};

FF(NormalizeImage)

#endif

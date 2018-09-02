#ifndef IM_COMPARE_H
#define IM_COMPARE_H

#include "filterfactory.h"
#include "f_filterallchannels2.h"

class Compare: public FilterAllChannelsTwoInputs
{
public:
  Compare(): FilterAllChannelsTwoInputs("compare", 4, 5,
                                          "compare two images pixelwise - <img1> <img2> <mode> => <result>",
                                          mm_strict) {}

  FFUNC(Compare)

protected:

  virtual GImage* createResult(int idx = 0, bool withParameters = true)
  {
    vmax = getInputInt(3, vmax);
    return FilterAllChannelsTwoInputs::createResult(idx, withParameters);
  }

  virtual void filterOneChannel(const Image& img1, const Image& img2, const Image& img3)
  {
    int mode = getInputInt(2, 0);

    int fac = vmax / 2;
    for (int y = 0; y < ysize; ++y)
      for (int x = 0; x < xsize; ++x)
        {
          int val1 = img1.getPixel(x, y);
          int val2 = img2.getPixel(x, y);
          int val3;
          int cresult = val1 < val2 ? -1 : val1 > val2 ? 1 : 0;
          switch (mode)
            {
            case -1:
              val3 = cresult == -1 ? 2 : 0;
              break;
            case 0:
              val3 = cresult == 0 ? 2 : 0;
              break;
            case 1:
              val3 = cresult == 1 ? 2 : 0;
              break;
            default:
              val3 = cresult + 1;
              break;
            }
          img3.setPixel(x, y, val3 * fac);
        }
  }
};

FF(Compare)
#endif

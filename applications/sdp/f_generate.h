#ifndef IM_GENERATE_H
#define IM_GENERATE_H

#include "filterfactory.h"
#include "f_ifilter.h"

class Generator: public ImageFilter
{
public:
  Generator(): ImageFilter("generate", 4, 5, "generate image - "
                             "<mode> <xsize> <ysize> <maxval> => <image>") {}
  FFUNC(Generator)

protected:

  virtual void get_data()
  {
    int mode = getInputInt(0, 0);
    xsize = getInputInt(1, 777);
    ysize = getInputInt(2, 555);
    vmax = getInputInt(3, 255);
    nImg = 1;

    GImage* tresult = createResult();

    Image& gimg = (*tresult)[0];

    switch (mode)
      {
      case 0:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, 0);
        break;

      case 1:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, x % (vmax + 1));
        break;

      case 2:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (x + y) % (vmax + 1));
        break;

      case 3:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, y % (vmax + 1));
        break;

      case 4:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (xsize - x + y) % (vmax + 1));
        break;

      case 5:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (xsize - x) % (vmax + 1));
        break;

      case 6:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (xsize - x + ysize - y) % (vmax + 1));
        break;

      case 7:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (ysize - y) % (vmax + 1));
        break;

      case 8:
        for (int y = 0; y < ysize; ++y)
          for (int x = 0; x < xsize; ++x)
            gimg.setPixel(x, y, (x + ysize - y) % (vmax + 1));
        break;

      }
  }
};

FF(Generator)
#endif

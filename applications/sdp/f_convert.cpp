#include "genimage.h"
#include "gnumber.h"

#include "f_convert.h"

void X2Color::get_data()
{
  const GImage* img = getInputImage(0);
  if (img->getSize() == 3)
    {
      result[0] = cloneGImage(img);
    }
  else
    {
      result[0] = mkGImage(img, 3);
      const Image& src = (*img)[0];

      const int fac_red = getInputInt(1, src->maxval);
      const int fac_green = getInputInt(2, fac_red);
      const int fac_blue = getInputInt(3, fac_red);

      const GImage* dst = dynamic_cast<const GImage*>(result[0]);
      ColorImage cimg((*dst)[0], (*dst)[1], (*dst)[2]);

      for (int y = 0; y < src.ysize; ++y)
        for (int x = 0; x < src.xsize; ++x)
          {
            int g = src.getIntensity(x, y);
            ColorValue cv(g * fac_red / src->maxval,
                          g * fac_green / src->maxval,
                          g * fac_blue / src->maxval);
            cimg.setPixelLimited(x, y, cv);
          }
    }
  addImageParameters(result, 0);
};

void X2Gray::get_data()
{
  DType itype = getInputType(0);
  if (itype.matchType(DType::image))
    {
      const GImage* img = getInputImage(0);
      if (img->getSize() == 1)
        {
          result[0] = cloneGImage(img);
        }
      else
        {
          int mode = getInputInt(1, gray);
          GImage* tresult = mkGImage(img, 1);
          result[0] = tresult;
          const Image& dst = (*tresult)[0];

          ColorImage cimg((*img)[0], (*img)[1], (*img)[2]);

          switch (mode)
            {
            case gray:
              for (int y = 0; y < dst.ysize; ++y)
                for (int x = 0; x < dst.xsize; ++x)
                  {
                    ColorValue g = cimg.getPixel(x, y);
                    dst.setIntensity(x, y, g.getGray());
                  }
              break;
            case red:
              for (int y = 0; y < dst.ysize; ++y)
                for (int x = 0; x < dst.xsize; ++x)
                  {
                    ColorValue g = cimg.getPixel(x, y);
                    dst.setIntensity(x, y, g.red);
                  }
              break;
            case green:
              for (int y = 0; y < dst.ysize; ++y)
                for (int x = 0; x < dst.xsize; ++x)
                  {
                    ColorValue g = cimg.getPixel(x, y);
                    dst.setIntensity(x, y, g.green);
                  }
              break;
            case blue:
              for (int y = 0; y < dst.ysize; ++y)
                for (int x = 0; x < dst.xsize; ++x)
                  {
                    ColorValue g = cimg.getPixel(x, y);
                    dst.setIntensity(x, y, g.blue);
                  }
              break;
            }
          addImageParameters(result, 0);
        }
    }
};

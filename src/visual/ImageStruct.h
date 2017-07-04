#ifndef ICE_IMAGE_STRUCT_H
#define ICE_IMAGE_STRUCT_H

#include "util.h"
#include "base.h"
#include "based.h"

namespace ice
{
  class ImageStruct
  {
  public:
    ImageStruct()
    {
    }

    virtual ~ImageStruct() {}

    virtual void StopVis() = 0;
    virtual int ImageType() const = 0;

    virtual unsigned char getValueShifted(int x, int y) const = 0;
    virtual int getPixel(int x, int y) const = 0;
  };

  class ImageStructInt: public ImageStruct
  {
  private:
    ImageBase* img;
    unsigned int shift;
  public:
    ImageStructInt(ImageBase* imgp): img(imgp)
    {
      shift = 0;
      while ((imgp->maxval >> shift) > 255)
        {
          shift++;
        }
    }
    virtual ~ImageStructInt() {};

    void StopVis()
    {
      img -> stopVis();
    }

    virtual int ImageType() const
    {
      return 1;
    }

    const ImageBase* Img() const
    {
      return img;
    }

    unsigned char getValueShifted(int x, int y) const
    {
      return (img->getP(x, y) >> shift) ^ 255;
    }

    int getPixel(int x, int y) const
    {
      return img->getP(x, y);
    }
  };

  class ImageStructDouble: public ImageStruct
  {
  private:
    ImageD* img;
    ImageD* img_duplicate;
  public:
    ImageStructDouble(ImageD* imgp): img(imgp) //?? img(new ImageD(*imgp))
    {
      // duplicate image avoid destruction of image buffer during paint
      img_duplicate = new ImageD(*imgp);
    }

    virtual ~ImageStructDouble()
    {
      delete img_duplicate;
    };

    void StopVis()
    {
      img -> stopVis();
    }

    virtual int ImageType() const
    {
      return 2;
    }

    const ImageD* Img() const
    {
      return img;
    }

    unsigned char getValueShifted(int x, int y) const
    {
      double range = img->maxval - img->minval;
      double shiftedValue = img->maxval - img->getPixel(x, y);
      double normalizedValue = shiftedValue / range * 255 ;
      return limited(normalizedValue, 0, 255);
    }

    int getPixel(int x, int y) const
    {
      return getValueShifted(x, y);
    }
  };
}
#endif

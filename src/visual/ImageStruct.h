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

    virtual int ImageType() const = 0;
    virtual int getReferenceCount() const = 0;
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
      imgp->refcount++;
      shift = 0;
      while ((imgp->maxval >> shift) > 255)
        {
          shift++;
        }
    }

    virtual ~ImageStructInt()
    {
      img->refcount--;
      if (img->refcount == 0)
        {
          delete img;
        }
    };

    virtual int ImageType() const
    {
      return 1;
    }

    virtual int getReferenceCount() const
    {
      return img->refcount;
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
  public:
    ImageStructDouble(ImageD* imgp): img(imgp)
    {
      img->getMatrix().refcount++;
    }

    virtual ~ImageStructDouble()
    {
      img->getMatrix().refcount--;
      if (img->getMatrix().refcount == 0)
        delete img;
    };

    virtual int ImageType() const
    {
      return 2;
    }

    const ImageD* Img() const
    {
      return img;
    }

    virtual int getReferenceCount() const
    {
      return img->getMatrix().refcount;
    }

    unsigned char getValueShifted(int x, int y) const
    {
      double max = img->getMatrix().maxValue;
      double min = img->getMatrix().minValue;
      double range = max - min;
      double shiftedValue = max - img->getPixel(x, y);
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

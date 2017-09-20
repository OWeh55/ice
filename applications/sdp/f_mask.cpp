
#include "genimage.h"
#include "gregion.h"

#include "f_mask.h"

void MaskImage::get_data()
{
  const GImage *src1 = getInputPointer<GImage, DType::image>(0);
  const GData *src2 = getInputData(1, DType::image | DType::region);

  calcSize(src1);

  bool imagemask = false;

  if (src2->getType().type == DType::image)
    {
      imagemask = true;
      if (src2->getSize() != 1)
        SdpException(name, "expected binary image as mask");
      adaptSize(dynamic_cast<const GImage *>(src2));
    }

  vmax = src1->maxVal();

  GImage *tresult = createResult();

  if (imagemask)
    {
      const GImage *srci = dynamic_cast<const GImage *>(src2);
      for (int y = 0; y < ysize; ++y)
        for (int x = 0; x < xsize; ++x)
          {
            int maskval = (*srci)[0].getPixel(x, y);
            for (int i = 0; i < nImg; ++i)
              {
                int srcval = (*src1)[i].getPixel(x, y);

                if (maskval != 0)
                  (*tresult)[i].setPixel(x, y, srcval);
                else
                  (*tresult)[i].setPixel(x, y, 0);
              }
          }
    }
  else
    {
      // mask is Region
      Region reg = (* dynamic_cast<const GRegion *>(src2))[0];
      for (int y = 0; y < ysize; ++y)
        for (int x = 0; x < xsize; ++x)
          {
            bool maskval = reg.inside(x, y);
            for (int i = 0; i < nImg; ++i)
              {
                int srcval = (*src1)[i].getPixel(x, y);

                if (maskval)
                  (*tresult)[i].setPixel(x, y, srcval);
                else
                  (*tresult)[i].setPixel(x, y, 0);
              }
          }
    }
}

void SelectImage::get_data()
{
  const GImage *src1 = getInputPointer<GImage, DType::image>(0);
  const GImage *src2 = getInputPointer<GImage, DType::image>(1);
  const GData *src3 = getInputData(2, DType::image | DType::region);

  calcSize(src1);
  adaptSize(src2);

  if (src1->getSize() != src2->getSize())
    throw SdpException("select", "image types differ");

  bool imagemask = false;

  if (src3->getType().type == DType::image)
    {
      imagemask = true;
      if (src2->getSize() != 1)
        SdpException(name, "expected binary image as mask");
      adaptSize(dynamic_cast<const GImage *>(src2));
    }

  nImg = src1->getSize();
  vmax = src1->maxVal();

  GImage *tresult = createResult();

  if (imagemask)
    {
      const GImage *srci = dynamic_cast<const GImage *>(src3);
      for (int y = 0; y < ysize; ++y)
        for (int x = 0; x < xsize; ++x)
          {
            int maskval = (*srci)[0].getPixel(x, y);
            for (int i = 0; i < nImg; ++i)
              {
                int srcval1 = (*src1)[i].getPixel(x, y);
                int srcval2 = (*src2)[i].getPixel(x, y);

                if (maskval != 0)
                  (*tresult)[i].setPixel(x, y, srcval1);
                else
                  (*tresult)[i].setPixel(x, y, srcval2);
              }
          }
    }
  else
    {
      // mask is Region
      Region reg = (* dynamic_cast<const GRegion *>(src3))[0];
      for (int y = 0; y < ysize; ++y)
        for (int x = 0; x < xsize; ++x)
          {
            bool maskval = reg.inside(x, y);
            for (int i = 0; i < nImg; ++i)
              {
                int srcval1 = (*src1)[i].getPixel(x, y);
                int srcval2 = (*src2)[i].getPixel(x, y);

                if (maskval)
                  (*tresult)[i].setPixel(x, y, srcval1);
                else
                  (*tresult)[i].setPixel(x, y, srcval2);
              }
          }
    }
}

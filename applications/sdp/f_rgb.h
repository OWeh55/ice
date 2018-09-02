#ifndef SDP_FILTER_RGB_H
#define SDP_FILTER_RGB_H

#include "filterfactory.h"

#include "f_filter.h"
#include "f_ifilter.h"

class X2RGB : public ImageFilter
{
public:
  X2RGB():
    ImageFilter("color2rgb", 1, 6, "split color input image into 3 channels - "
                "<colorimage> => <red_image> <green_image> <blue_image>")
  {
  }

  FFUNC(X2RGB)

protected:
  virtual void get_data()
  {
    const GImage* src = getInputImage(0);
    calcSize(src);
    if (nImg != 3)
      throw SdpException(name, "expected color image");
    GImage* res1;
    GImage* res2;
    GImage* res3;
    result[0] = res1 = mkGImage(src, 1);
    CopyImg((*src)[0], (*res1)[0]);
    result[1] = res2 = mkGImage(src, 1);
    CopyImg((*src)[1], (*res2)[0]);
    result[2] = res3 = mkGImage(src, 1);
    CopyImg((*src)[2], (*res3)[0]);

    result[3] = new GInteger(res1->xSize());
    result[4] = new GInteger(res1->ySize());
    result[5] = new GInteger(res1->maxVal());
  }
};

FF(X2RGB)

class X2HSI : public ImageFilter
{
public:
  X2HSI():
    ImageFilter("color2hsi", 1, 6, "split color input image into 3 channels - "
                "<colorimage> => <hue_image> <saturation_image> <intensity_image>")
  {
  }

  FFUNC(X2HSI)

protected:
  virtual void get_data()
  {
    const GImage* src = getInputImage(0);
    calcSize(src);
    if (nImg != 3)
      throw SdpException(name, "expected color image");

    ColorImage cimg((*src)[0], (*src)[1], (*src)[2]);

    GImage* res1;
    GImage* res2;
    GImage* res3;
    result[0] = res1 = mkGImage(src, 1);
    result[1] = res2 = mkGImage(src, 1);
    result[2] = res3 = mkGImage(src, 1);

    ColorImageToHsi(cimg, (*res1)[0], (*res2)[0], (*res3)[0]);

    result[3] = new GInteger(res1->xSize());
    result[4] = new GInteger(res1->ySize());
    result[5] = new GInteger(res1->maxVal());
  }
};

FF(X2HSI)

class X2YUV : public ImageFilter
{
public:
  X2YUV():
    ImageFilter("color2yuv", 1, 6, "split color input image into 3 channels - "
                "<colorimage> => <Y_image> <U_image> <V_image>")
  {
  }

  FFUNC(X2YUV)

protected:
  virtual void get_data()
  {
    const GImage* src = getInputImage(0);
    calcSize(src);
    if (nImg != 3)
      throw SdpException(name, "expected color image");

    ColorImage cimg((*src)[0], (*src)[1], (*src)[2]);

    GImage* res1;
    GImage* res2;
    GImage* res3;
    result[0] = res1 = mkGImage(src, 1);
    result[1] = res2 = mkGImage(src, 1);
    result[2] = res3 = mkGImage(src, 1);

    ColorImageToHsi(cimg, (*res1)[0], (*res2)[0], (*res3)[0]);

    result[3] = new GInteger(res1->xSize());
    result[4] = new GInteger(res1->ySize());
    result[5] = new GInteger(res1->maxVal());
  }
};

FF(X2YUV)

class RGB2Color : public ImageFilter
{
public:
  RGB2Color():
    ImageFilter("rgb2color", 3, 5,
                "combine 3 images to rgb color image - <red> <green> <blue> => <color>", mm_strict)
  {
  }

  FFUNC(RGB2Color)

protected:
  virtual void get_data()
  {
    const GImage* src1 = getInputImage(0);
    calcSize(src1);
    if (nImg != 1)
      throw SdpException(name, "expected grayvalue image");
    const GImage* src2 = getInputImage(1);
    adaptSize(src2);
    const GImage* src3 = getInputImage(2);
    adaptSize(src3);
    nImg = 3;
    GImage* tresult = createResult();

    CopyImg((*src1)[0], (*tresult)[0]);
    CopyImg((*src2)[0], (*tresult)[1]);
    CopyImg((*src3)[0], (*tresult)[2]);
  }
};

FF(RGB2Color)

#endif

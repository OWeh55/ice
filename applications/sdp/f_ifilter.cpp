#include "gnumber.h"
#include "genimage.h"

#include "f_ifilter.h"

void ImageFilter::adaptSize(const GImage* in)
{
  if (mm & mm_size)
    {
      if (xsize != in->xSize() ||
          ysize != in->ySize())
        throw SdpException(name, "image sizes differ");
    }
  else
    {
      xsize = ice::min(xsize, in->xSize());
      ysize = ice::min(ysize, in->ySize());
    }

  if (mm & mm_value)
    {
      if (vmax != in->maxVal())
        throw SdpException(name, "maximum values differ");
    }
  else
    vmax = ice::max(vmax, in->maxVal());

  if (mm & mm_channels)
    {
      if (nImg != in->getSize())
        throw SdpException(name, "image types differ");
    }
  else
    {
      nImg = ice::max(nImg, in->getSize());
    }
}

void ImageFilter::calcSize(const GImage* in)
{
  xsize = in->xSize();
  ysize = in->ySize();
  vmax = in->maxVal();
  nImg = in->getSize();
}

void ImageFilter::calcSize(const Image& in, int nImages)
{
  xsize = in.xsize;
  ysize = in.ysize;
  vmax = in.maxval;
  nImg = nImages;
}

void ImageFilter::calcSize(const GImage* in1, const GImage* in2)
{
  calcSize(in1);
  adaptSize(in2);
}

void ImageFilter::addImageParameters(vector<GData*>& result, int index)
{
  if (index + 5 > (int)result.size())
    throw SdpException(name, "result index out of range");
  const GImage* img = dynamic_cast<const GImage*>(result[index]);
  if (img == NULL)
    throw SdpException("ImageFilter", "result is no image");
  result[index + 1] = new GInteger(img->xSize());
  result[index + 2] = new GInteger(img->ySize());
  result[index + 3] = new GInteger(img->maxVal());
  result[index + 4] = new GInteger(img->getSize());
}


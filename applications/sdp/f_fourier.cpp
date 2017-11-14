#include "genimage.h"
#include "gnumber.h"

#include "f_fourier.h"

void FourierImg::get_data()
{
  const GImage *img = getInputPointer<GImage, DType::image>(0);
  Image real, imag;
  if (img->getSize() == 1)
    {
      real = (*img)[0];
      imag = NewImg(real.xsize, real.ysize, real.maxval);
    }
  else if (img->getSize() >= 2)
    {
      real = (*img)[0];
      imag = (*img)[1];
    }
  else
    throw SdpException("FourierImg", "Wrong number of images");

  int xsize = real.xsize;
  int ysize = real.ysize;

  int rmaxval = (1 << 30) - 1;
  int rnull = (rmaxval + 1) / 2;

  GImage *tresult = mkGImage(xsize, ysize, rmaxval, 2);
  result[0] = tresult;
  ImageD dreal;
  dreal.create(xsize, ysize);
  ImageD dimag;
  dimag.create(imag.xsize, imag.ysize);
  ImageD dreal2;
  dreal2.create(xsize, ysize);
  ImageD dimag2;
  dimag2.create(imag.xsize, imag.ysize);

  double rsum = 0;
  double isum = 0;

  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        rsum += real.getPixel(x, y);
        isum += imag.getPixel(x, y);
      }

  rsum /= xsize * ysize;
  isum /= xsize * ysize;

  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        dreal.setPixel(x, y, real.getPixel(x, y) - rsum);
        dimag.setPixel(x, y, imag.getPixel(x, y) - isum);
      }

  FourierImgD(dreal, dimag, NORMAL, dreal2, dimag2);

  double maxval = 0.0001;
  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        double aval = fabs(dreal2.getPixel(x, y));
        if (aval > maxval)
          maxval = aval;
        aval = fabs(dimag2.getPixel(x, y));
        if (aval > maxval)
          maxval = aval;
      }

  double factor = (rmaxval / 2) / maxval;
  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        double aval = dreal2.getPixel(x, y);
        (*tresult)[0].setPixel(x, y, RoundInt(aval * factor) + rnull);
        aval = dimag2.getPixel(x, y);
        (*tresult)[1].setPixel(x, y, RoundInt(aval * factor) + rnull);
      }
  result[1] = new GFloat(1.0 / factor);
  result[2] = new GFloat(rsum);
  result[3] = new GFloat(isum);
  result[4] = new GInteger(real.maxval);
}

void FourierInverseImg::get_data()
{
  const GImage *img = getInputPointer<GImage, DType::image>(0);

  if (img->getSize() != 2)
    {
      throw SdpException("FourierInverseImg", "Expected complex image");
    }

  const Image &real = (*img)[0];
  const Image &imag = (*img)[1];

  int xsize = real.xsize;
  int ysize = real.ysize;

  double factor = getInputFloat(1, 1);
  double rsum = getInputFloat(2, 0);
  double isum = getInputFloat(3, 0);
  int rnull = (real.maxval + 1) / 2;
  int maxval = getInputInt(4, 255);

  GImage *tresult = mkGImage(xsize, ysize, maxval, 2);
  result[0] = tresult;
  ImageD dreal;
  dreal.create(xsize, ysize);
  ImageD dimag;
  dimag.create(imag.xsize, imag.ysize);
  ImageD dreal2;
  dreal2.create(xsize, ysize);
  ImageD dimag2;
  dimag2.create(imag.xsize, imag.ysize);

  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        dreal.setPixel(x, y, (real.getPixel(x, y) - rnull)*factor);
        dimag.setPixel(x, y, (imag.getPixel(x, y) - rnull)*factor);
      }

  FourierImgD(dreal, dimag, INVERS, dreal2, dimag2);

  for (int y = 0; y < ysize; ++y)
    for (int x = 0; x < xsize; ++x)
      {
        double aval = dreal2.getPixel(x, y);
        (*tresult)[0].setPixelLimited(x, y, aval + rsum);
        aval = dimag2.getPixel(x, y);
        (*tresult)[1].setPixelLimited(x, y, aval + isum);
      }
}

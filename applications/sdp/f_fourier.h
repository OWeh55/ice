#ifndef SDP_FOURIER_H
#define  SDP_FOURIER_H

#include "exceptions.h"

#include "filterfactory.h"

#include "f_ifilter.h"

class FourierImg : public ImageFilter
{
public:
  FourierImg():
    ImageFilter("fourierimage", 1, 5, "fourier transform of image - "
                "<image> => <image> <factor> <real> <imag> <maxval>")
  {
  }

  FFUNC(FourierImg)

protected:
  virtual void get_data();
};

FF(FourierImg);

class FourierInverseImg : public ImageFilter
{
public:
  FourierInverseImg():
    ImageFilter("fourierinverseimage", 5, 1, "inverse fourier transform of image - "
                "<image> <factor> <real> <imag> <maxval> => <image>")
  {
  }

  FFUNC(FourierInverseImg)

protected:
  virtual void get_data();
};

FF(FourierInverseImg);

#endif

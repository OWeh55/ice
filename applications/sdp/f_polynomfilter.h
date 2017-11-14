#ifndef IM_POLYNOM_H
#define IM_POLYNOM_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class PolynomImage: public FilterAllChannelsOneInput
{
public:
  PolynomImage(): FilterAllChannelsOneInput("polynomfilter", 5, 5,
        "filter based on fitting a polynom - "
        "<image> <size> <grad> <ix> <iy> => <image>") {}
  FFUNC(PolynomImage);
protected:
  void filterOneChannel(const Image &img1, const Image &img2)
  {
    int size = getInputInt(1);
    int grad = getInputInt(2);
    int ix = getInputInt(3);
    int iy = getInputInt(4);
    LSIFilter pf = mkPolynomFilter(size, grad, ix, iy);
    int offset = pf.proposeOffset(img2);
    pf.Filter(img1, img2, offset);
  }
};

FF(PolynomImage);
#endif

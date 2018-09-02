#ifndef IM_ORIENTED_DOB_H
#define IM_ORIENTED_DOB_H

#include "filterfactory.h"
#include "f_ifilter.h"

class OrientedDob: public ImageFilter
{
public:
  OrientedDob():
    ImageFilter("orienteddob", 5, 5,
                "oriented DoB filter - "
                "<image> <directionimage> <size> <lenght> <width> => <image+>") {}
  FFUNC(OrientedDob)
protected:
  virtual void get_data()
  {
    const GImage* img1 = getInputImage(0);
    const GImage* img2 = getInputImage(1);
    if (img2->getSize() != 1)
      throw SdpException(name, "Need grayvalue image for direction");

    calcSize(img1, img2);
    GImage* tresult = createResult();

    int fsize = getInputInt(2, 11);
    int flength = getInputInt(3, fsize);
    int fwidth = getInputInt(4, 1);

    for (int i = 0; i < tresult->getSize(); ++i)
      OrientedDoBImg((*img1)[i], (*img2)[0], (*tresult)[i],
                     fsize, flength, fwidth);
  }
};
FF(OrientedDob)
#endif

#ifndef IM_ORIENTED_SMEAR_H
#define IM_ORIENTED_SMEAR_H

#include "filterfactory.h"
#include "f_ifilter.h"

class OrientedSmear: public ImageFilter
{
public:
  OrientedSmear(): ImageFilter("orientedsmear", 5, 5,
                                 "oriented smear filter - "
                                 "<image> <directionimage> <size> <lenght> <width> => <image+>") {}
  FFUNC(OrientedSmear)

protected:
  virtual void get_data()
  {
    const GImage* img1 = getInputImage(0);
    const GImage* img2 = getInputImage(1);
    if (img2->getSize() != 1)
      throw SdpException(name, "Need grayvalue image for direction");
    // cout << img1->maxVal()<<" " << img2->maxVal() <<endl;
    calcSize(img1);
    GImage* tresult = createResult();

    int fsize = getInputInt(2, 11);
    int flength = getInputInt(3, fsize);
    int fwidth = getInputInt(4, 1);

    for (int i = 0; i < tresult->getSize(); ++i)
      OrientedSmearImg((*img1)[i], (*img2)[0], (*tresult)[i],
                       fsize, flength, fwidth);
  }
};
FF(OrientedSmear)
#endif

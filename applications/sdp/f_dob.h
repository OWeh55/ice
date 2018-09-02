#ifndef IM_DOB_H
#define IM_DOB_H

#include "filterfactory.h"
#include "f_filterallchannels1.h"

class DobImage: public FilterAllChannelsOneInput
{
protected:
  int s1, s2;
public:
  DobImage(): FilterAllChannelsOneInput("dob", 3, 5, "apply DoB filter - "
                                          "<image> <size1*> <size2*> => <image+>") {}
  FFUNC(DobImage);
protected:
  void filterOneChannel(const Image& img1, const Image& img2)
  {
    int s1 = getInputInt(1, 5);
    int s2 = getInputInt(2, 15);
    ice::DoBImg(img1, img2, s1, s2);
  }
};
FF(DobImage)
#endif

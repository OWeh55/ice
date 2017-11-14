#ifndef IM_INVERS_H
#define IM_INVERS_H

#include "f_filterallchannels1.h"

class InverseImage: public FilterAllChannelsOneInput
{
public:
  InverseImage(): FilterAllChannelsOneInput("inverseimage", 1, 5,
        "inverts the input image - <image> => <inverted image>") {}
  FFUNC(InverseImage)

protected:
  void filterOneChannel(const Image &src, const Image &dst)
  {
    invertImg(src, dst);
  }
};

FF(InverseImage)

#endif

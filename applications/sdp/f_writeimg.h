#ifndef IM_IMAGE_WRITE_H
#define IM_IMAGE_WRITE_H

#include "genimage.h"
#include "f_filter.h"
#include "f_ifilter.h"

class ImageWrite: public ImageFilter
{
public:
  ImageWrite(): ImageFilter("writeimage", 2, 5,
                              "write image to file (and copy to output)"
                              "- <image_in> <filename> => <image_out>") {}
  FFUNC(ImageWrite)

protected:
  virtual void get_data()
  {
    const GImage *img = getInputImage(0);
    string fn = getInputString(1);
    int nImg = img->getSize();
    GImage *tresult = cloneGImage(img);
    result[0] = tresult;

    if (nImg == 1)
      {
        WriteImg((*img)[0], fn);
      }
    else
      {
        WriteImg((*img)[0], (*img)[1], (*img)[2], fn);
      }
    addImageParameters(result, 0);
  }
};

FF(ImageWrite);

#endif

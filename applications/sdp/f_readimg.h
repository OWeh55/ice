#ifndef IM_IMAGE_FILE_H
#define IM_IMAGE_FILE_H

#include "exceptions.h"

#include "f_ifilter.h"

class ImageFile: public ImageFilter
{
public:
  ImageFile(): ImageFilter("readimage", 1, 5, "read image from file - <filename> => <image>") {}
  FFUNC(ImageFile)
protected:
  virtual void get_data()
  {
    string filename = getInputString(0);
    
    try {
      InfImgFile(filename, xsize, ysize, vmax, nImg);
    }
    catch (IceException &ex)
      {
	throw FileException("ImageFile", filename);
      }

    GImage *tresult = createResult();

    if (nImg == 1)
      ReadImg(filename, (*tresult)[0]);
    else
      ReadImg(filename, (*tresult)[0], (*tresult)[1], (*tresult)[2]);
    addImageParameters(result, 0);
  }
};

FF(ImageFile)
#endif

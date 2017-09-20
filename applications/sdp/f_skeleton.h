#ifndef IM_IMAGE_SKELETON_H
#define IM_IMAGE_SKELETON_H

#include "f_ifilter.h"

class ImageSkeleton: public ImageFilter
{
public:
  ImageSkeleton(): ImageFilter("skeleton", 2, 5,
			       "skeletonizing - "
			       "<image> <threshold*> => <image+>") { }
  
  FFUNC(ImageSkeleton)

  protected:
  virtual void get_data()
  {
    const GImage *img = getInputImage(0);
    int threshold = getInputInt(1, 1);
    
    if (img->getSize() != 1)
      throw SdpException(name, "expected gray value image");
    
    GImage *tresult = mkGImage(img);
    result[0] = tresult;
    addImageParameters(result, 0);
    
    SkeletonImg((*img)[0], (*tresult)[0], threshold);
  }
};

FF(ImageSkeleton)

#endif

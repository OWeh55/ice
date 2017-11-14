#include "exceptions.h"
#include "genimage.h"

GImage *mkGImage(int xm, int ym, int vm, int nImg)
{
  return new GImage(xm, ym, vm, nImg);
}

GImage *mkGImage(const GImage *timg, int nImg)
{
  return mkGImage(timg->xSize(), timg->ySize(), timg->maxVal(), nImg);
}

GImage *mkGImage(const GImage *timg)
{
  return mkGImage(timg, timg->getSize());
}

GImage *cloneGImage(const GImage *img)
{
  GImage *res = mkGImage(img);
  copyGImage(img, res);
  return res;
}

bool matchGImage(const GImage *img1, const GImage *img2)
{
  return img1->getSize() == img2->getSize() &&
         img1->xSize() != img2->xSize() &&
         img1->ySize() != img2->ySize() &&
         img1->maxVal() != img2->maxVal() ;
}

void copyGImage(const GImage *img1, const GImage *img2)
{
  if (img1->getType() != img2->getType())
    throw SdpException("copyGImage", "different kind of images");
  for (int i = 0; i < img1->getSize(); ++i)
    CopyImg((*img1)[i], (*img2)[i]);
}

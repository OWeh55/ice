#ifndef GEN_IMAGE_H
#define GEN_IMAGE_H

// "generic image"

#include <image.h>
#include <vector>

#include "gdata.h"

class GImage: public GData
{
public:
  GImage(): GData() {}
  GImage(int xs, int ys, int vm, int nImages): GData()
  {
    Image h;
    for (int i = 0; i < nImages; ++i)
      {
        imgv.push_back(NewImg(xs, ys, vm));
      }
  }
  virtual ~GImage() {}

  virtual void copyFrom(const GImage* img2)
  {
    if (getType() != img2->getType())
      throw SdpException("GImage::copyFrom", "different kind of images");
    for (int i = 0; i < getSize(); ++i)
      CopyImg((*img2)[i], imgv[i]);
  }

  virtual GImage* newGImage() const
  {
    return new GImage(imgv[0].xsize, imgv[0].ysize, imgv[0].maxval, imgv.size());
  }

  virtual GData* clone() const
  {
    GImage* res = newGImage();
    for (int i = 0; i < (int)imgv.size(); ++i)
      CopyImg(imgv[i], res->imgv[i]);
    return res;
  }

  virtual DType getType() const
  {
    return DType(DType::image, imgv.size());
  }

  // size is number of Images == channels
  virtual int getSize() const
  {
    return imgv.size();
  }

  virtual void resize(int i)
  {
    if (imgv.empty())
      throw SdpException("GImage", "Cannot resize empty image sequence");
    int oldsize = imgv.size();
    imgv.resize(i);
    for (int i = oldsize; i < (int)imgv.size(); ++i)
      imgv[i] = NewImg(imgv[0]);
  }

  virtual int xSize() const
  {
    return imgv[0].xsize;
  }
  virtual int ySize() const
  {
    return imgv[0].ysize;
  }
  virtual int maxVal() const
  {
    return imgv[0].maxval;
  }

  // n-th channel == Image
  virtual Image& operator[](int i)
  {
    return imgv[i];
  }
  virtual const Image& operator[](int i) const
  {
    return imgv[i];
  }
protected:
  std::vector<Image> imgv;
};

GImage* mkGImage(int xm, int ym, int vm, int itype);
GImage* mkGImage(const GImage* timg, int itype);
GImage* mkGImage(const GImage* timg);

GImage* cloneGImage(const GImage* timg);

void copyGImage(const GImage* img1, const GImage* img2);

bool matchGImage(const GImage* img1, const GImage* img2);

#endif

#ifndef SDP_FORK_H
#define SDP_FORK_H

#include "filterfactory.h"

#include "f_filter.h"
#include "f_ifilter.h"

class Fork2 : public ImageFilter
{
public:
  Fork2():
    ImageFilter("fork2", 1, 2, "split input image sequence into 2 channels - "
                "<image> => <image1> <green_image2>")
  {
  }

  FFUNC(Fork2);

protected:
  virtual void get_data()
  {
    const GImage *src = getInputImage(0);
    calcSize(src);
    if (nImg < 2)
      throw SdpException(name, "expected image sequence");

    nImg = 1;
    GImage *res1 = createResult(0, false);
    GImage *res2 = createResult(1, false);

    CopyImg((*src)[0], (*res1)[0]);
    CopyImg((*src)[1], (*res2)[0]);
  }
};

FF(Fork2)

class Combine2 : public ImageFilter
{
public:
  Combine2():
    ImageFilter("combine2", 2, 1,
                "combine 2 images to image - <image1> <image2> => <images>", mm_strict)
  {
  }

  FFUNC(Combine2)

protected:
  virtual void get_data()
  {
    const GImage *src1 = getInputImage(0);
    calcSize(src1);
    if (nImg != 1)
      throw SdpException(name, "expected grayvalue image");
    const GImage *src2 = getInputImage(1);
    adaptSize(src2);
    nImg = 2;

    GImage *tresult = createResult(0, false);

    CopyImg((*src1)[0], (*tresult)[0]);
    CopyImg((*src2)[0], (*tresult)[1]);
  }
};

FF(Combine2)

#endif

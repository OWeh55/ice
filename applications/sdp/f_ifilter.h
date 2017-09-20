#ifndef SDP_IMAGE_FILTER_H
#define SDP_IMAGE_FILTER_H

#include "gnumber.h"
#include "genimage.h"

#include "f_filter.h"

class ImageFilter: public Filter
{
public:
  static const int mm_size = 1;
  static const int mm_value = 2;
  static const int mm_channels = 4;

  static const int mm_lax = 0;
  static const int mm_strict = 7;

  ImageFilter(const string &name, int in, int out,
              const std::string &help = "", int matchmode = mm_strict):
    Filter(name, in, out, help),
    xsize(0), ysize(0), vmax(0), nImg(0), mm(matchmode) {};

  virtual ~ImageFilter() {}

  virtual void adaptSize(const GImage *in);
  virtual void calcSize(const GImage *in);
  virtual void calcSize(const Image &in, int nImages = 1);

  virtual void calcSize(const GImage *in1, const GImage *in2);

  virtual GImage *createResult(int idx = 0, bool withParameters = true)
  {
    GImage *tresult = mkGImage(xsize, ysize, vmax, nImg);
    result[idx] = tresult;
    if (withParameters)
      addImageParameters(result, idx);
    return tresult;
  }

  virtual void addImageParameters(vector<GData *> &result, int index);
protected:
  int xsize, ysize, vmax, nImg; // typical data for images
  int mm;
};
#endif

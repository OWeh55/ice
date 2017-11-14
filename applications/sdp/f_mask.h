#ifndef SDP_F_MASK_H
#define SDP_F_MASK_H

#include "f_ifilter.h"

class MaskImage: public ImageFilter
{
public:
  MaskImage(): ImageFilter("mask", 2, 5,
                             "mask image with binary mask - "
                             "<image> <mask> => <image>") {}
  FFUNC(MaskImage);

protected:
  virtual void get_data();

};

FF(MaskImage);

class SelectImage: public ImageFilter
{
public:
  SelectImage(): ImageFilter("select", 3, 5,
                               "select pixelwise from 2 sources - "
                               "<image_true> <image_false> <selection> => <image>",
                               mm_size | mm_value) {}
  FFUNC(SelectImage);

protected:
  virtual void get_data();

};

FF(SelectImage);

#endif

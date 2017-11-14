#ifndef IM_TYPE_CONVERSION_H
#define IM_TYPE_CONVERSION_H

#include "exceptions.h"

#include "filterfactory.h"

#include "f_ifilter.h"

class X2Color : public ImageFilter
{
public:
  X2Color():
    ImageFilter("x2color", 4, 5, "convert image to color image - "
                "<image> <color_value> => <colorimage>")
  {
  }

  FFUNC(X2Color)

protected:
  virtual void get_data();
};

FF(X2Color)

class X2Gray : public ImageFilter
{
public:
  static const int red = 0;
  static const int green = 1;
  static const int blue = 2;
  static const int gray = 3;

  X2Gray(): ImageFilter("x2gray", 2, 5, "convert image to grayvalue image - "
                          " <image> <mode> => <grayvalueimage>") {}
  FFUNC(X2Gray)

protected:
  virtual void get_data();
};

FF(X2Gray)
#endif

#ifndef SDP_DIAL_H
#define SDP_DIAL_H

#include "f_filter.h"
#include "gnumber.h"

class Dial: public Filter
{
public:
  Dial(int val, int min, int max, int raster):
    Filter("dial", 0, 1, "interactive selection of value -  => <value>"),
    value(val), min(min), max(max), raster(raster)
  {
    if (value < min || value > max)
      throw SdpException("dial", "value out of range");

    if (raster > (max - min))
      throw SdpException("dial", "raster to big for range");

    step = raster;
    normalizeStep();
  }

  void normalizeStep()
  {
    if (step < raster)
      step = raster;
    step = ((step + raster - 1) / raster) * raster;
  }

  FFUNC(Dial);

  virtual void up()
  {
    int valuen = value + step;
    if (valuen <= max)
      value = valuen;
  }

  virtual void down()
  {
    int valuen = value - step;
    if (valuen >= min)
      value = valuen;
  }

  virtual void fine()
  {
    if (step > raster)
      {
        step = step * 2 / 3;
        normalizeStep();
      }
  }

  virtual void coarse()
  {
    if (step > 1)
      {
        step = step * 3 / 2;
      }
    else
      {
        step = 2;
      }
    normalizeStep();
  }

protected:
  virtual void get_data()
  {
    result[0] = new GInteger(value);
  }

public:
  int value;
  int min;
  int max;
  int raster;
  int step;
};

bool handleDials();

#endif

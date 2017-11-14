#ifndef IM_IMAGE_DISPLAY_H
#define IM_IMAGE_DISPLAY_H

#include "filterfactory.h"
#include "genimage.h"
#include "f_filter.h"

class display: public Filter
{
public:
  display(): Filter("display", 2, 1,
                      "display image (and copy to output) - <image_in> => <image_out>"), v(NULL), lastresult(NULL)
  {
  }

  virtual ~display()
  {
    if (v)
      Show(OFF, v);
  }

  virtual void reset()
  {
    lastresult = result[0];
    result[0] = NULL;
    Filter::reset();
  }

  CLONE(display)

protected:
  virtual void get_data();
  Visual v;
  GData *lastresult;
};

FF(display)

#endif

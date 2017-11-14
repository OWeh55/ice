#ifndef IM_DRAW_H
#define IM_DRAW_H

#include "filterfactory.h"
#include "f_ifilter.h"

class Draw: public ImageFilter
{
public:
  Draw(): ImageFilter("draw", 5, 5,
                        "draw data in image - "
                        "<image> <data> <redvalue*> <greenvalue*> <bluevalue*> => <image+>") {}
  FFUNC(Draw)

protected:

  virtual void get_data();
};
FF(Draw);
#endif

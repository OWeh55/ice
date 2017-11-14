#ifndef SDP_F_CONTUR_H
#define SDP_F_CONTUR_H

#include "f_filter.h"

class getConturs: public Filter
{
public:
  getConturs(): Filter("getconturs", 2, 1, "find all conturs in image - "
                         "<image> <threshold*> => <conturlist>")
  {
  }
  FFUNC(getConturs);
protected:
  virtual void get_data();
};

FF(getConturs);
#endif

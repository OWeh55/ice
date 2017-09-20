#ifndef SDP_F_INFO_H
#define SDP_F_INFO_H

#include "gpointset.h"

#include "f_filter.h"

class getInfos: public Filter
{
public:
  getInfos(): Filter("getinfos", 1, 5, "gives information about data - "
                       "<data> => <data> <size> <xsize> <ysize> <vmax>")
  {
  }
  FFUNC(getInfos);
protected:
  virtual void get_data();
};

FF(getInfos);
#endif

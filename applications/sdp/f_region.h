#ifndef SDP_F_REGION_H
#define SDP_F_REGION_H

#include "f_filter.h"

class Contur2Region: public Filter
{
public:
  Contur2Region(): Filter("contur2region", 1, 1, "convert conturs to regions - "
                            "<conturlist> => <regionlist>")
  {
  }
  FFUNC(Contur2Region);
protected:
  virtual void get_data();
};

FF(Contur2Region);
#endif

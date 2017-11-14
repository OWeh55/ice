#ifndef SDP_EVAL_H
#define SDP_EVAL_H

#include "filterfactory.h"
#include "f_filter.h"

class Evaluate: public Filter
{
public:
  Evaluate(): Filter("evaluate", 6, 1,
                       "evaluates expression for all data elements - "
                       "<expression> <data1> <data2> <data3> <data4> <data5> => <data>")
  {}
  FFUNC(Evaluate);

protected:

  virtual void get_data();
};

FF(Evaluate);

#endif

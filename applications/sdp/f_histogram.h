#ifndef SDP_HISTOGRAM_H
#define SDP_HISTOGRAM_H

#include "f_filter.h"

class CalcHistogram: public Filter
{
public:
  CalcHistogram(): Filter("histogram", 1, 1,
                            "calculate histogram of input data - "
                            "<inputdata> => <histogram>") {}
  FFUNC(CalcHistogram);

protected:
  virtual void get_data();
};

FF(CalcHistogram)

class Otsu: public Filter
{
public:
  Otsu(): Filter("otsu", 1, 1,
                   "calculate discrimination level - "
                   "<histogram|image> => <level>") {}
  FFUNC(Otsu);
protected:
  virtual void get_data();
};

FF(Otsu)

#endif

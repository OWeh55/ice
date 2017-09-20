#ifndef IM_DELAUNAY_H
#define IM_DELAUNAY_H

#include "f_filter.h"

class DelaunayPL: public Filter
{
public:
  DelaunayPL(): Filter("delaunay", 2, 1,
                         "delaunay triangulation of pointset - "
                         "<pointlist> => <graph>") {}
  FFUNC(DelaunayPL);

protected:
  virtual void get_data();
};

FF(DelaunayPL)
#endif

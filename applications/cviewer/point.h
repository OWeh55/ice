#ifndef POINT_H
#define POINT_H

#include "object.h"

class point: public objekt
{
protected:
  virtual void myDraw() const;
public:
  point(vect *posp): objekt(posp) { };

  virtual ~point() {};
};

#endif

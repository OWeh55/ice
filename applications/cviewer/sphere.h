#ifndef SPHERE_H
#define SPHERE_H

#include "point.h"

class sphere: public point
{
public:
  sphere(vect *posp, number *radp): point(posp), rad(radp) {};
  virtual ~sphere() {};

protected:
  virtual void MyOptimize();
  virtual void myDraw() const;
  numberp rad;
};

#endif

#ifndef CYLINDER_H
#define CYLINDER_H

#include "point.h"

class cylinder: public point
{
public:
  cylinder(vect *posp, vect *posp2, number *radp):
    point(posp), rad(radp), pos2(posp2) {};
  virtual ~cylinder() {};
protected:
  virtual void MyOptimize()
  {
    DEBUGOUT("opt.cylinder ");
    objekt::MyOptimize();
    optnumber(rad);
    optvector(pos2);
    DEBUGOUT(endl);
  };
  virtual void myDraw() const;
  numberp rad;
  vect *pos2;
};

#endif

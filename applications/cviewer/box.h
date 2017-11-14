#ifndef BOX_H
#define BOX_H


#include "point.h"

class box: public point
{
public:
  box(vect *posp, vect *posp2):
    point(posp), pos2(posp2), tex(NULL) {};
  box(vect *posp, vect *posp2, texture *texp):
    point(posp), pos2(posp2), tex(texp) {};
  virtual ~box() {};
protected:
  virtual void MyOptimize()
  {
    DEBUGOUT("opt. box ");
    objekt::MyOptimize();
    optvector(pos2);
    DEBUGOUT(endl);
  };
  virtual void myDraw() const;
  vect *pos2;
  texture *tex;
};


#endif

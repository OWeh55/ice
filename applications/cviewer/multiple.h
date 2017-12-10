#include "object.h"

class MultipleObject: public objekt
{
public:
  MultipleObject(number* fromp, number* top, number* stepp,
                 nvar* lvarp, objekt* obp):
    ob(obp), from(fromp), to(top), step(stepp), lvar(lvarp) {}
protected:
  void MyOptimize();
  objekt* ob; // sub-world which has to be drawn multiple
  number* from; // start value for variable
  number* to; // last value
  number* step; // step size
  nvar* lvar; // variable to change
protected:
  virtual void myDraw() const;
};

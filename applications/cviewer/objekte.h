#ifndef OBJEKTE_H
#define OBJEKTE_H

#include <iostream>

#include "cviewer.h"
#include "arith.h"
#include "surface.h"
#include "pbmio.h"

#include "object.h"
#include "point.h"

class color: public objekt
{
public:
  color(vect *posp): objekt(posp) {};
  ~color() {};
protected:
  virtual void myDraw() const;
  virtual void MyOptimize()
  {
    DEBUGOUT("optimize color");
    optvector(pos);
    DEBUGOUT(endl);
  }
};

class ocall: public objekt
{
public:
  ocall(objekt *ob, vect *shift, number *angle, vect *rot);
protected:
  void MyOptimize()
  {
    objekt::MyOptimize();
    DEBUGOUT("optimize ocall ");
    Optimize(ob);
    optnumber(angle);
    optvector(rot);
    DEBUGOUT(endl);
  }
  objekt *ob;
  number *angle;
  vect *rot;
protected:
  virtual void myDraw() const;
};

class ocalltrafo: public objekt
{
public:
  ocalltrafo(objekt *ob, trafo *shift);
protected:
  void MyOptimize()
  {
    objekt::MyOptimize();
    DEBUGOUT("optimize ocall ");
    Optimize(ob);
    //!!    opttrafo(tr);
    DEBUGOUT(endl);
  }
  objekt *ob;
  trafo *tr;
protected:
  virtual void myDraw() const;
};

class CondObject: public objekt
{
public:
  CondObject(boolean *cond, objekt *o1, objekt *o2):
    condition(cond), thenobject(o1), elseobject(o2) {}
protected:
  void MyOptimize()
  {
    objekt::MyOptimize();
    if (thenobject != NULL) Optimize(thenobject);
    if (elseobject != NULL) Optimize(elseobject);
  }
  boolean *condition;
  objekt *thenobject;
  objekt *elseobject;
protected:
  virtual void myDraw() const;
};

#endif

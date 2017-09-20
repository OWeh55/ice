#include "object.h"

void objekt::setv(vect * (&v), vect *vs)
{
  if (v != NULL) delete v;
  v = vs;
};

void objekt::setn(number * (&v), number *vs)
{
  if (v != NULL) delete v;
  v = vs;
};

void objekt::settr(trafo * (&tr), trafo *trs)
{
  if (tr != NULL) delete tr;
  tr = trs;
}

void objekt::optnumber(number * (&n))
{
  if (n->Constant())
    {
      DEBUGOUT("+") ;
      float value = n->Value();
      DEBUGOUT(value);
      delete n;
      n = new nconst(value);
    }
}

void objekt::optvector(vect * (&n))
{
  if (n->Constant())
    {
      DEBUGOUT("*") ;
      vect *vp = n;
      n = new vconst(n->Value());
      delete vp;
    }
}

#ifndef OBJECT_H
#define OBJECT_H

#include "tools.h"

#include "arith.h"

class objekt
{
private:
  objekt* next; // Verkettung
  objekt* prev; // zeiger auf voriges Element der liste,
  // zeigt beim ersten Element auf das letzte Element

protected:
  vect* pos;
  bool noop; // objekt tut nichts
  bool constant;

  static void setv(vect * (&p), vect* v);
  static void setn(number * (&p), number* v);
  static void settr(trafo * (&tr), trafo* trs);

  static void optnumber(number * (&n));
  static void optvector(vect * (&n));

  virtual void myDraw() const = 0;
  virtual void MyOptimize()
  {
    if (pos != NULL)
      optvector(pos);
  };

private:
  objekt(const objekt& o)
  {
    cerr << "copy constructor not implemented" << endl;
  }

public:
  objekt(vect* posp): next(this), prev(this), pos(posp), noop(false), constant(false) {}
  objekt(): next(this), prev(this), pos(NULL), noop(false), constant(false) {}

  virtual ~objekt()
  {
    if (next != this)
      {
        prev->next = next;
        next->prev = prev;
        delete next;
      }
  };

  void Draw(const objekt* first) const
  {
    myDraw();
    if (next != first)
      next->Draw(first);
  };

  void Draw() const
  {
    Draw(this);
  };

  friend void Optimize(objekt*& ob);
  friend void Append(objekt * (&ptr), objekt* ob);
};

typedef objekt* optr;

#endif

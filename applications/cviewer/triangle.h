#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "point.h"

class triangle: public point
{
public:
  triangle(vect *posp, vect *posp2, vect *posp3):
    point(posp), pos2(posp2), pos3(posp3), tex(NULL) {}
  triangle(vect *posp, vect *posp2, vect *posp3, texture *texp):
    point(posp), pos2(posp2), pos3(posp3), tex(texp) {}
  virtual ~triangle() {};

  static vector3 Normale(vector3 p1, vector3 p2, vector3 p3)
  {
    vector3 normal = (p2 - p1).cross(p3 - p1);
    normal.Normalize();
    return normal;
  }

protected:
  virtual void MyOptimize();
  virtual void myDraw() const;
  vect *pos2;
  vect *pos3;
  texture *tex;
  // cache für vorberechnete konstante Werte
  mutable vector3 v0;
  mutable vector3 v1;
  mutable vector3 v2;
  mutable vector3 normal;
};



#endif

#ifndef POLYGON_H
#define POLYGON_H

#include "point.h"

class polygon: public point
{
protected:
  vectlist* points;

// cache für vorberechnete konstante Normale
  vector3 normal;

  //  vector3 Normale() const;

  virtual void MyOptimize();

public:
  polygon(vectlist* vl, bool inverse): point(NULL), points(vl)
  {
    if (points->size() < 3)
      {
        cerror("Polygon mit weniger als 3 Punkten!");
      }
    if (inverse) points->Invert();
  };
  ~polygon() {};
protected:
  virtual void myDraw() const;
};

#endif

#include "Contur.h"

#ifndef _CONTURPOLY_
#define _CONTURPOLY_

namespace ice
{
  int conturFromPolygon(const Matrix& pl, Contur& c);

  bool pointInside(double x, double y, const Matrix& pl);

  bool pointInside(const Point& p, const Matrix& pl);

  bool pointInside(double x, double y, const Contur& c);

  bool pointInside(const Point& p, const Contur& c);
}
#endif

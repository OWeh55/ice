#include "point.h"

void point::myDraw() const
{
  glBegin(GL_POINTS);
  glVertex3fv(pos->Value().carray());
  glEnd();
}

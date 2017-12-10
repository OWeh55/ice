#include "defs.h"
#include "sphere.h"

void sphere::MyOptimize()
{
  DEBUGOUT("opt.sphere ");
  objekt::MyOptimize();
  optnumber(rad);
  DEBUGOUT(endl);
};

void sphere::myDraw() const
{
  const vector3& vpos = pos->Value();
  const float& arad = rad->Value();
  glPushMatrix();
  glTranslatef(vpos.X(), vpos.Y(), vpos.Z());
  glutSolidSphere(arad, minseg, minseg / 2);
  glPopMatrix();
}

#include "triangle.h"

void triangle::MyOptimize()
{
  DEBUGOUT("opt. triangle ");
  objekt::MyOptimize();
  optvector(pos2);
  optvector(pos3);
  constant = pos->Constant() && pos2->Constant() && pos3->Constant() ;
  if (constant)
    {
      v0 = pos->Value();
      v1 = pos2->Value();
      v2 = pos3->Value();
      normal = Normale(v0, v1, v2);
    }
  DEBUGOUT(endl);
};

void triangle::myDraw() const
{
  //glBegin(GL_POLYGON);
  glBegin(GL_TRIANGLES);

  if (!constant)
    {
      // Werte berechnen
      v0 = pos ->Value();
      v1 = pos2->Value();
      v2 = pos3->Value();

      normal = Normale(v0, v1, v2);
    }

  glNormal3fv(normal.carray());

  glVertex3fv(v0.carray());
  glVertex3fv(v1.carray());
  glVertex3fv(v2.carray());

  glEnd();
}

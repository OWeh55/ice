#include "triangle.h"
#include "polygon.h"

void polygon::MyOptimize()
{
  DEBUGOUT("opt. polygon ");
  objekt::MyOptimize();
  constant = points->Constant();
  if (constant)
    {
      DEBUGOUT("n");
      vector3 x0 = points->Value(0);
      vector3 x1 = points->Value(1);
      vector3 x2 = points->Value(2);
      normal = triangle::Normale(x0, x1, x2);
    }

  for (unsigned int i = 0; i < points->size(); i++)
    {
      if (points->operator[](i) != NULL)
        optvector(points->operator[](i));
    }

  DEBUGOUT(endl);
}

void polygon::myDraw() const
{
  // normale berechnen!

  const vector3 x0 = points->Value(0);
  const vector3 x1 = points->Value(1);
  const vector3 x2 = points->Value(2);

  glBegin(GL_POLYGON);

  if (constant)
    glNormal3fv(normal.carray());
  else
    {
      vector3 normale = (x1 - x0).cross(x2 - x1);
      normale.Normalize();
      glNormal3fv(normale.carray());
    }

//  cout << normale.X() << " " << normale.Y()<<" " << normale.Z()<<" " << endl;
//  printf("(%f,%f,%f)\n", normale.X(),normale.Y(),normale.Z());

  glVertex3fv(x0.carray());
  glVertex3fv(x1.carray());
  glVertex3fv(x2.carray());

  for (unsigned int i = 3; i < points->size(); i++)
    glVertex3fv(points->Value(i).carray());

  glEnd();
}

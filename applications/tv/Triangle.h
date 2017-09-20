#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector3.h"

class Triangle
{
protected:
  Vector3 p1, p2, p3;
  Vector3 normale;
  Vector3 color;
public:

  Triangle(const Vector3& v1, const Vector3& v2, const Vector3& v3,
           Vector3 color):
    p1(v1), p2(v2), p3(v3), color(color)
  {
    normale = (p2 - p1).cross(p3 - p1);
    normale.Normalize();
  }

  void draw() const
  {
    glColor3f(color.X(), color.Y(), color.Z());

    glBegin(GL_TRIANGLES);

    glNormal3fv(normale.carray());

    glVertex3fv(p1.carray());
    glVertex3fv(p2.carray());
    glVertex3fv(p3.carray());

    glEnd();
  }

  void shift(Vector3 s)
  {
    p1 += s;
    p2 += s;
    p3 += s;
  }

};

#endif

#include "tools.h"
#include "box.h"

void box::myDraw() const
{
  float x1 = pos->Value().X();
  float y1 = pos->Value().Y();
  float z1 = pos->Value().Z();

  float x2 = pos2->Value().X();
  float y2 = pos2->Value().Y();
  float z2 = pos2->Value().Z();

  sort(x1, x2);
  sort(y1, y2);
  sort(z1, z2);

  glBegin(GL_POLYGON);
  glNormal3fv(vector3(0, 0, -1).carray());
  glTexCoord2f(x1, y2);
  glVertex3fv(vector3(x1, y2, z1).carray());
  glTexCoord2f(x2, y2);
  glVertex3fv(vector3(x2, y2, z1).carray());
  glTexCoord2f(x2, y1);
  glVertex3fv(vector3(x2, y1, z1).carray());
  glTexCoord2f(x1, y1);
  glVertex3fv(vector3(x1, y1, z1).carray());
  glEnd();

  glBegin(GL_POLYGON);
  glNormal3fv(vector3(0, 0, 1).carray());
  glVertex3fv(vector3(x1, y1, z2).carray());
  glVertex3fv(vector3(x2, y1, z2).carray());
  glVertex3fv(vector3(x2, y2, z2).carray());
  glVertex3fv(vector3(x1, y2, z2).carray());
  glEnd();

//------------------------------------------
  glBegin(GL_POLYGON);
  glNormal3fv(vector3(-1, 0, 0).carray());
  glVertex3fv(vector3(x1, y1, z2).carray());
  glVertex3fv(vector3(x1, y2, z2).carray());
  glVertex3fv(vector3(x1, y2, z1).carray());
  glVertex3fv(vector3(x1, y1, z1).carray());
  glEnd();

  glBegin(GL_POLYGON);
  glNormal3fv(vector3(1, 0, 0).carray());
  glVertex3fv(vector3(x2, y1, z1).carray());
  glVertex3fv(vector3(x2, y2, z1).carray());
  glVertex3fv(vector3(x2, y2, z2).carray());
  glVertex3fv(vector3(x2, y1, z2).carray());
  glEnd();
//------------------------------------------
  glBegin(GL_POLYGON);
  glNormal3fv(vector3(0, -1, 0).carray());
  glVertex3fv(vector3(x1, y1, z1).carray());
  glVertex3fv(vector3(x2, y1, z1).carray());
  glVertex3fv(vector3(x2, y1, z2).carray());
  glVertex3fv(vector3(x1, y1, z2).carray());
  glEnd();

  glBegin(GL_POLYGON);
  glNormal3fv(vector3(0, 1, 0).carray());
  glVertex3fv(vector3(x1, y2, z2).carray());
  glVertex3fv(vector3(x2, y2, z2).carray());
  glVertex3fv(vector3(x2, y2, z1).carray());
  glVertex3fv(vector3(x1, y2, z1).carray());
  glEnd();
}

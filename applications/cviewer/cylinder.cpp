#include "defs.h"
#include "numbase.h"
#include "cylinder.h"

void cylinder::myDraw() const
{
  int i;
  int nSegments = (int)(rad->Value()) * 10;
  if (nSegments < minseg) nSegments = minseg;
  /*
  float dfi = 360.0 / nSegments;
  float fi1;
  */
  float sinfi, cosfi, sindfi, cosdfi;
  CSInit(nSegments, sinfi, cosfi, sindfi, cosdfi);

  float arad = rad->Value();
  vector3 apos1 = pos->Value();
  vector3 apos2 = pos2->Value();
  vector3 achse = Normalize(apos1 - apos2);
  vector3 norm1 = achse.cross(vector3(0, 0, 1));
  if (norm1.Length() == 0.0)
    norm1 = achse.cross(vector3(1, 0, 0));
  norm1.Normalize();
  vector3 norm2 = Normalize(achse.cross(norm1));

  glBegin(GL_TRIANGLE_FAN);

  glNormal3fv(achse.carray());
  glVertex3fv(apos1.carray());
  for (i = 0; i <= nSegments; i++)
    {
      vector3 punkt = (-norm1 * sinfi + norm2 * cosfi) * arad + apos1;
      glVertex3fv(punkt.carray());
      CSNext(sinfi, cosfi, sindfi, cosdfi);
    }
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glNormal3fv((-achse).carray());
  glVertex3fv(apos2.carray());
  CSReInit(sinfi, cosfi);
  for (i = 0; i <= nSegments; i++)
    {
      vector3 punkt = (norm1 * sinfi + norm2 * cosfi) * arad + apos2;
      glVertex3fv(punkt.carray());
      CSNext(sinfi, cosfi, sindfi, cosdfi);
    }
  glEnd();

  glBegin(GL_TRIANGLE_STRIP);
  CSReInit(sinfi, cosfi);
  for (i = 0; i <= nSegments; i++)
    {
      vector3 normale = norm1 * sinfi + norm2 * cosfi;
      glNormal3fv(normale.carray());

      vector3 punkt = (norm1 * sinfi + norm2 * cosfi) * arad + apos2;
      glVertex3fv(punkt.carray());

      punkt = (norm1 * sinfi + norm2 * cosfi) * arad + apos1;
      glVertex3fv(punkt.carray());
      CSNext(sinfi, cosfi, sindfi, cosdfi);
    }
  glEnd();
}

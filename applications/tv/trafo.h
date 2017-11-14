#ifndef TRAFO_H
#define TRAFO_H

#include "vector3.h"
#include "matrix3.h"

#include <GL/glut.h>

class Trafo
{
private:
  Matrix3 m;
  Vector3 s;

public:
  Trafo(): m(), s() {}

  Vector3 Transform(const Vector3& p) const;

  void clear()
  {
    m.clear();
    s.clear();
  }

  void glGet(GLenum which = GL_MODELVIEW_MATRIX);
  //  void glGet();
  void glSet(GLenum which = GL_MODELVIEW_MATRIX) const;

  // Transformationen erzeugen neue Trafo aus alter trafo mit
  // "angehängter" Transformation
  // Verschiebung
  void Shift(float dx, float dy, float dz);
  void Shift(const Vector3& d);

  // Rotation um Achse durch Ursprung
  void Rotate(const Vector3& dir, float phi);

  // Rotation um Achse durch o
  void Rotate(const Vector3& o, const Vector3& dir, float phi);

  // Verketten
  void Append(const Trafo& tr);

  std::string toString() const;
  friend std::string ToString(const Trafo& tr);
};

#endif

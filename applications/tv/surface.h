#ifndef SURFACE_H
#define SURFACE_H

#include <string>
#include <GL/glut.h>

#include "vector3.h"

class Surface
{
private:
  Vector3 color;
protected:
  Surface(const Vector3& col): color(col) {};
  void SetTexture()
  {
    glColor3f(color.X(), color.Y(), color.Z());
  }
};

class Texture
{
private:
  GLuint handle;
  std::string filename;
  bool wrap;
  int width, height;
public:
  Texture(const std::string& fn): filename(fn), wrap(true) {}
  void Make();
  void Select()
  {
    glBindTexture(GL_TEXTURE_2D, handle);
  }
};

#endif

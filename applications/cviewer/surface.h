#ifndef SURFACE_H
#define SURFACE_H

#include "vector3.h"
#include <GL/glut.h>

class Surface
{
private:
  vector3 color;
protected:
  Surface(const vector3 &col): color(col) {};
  void SetTexture()
  {
    glColor3f(color.X(), color.Y(), color.Z());
  }
};

class Texture
{
private:
  GLuint handle;
  string filename;
  bool wrap;
  int width, height;
public:
  Texture(const string &fn): filename(fn), wrap(true) {}
  void Make();
  void Select()
  {
    glBindTexture(GL_TEXTURE_2D, handle);
  }
};

#endif

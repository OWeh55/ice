#include "cviewer.h"
#include "surface.h"
#include "pbmio.h"

void Texture::Make()
{
  glGenTextures(1, &handle);
  glBindTexture(GL_TEXTURE_2D, handle);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // when texture area is small, bilinear filter the closest mipmap
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  // when texture area is large, bilinear filter the original
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // the texture wraps over at the edges (repeat)
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);

  int maxval, nr;
  if (!InfPBMFile(filename, width, height, maxval, nr))
    {
      cerror("Kann Datei " + filename + " nicht öffnen");
    }
  unsigned char *data = new unsigned char [width * height * 3];
  ReadPBM(filename, data);
  // build our texture mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
                    GL_RGB, GL_UNSIGNED_BYTE, data);

  delete data;
}

#ifndef DISPLAY3D_RGB
#define DISPLAY3D_RGB

#include <vector>
#include <image.h>
#include "image3d.h"

class Display3dRGB
{
public:
  enum plane_t { xy_plane, xz_plane, zy_plane };

  Display3dRGB(Image3d<Image> &series1, Image3d<Image> &series2, Image3d<Image> &series3, const string &title = "");
  ~Display3dRGB();

  void getSize(int &xa, int &ya, int &za) const
  {
    xa = xs;
    ya = ys;
    za = zs;
  }

  void Show(bool on = true)
  {
    if (on)
      ice::Show(_RGB, red, green, blue);
    else
      ice::Show(OFF, red);
  }

  void Cursor(bool on = true)
  {
    cursor = on;
    Update(last);
  }

  void Active(bool on = true)
  {
    active = on;
    Update(last);
  }

  void Update();
  void Update(int xa, int ya, int za);
  void Update(IVector3d p);

  bool getMouse(int &x, int &y, int &z, plane_t &plane) const;
  bool getMouse(int &x, int &y, int &z) const;
  bool getMouse(IVector3d &p, plane_t &plane) const;
  bool getMouse(IVector3d &p) const;

  string getTitle() const
  {
    return title;
  }

  Image3d<Image> *getImg(int n = 0)
  {
    return img3d[n];
  }
  const Image3d<Image> *getImg(int n = 0) const
  {
    return img3d[n];
  }

  IVector3d lastCursor() const
  {
    return last;
  }

private:

  void init();

  static const int gap = 10;

  void Line(int x1, int y1, int x2, int y2, int cred, int cgreen, int cblue);
  void Line(int x1, int y1, int x2, int y2, int c, Image img[3]);

  Image3d<Image> *img3d[3];

  bool owner;

  string title;

  Image red;
  Image green;
  Image blue;

  // Grayvalue subimages
  Image gxy[3];
  Image gxz[3];
  Image gzy[3];

  // size of visible image
  int xs, ys, zs;

  // letzte visualisierungswerte
  IVector3d last;

  bool cursor;
  bool active;
};
#endif

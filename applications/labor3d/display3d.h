#ifndef DISPLAY3D
#define DISPLAY3D

#include <vector>
#include <image.h>
#include "image3d.h"

class Display3d
{
public:
  enum plane_t { xy_plane, xz_plane, zy_plane };

  Display3d(Image3d<Image>& img, const string& title = "");
  Display3d(Image3d<Image>* img, const string& title = "");
  ~Display3d();

  void getSize(int& xa, int& ya, int& za) const
  {
    xa = xs;
    ya = ys;
    za = zs;
  }

  void Show(bool on = true)
  {
    if (on)
      ice::Show(OVERLAY, gray, overlay);
    else
      ice::Show(OFF, gray);
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

  bool getMouse(int& x, int& y, int& z, plane_t& plane) const;
  bool getMouse(int& x, int& y, int& z) const;
  bool getMouse(IVector3d& p, plane_t& plane) const;
  bool getMouse(IVector3d& p) const;

  void setMarkerLevel(int val)
  {
    markerlevel = val;
  }

  string getTitle() const
  {
    return title;
  }

  Image3d<Image>* getImg()
  {
    return img3d;
  }
  const Image3d<Image>* getImg() const
  {
    return img3d;
  }

  IVector3d lastCursor() const
  {
    return last;
  }

private:

  void init();

  static const int gap = 10;


  Image3d<Image>* img3d;
  bool owner;

  string title;

  Image gray;
  Image overlay;
  // Grayvalue subimages
  Image gxy;
  Image gxz;
  Image gzy;

  // Overlay subimages
  Image oxy;
  Image oxz;
  Image ozy;

  // size of visible image
  int xs, ys, zs;
  int maxval;

  // letzte visualisierungswerte
  IVector3d last;

  int markerlevel;

  bool cursor;
  bool active;
};
#endif

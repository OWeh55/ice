#include "display3drgb.h"

Display3dRGB::Display3dRGB(Image3d<Image> &series1, Image3d<Image> &series2, Image3d<Image> &series3,
                           const string &title):
  owner(false), title(title), cursor(true), active(false)
{
  img3d[0] = &series1;
  img3d[1] = &series2;
  img3d[2] = &series3;
  init();
}

Display3dRGB::~Display3dRGB()
{
  if (owner)
    {
    delete [] img3d[0];
    delete [] img3d[1];
    delete [] img3d[2];
    }
}

void Display3dRGB::init()
{
  double xsi, ysi, zsi;
  img3d[0]->getSizeD(xsi, ysi, zsi);

  //  cout << xsi << " " << ysi << " " << zsi << endl;

  xs = RoundInt(xsi);
  ys = RoundInt(ysi);
  zs = RoundInt(zsi);

  int length = title.length();
  if (length > 40)
    length = 40;
  string stitle = title.substr(title.length() - length);

  // Display images with
  // -------------
  // | xy || zy  |
  // =====
  // | xz |
  // ------
  //  4 Borders
  //
  red = NewImg(xs + zs + 4, ys + zs + 4, 255);
  red.set(255);
  green = NewImg(xs + zs + 4, ys + zs + 4, 255);
  green.set(255);
  blue = NewImg(xs + zs + 4, ys + zs + 4, 255);
  blue.set(255);

  ice::Show(_RGB, red, green, blue, stitle);

  Window wxy(1, 1, xs, ys);
  gxy[0] = red(wxy);
  gxy[1] = green(wxy);
  gxy[2] = blue(wxy);

  Window wxz(1, ys + 3, xs, ys + zs + 2);
  gxz[0] = red(wxz);
  gxz[1] = green(wxz);
  gxz[2] = blue(wxz);

  Window wzy(xs + 3, 1, xs + zs + 2, ys);
  gzy[0] = red(wzy);
  gzy[1] = green(wzy);
  gzy[2] = blue(wzy);

  Update(xs / 2, ys / 2, zs / 2);
}

void Display3dRGB::Update()
{
  //  cout << "update last"<<endl;
  Update(last);
}

void Display3dRGB::Update(int xa, int ya, int za)
{
  //  cout << "update x,y,z"<<endl;
  Update(IVector3d(xa, ya, za));
}

void Display3dRGB::Line(int x1, int y1, int x2, int y2, int cred, int cgreen, int cblue)
{
  ice::Line(x1, y1, x2, y2, cred, red);
  ice::Line(x1, y1, x2, y2, cgreen, green);
  ice::Line(x1, y1, x2, y2, cblue, blue);
}

void Display3dRGB::Line(int x1, int y1, int x2, int y2, int c, Image img[3])
{
  int r = 0, g = 0, b = 0;
  switch (c)
    {
    case 1:
      r = 0;
      g = 255;
      b = 255;
      break;
    case 2:
      r = 255;
      g = 0;
      b = 255;
      break;
    case 3:
      r = 255;
      g = 255;
      b = 0;
      break;
    }

  ice::Line(x1, y1, x2, y2, r, img[0]);
  ice::Line(x1, y1, x2, y2, g, img[1]);
  ice::Line(x1, y1, x2, y2, b, img[2]);
}

void Display3dRGB::Update(IVector3d p)
{
  //  cout << "update Ivector3d"<<endl;
  // cout << p.x << "," << p.y << "," << p.z << endl;
  // cout << xs << "," << ys << "," << zs << endl;
  // cout << "        " << img3d.sizeX() << "," << img3d.sizeY() << "," << img3d.sizeZ() << endl;

  p.x = limited(p.x, 0, xs - 1);
  p.y = limited(p.y, 0, ys - 1);
  p.z = limited(p.z, 0, zs - 1);

  last = p;

  for (int ch = 0; ch < 3; ch++)
    {
      //  cout << "xy" << endl;
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          {
            int g = img3d[ch]->getValue(x, y, p.z);
            PutVal(gxy[ch], x, y, g);
          }

      //  cout << "xz" << endl;
      for (int z = 0; z < zs; z++)
        for (int x = 0; x < xs; x++)
          {
            int g = img3d[ch]->getValue(x, p.y, z);
            PutVal(gxz[ch], x, z, g);
          }

      //  cout << "zy" << endl;
      for (int z = 0; z < zs; z++)
        for (int y = 0; y < ys; y++)
          {
            int g = img3d[ch]->getValue(p.x, y, z);
            PutVal(gzy[ch], z, y, g);
          }
    }

  // frames
  int r = 255, g = 255, b = 0;
  if (active)
    {
      r = 0;
      g = 255;
      b = 255;
    }
  // xy
  Line(0, 0, xs + 1, 0, r, g, b);
  Line(0, ys + 1, xs + 1, ys + 1, r, g, b);
  Line(0, 0, 0, ys + 1, r, g, b);
  Line(xs + 1, 0, xs + 1, ys + 1, r, g, b);
  // zy
  Line(xs + 2, 0, xs + zs + 3, 0, r, g, b);
  Line(xs + 2, ys + 1, xs + zs + 3, ys + 1, r, g, b);
  Line(xs + 2, 0, xs + 2, ys + 1, r, g, b);
  Line(xs + zs + 3, 0, xs + zs + 3, ys + 1, r, g, b);
  // xz
  Line(0, ys + 2, xs + 1, ys + 2, r, g, b);
  Line(0, ys + zs + 3, xs + 1, ys + zs + 3, r, g, b);
  Line(0, ys + 2, 0, ys + zs + 3, r, g, b);
  Line(xs + 1, ys + 2, xs + 1, ys + zs + 3, r, g, b);

  if (cursor)
    {
      Line(0, p.y, p.x - gap, p.y, 1, gxy);
      Line(p.x + gap, p.y, xs - 1, p.y, 1, gxy);

      Line(p.x, 0, p.x, p.y - gap, 2, gxy);
      Line(p.x, p.y + gap, p.x, ys - 1, 2, gxy);

      Line(p.x, 0, p.x, p.z - gap, 2, gxz);
      Line(p.x, p.z + gap, p.x, zs - 1, 2, gxz);

      Line(0, p.z, p.x - gap, p.z, 3, gxz);
      Line(p.x + gap, p.z, xs - 1, p.z, 3, gxz);

      Line(0, p.y, p.z - gap, p.y, 1, gzy);
      Line(p.z + gap, p.y, zs - 1, p.y, 1, gzy);

      Line(p.z, 0, p.z, p.y - gap, 3, gzy);
      Line(p.z, p.y + gap, p.z, ys - 1, 3, gzy);
    }
}

bool Display3dRGB::getMouse(IVector3d &p) const
{
  return getMouse(p.x, p.y, p.z);
}

bool Display3dRGB::getMouse(int &x, int &y, int &z) const
{
  plane_t plane;
  return getMouse(x, y, z, plane);
}

bool Display3dRGB::getMouse(IVector3d &p, plane_t &plane) const
{
  return getMouse(p.x, p.y, p.z, plane);
}

bool Display3dRGB::getMouse(int &x, int &y, int &z, plane_t &plane) const
{
  int display_x, display_y;
  int mousestat = Mouse(red, display_x, display_y);
  //  cout << mousestat<< endl;
  bool pressed = mousestat & M_LEFT_PRESSED;
  if (pressed)
    {
      bool left = display_x < xs;
      bool top = display_y < ys;
      if (left)
        {
          if (top)
            {
              plane = xy_plane;
              x = display_x;
              y = display_y;
            }
          else
            {
              plane = xz_plane;
              x = display_x;
              z = display_y - ys;
            }
        }
      else
        {
          if (top)
            {
              plane = zy_plane;
              z = display_x - xs;
              y = display_y;
            }
        }
    }
  return pressed;
}

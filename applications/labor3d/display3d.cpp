#include "display3d.h"

Display3d::Display3d(Image3d<Image> &series, const string &title):
  img3d(&series), owner(false), title(title), markerlevel(0), cursor(true), active(false)
{
  init();
}

Display3d::Display3d(Image3d<Image> *series, const string &title):
  img3d(series), owner(true), title(title), markerlevel(0), cursor(true), active(false)
{
  init();
}

Display3d::~Display3d()
{
  if (owner)
    delete img3d;
}

void Display3d::init()
{
  double xsi, ysi, zsi;
  img3d->getSizeD(xsi, ysi, zsi);

  //  cout << xsi << " " << ysi << " " << zsi << endl;

  xs = RoundInt(xsi);
  ys = RoundInt(ysi);
  zs = RoundInt(zsi);
  maxval = img3d->maxval;

  int length = title.length();
  if (length > 40)
    length = 40;
  string stitle = title.substr(title.length() - length);

  // Display images with
  // -------------
  // | xy || zy  |
  // ===== -------
  // | xz |
  // ------

  gray = NewImg(xs + zs + 4, ys + zs + 4, maxval);
  gray.set(maxval);
  overlay = NewImg(xs + zs + 4, ys + zs + 4, maxval);
  clearImg(overlay);

  ice::Show(OVERLAY, gray, overlay, stitle);

  Window wxy(1, 1, xs, ys);
  gxy = gray(wxy);
  oxy = overlay(wxy);

  Window wxz(1, ys + 3, xs, ys + zs + 2);
  gxz = gray(wxz);
  oxz = overlay(wxz);

  Window wzy(xs + 3, 1, xs + zs + 2, ys);
  gzy = gray(wzy);
  ozy = overlay(wzy);

  //  GetChar();

  Update(xs / 2, ys / 2, zs / 2);
}

void Display3d::Update()
{
  //  cout << "update last"<<endl;
  Update(last);
}

void Display3d::Update(int xa, int ya, int za)
{
  //  cout << "update x,y,z"<<endl;
  Update(IVector3d(xa, ya, za));
}

void Display3d::Update(IVector3d p)
{
  //  cout << "update Ivector3d"<<endl;
  // cout << p.x << "," << p.y << "," << p.z << endl;
  // cout << xs << "," << ys << "," << zs << endl;
  // cout << "        " << img3d.sizeX() << "," << img3d.sizeY() << "," << img3d.sizeZ() << endl;

  p.x = limited(p.x, 0, xs - 1);
  p.y = limited(p.y, 0, ys - 1);
  p.z = limited(p.z, 0, zs - 1);

  last = p;

  clearImg(overlay);

  //  cout << "xy" << endl;
  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      {
        int g = img3d->getValue(x, y, p.z);
        PutVal(gxy, x, y, g);
        if (g < markerlevel)
          PutVal(oxy, x, y, 4);
      }

  //  cout << "xz" << endl;
  for (int z = 0; z < zs; z++)
    for (int x = 0; x < xs; x++)
      {
        int g = img3d->getValue(x, p.y, z);
        //  cout << x << "," << z << endl;
        //  cout << gxz->xsize << "," << gxz->ysize << endl;
        PutVal(gxz, x, z, g);
        if (g < markerlevel)
          PutVal(oxz, x, z, 4);
      }

  //  cout << "zy" << endl;
  for (int z = 0; z < zs; z++)
    for (int y = 0; y < ys; y++)
      {
        int g = img3d->getValue(p.x, y, z);
        PutVal(gzy, z, y, g);
        if (g < markerlevel)
          PutVal(ozy, z, y, 4);
      }

  // frames
  int color = 3;
  if (active) color = 1;
  // xy
  Line(0, 0, xs + 1, 0, color, overlay);
  Line(0, ys + 1, xs + 1, ys + 1, color, overlay);
  Line(0, 0, 0, ys + 1, color, overlay);
  Line(xs + 1, 0, xs + 1, ys + 1, color, overlay);
  // zy
  Line(xs + 2, 0, xs + zs + 3, 0, color, overlay);
  Line(xs + 2, ys + 1, xs + zs + 3, ys + 1, color, overlay);
  Line(xs + 2, 0, xs + 2, ys + 1, color, overlay);
  Line(xs + zs + 3, 0, xs + zs + 3, ys + 1, color, overlay);
  // xz
  Line(0, ys + 2, xs + 1, ys + 2, color, overlay);
  Line(0, ys + zs + 3, xs + 1, ys + zs + 3, color, overlay);
  Line(0, ys + 2, 0, ys + zs + 3, color, overlay);
  Line(xs + 1, ys + 2, xs + 1, ys + zs + 3, color, overlay);

  if (cursor)
    {
      Line(0, p.y, p.x - gap, p.y, 1, oxy);
      Line(p.x + gap, p.y, xs - 1, p.y, 1, oxy);

      Line(p.x, 0, p.x, p.y - gap, 2, oxy);
      Line(p.x, p.y + gap, p.x, ys - 1, 2, oxy);

      Line(p.x, 0, p.x, p.z - gap, 2, oxz);
      Line(p.x, p.z + gap, p.x, zs - 1, 2, oxz);

      Line(0, p.z, p.x - gap, p.z, 3, oxz);
      Line(p.x + gap, p.z, xs - 1, p.z, 3, oxz);

      Line(0, p.y, p.z - gap, p.y, 1, ozy);
      Line(p.z + gap, p.y, zs - 1, p.y, 1, ozy);

      Line(p.z, 0, p.z, p.y - gap, 3, ozy);
      Line(p.z, p.y + gap, p.z, ys - 1, 3, ozy);
    }
}

bool Display3d::getMouse(IVector3d &p) const
{
  return getMouse(p.x, p.y, p.z);
}

bool Display3d::getMouse(int &x, int &y, int &z) const
{
  plane_t plane;
  return getMouse(x, y, z, plane);
}

bool Display3d::getMouse(IVector3d &p, plane_t &plane) const
{
  return getMouse(p.x, p.y, p.z, plane);
}

bool Display3d::getMouse(int &x, int &y, int &z, plane_t &plane) const
{
  int display_x, display_y;
  int mousestat = Mouse(gray, display_x, display_y);
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

// tests zu fourierdeskriptoren und conturen

#include <image.h>

#include <fourierdescriptor.h>

// interpolation of polygon
void interpolate(vector<Point> points, vector<Point>& ip, double delta)
{
  int nPoints = points.size();
  ip.clear();
  for (int i = 0; i < nPoints; i++)
    {
      Point p1 = points[i];
      int i2 = (i + 1) % nPoints;
      Point p2 = points[i2];
      Point dp = p2 - p1;
      double len1 = dp.length();
      int nSteps = len1 / delta;
      if (nSteps < 1)
        nSteps = 1;
      dp = dp / nSteps;
      for (int k = 0; k < nSteps; k++)
        ip.push_back(p1 + k * dp);
    }
}

// draw point list (polygon)
void draw(const vector<Point>& points, Image& m, int xi, int yi)
{
  double rMax = 0.1;
  for (auto p : points)
    if (p.r() > rMax)
      rMax = p.r();

  double fac = min(m.xsize, m.ysize) * 0.95 / 2.0 / rMax ;

  Point p0(m.xsize / 2, m.ysize / 2);

  Marker(1, points[0]*fac + p0, 3, 5, m);
  Point pa = points[0];
  for (unsigned int i = 1; i < points.size(); i++)
    {
      Marker(1, points[i]*fac + p0, 2, 5, m);
      Line(points[i]*fac + p0, pa * fac + p0, 1, m);
      pa = points[i];
    }
  Line(points[0]*fac + p0, pa * fac + p0, 2, m);
}

// draw point list from FD
void drawFD(const vector<Point>& fd, Image& m, int xi, int yi)
{
  vector<Point> p;
  computeFourier(fd, p, false);
  draw(p, m, xi, yi);
}

void print(const vector<Point>& fk)
{
  int i0 = fk.size() / 2;
  for (unsigned int i = 0; i < fk.size(); i++)
    cout << (i - i0) << " " << fk[i] << endl ;
}

int main(int argc, char** argv)
{
  int xs = 900;
  int ys = 900;

  Image gray;
  gray.create(xs, ys, 255);
  Image marker;
  marker.create(xs, ys, 16);
  Show(OVERLAY, gray, marker);

  vector<Point> p;

#if 0
  for (int g = 0; g < 360; g += 15)
    {
      double fi = g * 2.0 * M_PI / 360;
      double r = 0.9;
      if (g % 2 == 0)
        r = 0.2;
      p.push_back(Point(r * cos(fi), r * sin(fi)));
    }
  interpolate(p, p, 0.02);
#endif
#if 0
  p.push_back(Point(0, -0.7));
  p.push_back(Point(0.2, -0.2));
  p.push_back(Point(0.7, 0.0));
  p.push_back(Point(0.2, 0.2));
  p.push_back(Point(0.0, 0.7));
  p.push_back(Point(-0.2, 0.2));
  p.push_back(Point(-0.7, 0.0));
  p.push_back(Point(-0.2, -0.2));
  interpolate(p, p, 0.07);
#endif

#if 0
  const int nS = 99;
  for (int i = 0; i < nS; i++)
    {
      double phi = 2 * M_PI / nS * i;
      p.push_back(Point(0.7 * cos(phi), 0.7 * sin(phi)));
    }
#endif

#if 0
  p.push_back(Point(-1.0, -0.9));
  p.push_back(Point(-0.1, 0.0));
  p.push_back(Point(-0.3, 0.2));
  p.push_back(Point(0.9, 0.9));
  p.push_back(Point(0.2, -0.3));
  p.push_back(Point(0.0, -0.1));
  p.push_back(Point(-0.9, -1.0));
  interpolate(p, p, 0.2);
#endif

#if 0
  Contur c = SelContur(marker, true);
  vector<Point> praw;
  c.getPoints(praw);
  for (unsigned int i = 0; i < praw.size(); i++)
    {
      praw[i] -= Point(xs / 2, ys / 2);
      praw[i] *= 4.0 / max(xs, ys) ;
      if (i % 3 == 0)
        p.push_back(praw[i]);
    }
  marker.set(0);
#endif
  vector<Point> fd(1001, Point(0, 0));
  int i0 = fd.size() / 2;
  // set fourier descriptors
  fd[i0 + 1] = Point(2, 0);
  fd[i0 + 4] = Point(0.7, 0);
  //  fd[i0+2]=Point(0.5,0);
  //  fd[i0-1]=Point(-1,0.1);

  computeFourier(fd, p, false);

  draw(p, marker, 0, 0);

  GetChar();
  return OK;
}


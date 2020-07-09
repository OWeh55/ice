//

#include <image.h>

#include <fourierdescriptor.h>

void draw(const vector<Point>& points, Image& m)
{
  Point p0(m.xsize / 2, m.ysize / 2);
  Marker(1, points[0] + p0, 1, 5, m);
  Point pa = points[0];
  for (unsigned int i = 1; i < points.size(); i++)
    {
      Marker(1, points[i] + p0, 3, 5, m);
      Line(points[i] + p0, pa + p0, 1, m);
      pa = points[i];
    }
}

void print(const vector<Point>& fk)
{
  for (auto k : fk)
    cout << k << " " ;
  cout << endl;
}

int main(int argc, char** argv)
{
  int xs = 600;
  int ys = 600;

  Image gray;
  gray.create(xs, ys, 255);
  Image marker;
  marker.create(xs, ys, 16);
  Show(OVERLAY, gray, marker);

  vector<Point> p;
#if 0
  p.push_back(Point(0, -0.3 * ys));
  p.push_back(Point(0.1 * xs, -0.1 * ys));
  p.push_back(Point(0.3 * xs, 0.0 * ys));
  p.push_back(Point(0.1 * xs, 0.1 * ys));
  p.push_back(Point(0.0 * xs, 0.3 * ys));
  p.push_back(Point(-0.1 * xs, 0.1 * ys));
  p.push_back(Point(-0.3 * xs, 0.0 * ys));
  p.push_back(Point(-0.1 * xs, -0.1 * ys));
#else
  Contur c = SelContur(marker, true);
  c.getPoints(p);
  for (unsigned int i = 0; i < p.size(); i++)
    p[i] -= Point(xs / 2, ys / 2);
  marker.set(0);
#endif

  draw(p, marker);

  GetChar();

  vector<Point> fk;
  computeFourier(p, fk, true);
  print(fk);

  GetChar();
  vector<Point> fd;
  normalizeFDIShiftRotation(fk, fd);
  print(fd);

  computeFourier(fd, p, false);

  draw(p, marker);

  GetChar();
  return OK;
}

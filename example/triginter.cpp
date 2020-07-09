//
#include <image.h>

#include <fourierdescriptor.h>

#define nParts 2

void interpolate(vector<Point> points, double delta, vector<Point>& ip)
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

void interpolatePL(const vector<Point>& p1,
                   int newsize,
                   vector<Point>& p2)
{
  vector<Point> fk1;
  computeFourier(p1, fk1, true);
  vector<Point> fk2;
  zeroPaddingFD(fk1, newsize, fk2);
  computeFourier(fk2, p2, false);
}

void draw(const vector<Point>& points, Image& m, int xi, int yi)
{
  double fac = min(m.xsize, m.ysize) / (4 * nParts);

  Point p0((xi * 2 + 1)*m.xsize / (2 * nParts),
           (yi * 2 + 1)* m.ysize / (2 * nParts));

  Marker(1, points[0]*fac + p0, 3, 5, m);
  Point pa = points[0];
  for (unsigned int i = 1; i < points.size(); i++)
    {
      Marker(1, points[i]*fac + p0, 2, 5, m);
      Line(points[i]*fac + p0, pa * fac + p0, 1, m);
      pa = points[i];
    }
  Line(points[0]*fac + p0, pa * fac + p0, 1, m);
}

void drawFD(const vector<Point>& fd, Image& m, int xi, int yi)
{
  vector<Point> p;
  computeFourier(fd, p, false);
  draw(p, m, xi, yi);
}

void print(const vector<Point>& fk)
{
  for (auto k : fk)
    cout << k << " " ;
  cout << endl;
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

  p.push_back(Point(0, -0.7));
  p.push_back(Point(0.2, -0.2));
  p.push_back(Point(0.7, 0.0));
  p.push_back(Point(0.2, 0.2));
  p.push_back(Point(0.0, 0.7));
  p.push_back(Point(-0.2, 0.2));
  p.push_back(Point(-0.7, 0.0));
  p.push_back(Point(-0.2, -0.2));

  for (auto& point : p)
    point = point * 2;

  interpolate(p, 0.4, p);

  draw(p, marker, 0, 0);
  draw(p, marker, 0, 1);

  vector<Point> p2;
  interpolatePL(p, p.size() * 4, p2);

  draw(p2, marker, 1, 0);
  draw(p2, marker, 0, 1);

  GetChar();
  return OK;
}

/*
 * ecfit.cpp
 *
 * test für fit von kreis und ellipse
 *
 */

#include <cmath>

#include <image.h>

void mkUnityCircle(vector<Point>& pl, int nr, double noise)
{
  pl.clear();
  for (int i = 0; i < nr; i++)
    {
      double a = rand() * 0.0001;

      // we add equally distributed noise to the ideal point coordinates
      double x = sin(a) + ((rand() % 100) - 50) * 0.001 * noise;
      double y = cos(a) + ((rand() % 100) - 50) * 0.001 * noise;

      pl.push_back(ice::Point(x, y));
    }
}

int usage(const string& pn)
{
  cout << pn << " - test for fit of circle and ellipse" << endl;
  cout << "Usage: " << pn << " <options> filename1 filename2" << endl;
  cout << "options:" << endl;
  cout << "-h      this help" << endl;
  cout << "-x x    center of circle or ellipse" << endl;
  cout << "-y y    center of circle or ellipse" << endl;
  cout << "-r rad  Radius of circle, semi-major axis of ellipse" << endl;
  cout << "-R axis semi-minor axis of ellipse" << endl;
  cout << "-p phi  angle of major axis of ellipse" << endl;
  cout << "-N val  Noise of point coordinates" << endl;
  cout << "-t type Type of figure" << endl;
  cout << "         type=c Circle" << endl;
  cout << "         type=e Ellipse" << endl;
  cout << "-n      Number of points" << endl;
  exit(1);
}

void ShowPoints(Image& img, const vector<Point>& pl, int value)
{
  for (unsigned int i = 0; i < pl.size(); i++)
    {
      if (img.Inside(pl[i]))
        PutVal(img, pl[i].x, pl[i].y, value);
    }
}

int main(int argc, char** argv)
{
  Image img(1200, 800, 255);
  Show(ON, img);
  ClearImg(img);
  int npoints = 555;
  double xc = 300.0;
  double yc = 321.0;
  double rad = 123.0;
  double rad2 = 55.0;
  double phi = 45;

  double noise = 0.7;
  int rc;
  char type = 'c';

  while ((rc = getopt(argc, argv, "x:y:r:R:n:t:p:N:h")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          xc = atof(optarg);
          break;
        case 'y':
          yc = atof(optarg);
          break;
        case 'r':
          rad = atof(optarg);
          break;
        case 'R':
          rad2 = atof(optarg);
          break;
        case 'p':
          phi = atof(optarg);
          break;
        case 'N':
          noise = atof(optarg);
          break;
        case 'n':
          npoints = atoi(optarg);
          break;
        case 't':
          type = optarg[0];
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }
  // create new point list
  vector<Point> points;

  switch (type)
    {
    case 'c':
    {
      Trafo tr;
      tr.Scale(0, 0, rad);
      tr.Shift(xc, yc);
      // sample circle points
      mkUnityCircle(points, npoints, noise);
      // transform to desired circle
      Transform(tr, points);

      ShowPoints(img, points, 127);

      cout << "Created pointlist with " << npoints << " points ..." << endl;
      cout << "for circle at (" << xc << "," << yc << ") with radius " << rad << endl;
      cout << "Fitting ..." << endl;
      Circle c = FitCircle(points);
      Draw(c, img, 255, -1);
      cout << "Fitted circle at (" << c.getPos().x << "," << c.getPos().y << ") with radius " << c.getR() << endl;
    }
    break;

    case 'e':
    {
      Trafo tr;
      tr.Scale(0, 0, rad, rad2);
      tr.Rotate(0, 0, Arcus(phi));
      tr.Shift(xc, yc);
      // sample circle points
      mkUnityCircle(points, npoints, noise);
      // transform to ellipse
      Transform(tr, points);

      ShowPoints(img, points, 127);

      cout << "Created pointlist with " << npoints << " points ..." << endl;
      cout << "for ellipse at (" << xc << "," << yc << ") with axis " << rad << ", " << rad2 << endl;
      cout << "Fitting ..." << endl;
      Ellipse e = FitEllipse(points);
      Draw(e, img, 255, -1);
      cout << "Fitted ellipse at (" << e.getPos() << ") with axis " << e.getR() << ", " << e.getR2() << endl;

    }
    break;
    }
  GetChar();
  return 0;
}

#include <image.h>

void generatePoints(vector<Point>& p1, int n, int mode = 1)
{
  switch (mode)
    {
    case 1:
      for (int i = 0; i < n; ++i)
        {
          double fi = i * M_PI * 2 / n;
          p1.push_back(Point(300 + 200 * cos(fi), 300 + 200 * sin(fi)));
        }
      break;
    case 2:
      for (int i = 0; i < n; ++i)
        {
          double fi = i * M_PI * 2 / n;
          double r = 200 * RandomD();
          p1.push_back(Point(300 + r * cos(fi), 300 + r * sin(fi)));
        }
      break;
    default:
      throw "generatePoints - Wrong mode ";
    }
}

void mkPointLists(const Trafo& tr, vector<Point>& p1,
                  vector<Point>& p2, int n, int mode = 1)
{
  p1.clear();
  generatePoints(p1, n, mode);
  Transform(tr, p1, p2);
}

void drawPoints(const Image& img, const vector<Point>& pl, int color)
{
  for (int i = 0; i < (int)pl.size(); ++i)
    {
      IPoint pi(pl[i]);
      if (img.inside(pi))
        img.setPixel(pi, color);
    }
}

void addNoise(vector<Point>& pl, double strength = 4)
{
  for (int i = 0; i < (int)pl.size(); ++i)
    {
      double nx = (RandomD() - 0.5) * strength;
      double ny = (RandomD() - 0.5) * strength;
      pl[i] += Point(nx, ny);
    }
}

double getDistance(const vector<Point>& p1, const vector<Point>& p2)
{
  double dist2 = 0.0;
  for (int i = 0; i < (int)p1.size(); ++i)
    {
      dist2 += (p2[i] - p1[i]).r2();
    }
  return sqrt(dist2 / p1.size());
}

int main(int argc, char** argv)
{
  Image imgd;
  imgd.create(800, 800, 255);
  imgd.set(0);
  Show(OVERLAY, imgd);
  vector<Point> p1;
  vector<Point> p2;
  matrix<double> m(3, 3, 1);
  m[0][1] = -0.1;
  m[0][2] = 300;
  m[1][0] = 0.1;
  m[1][2] = 250;
  m[2][0] = 1e-5;
  m[2][1] = -9e-4;
  m[2][2] = 1.7;

  Trafo tr(m);
  cout << tr.Tmatrix() << endl;

  mkPointLists(tr, p1, p2, 1500, 1);
  addNoise(p2, 150);
  drawPoints(imgd, p1, 1);
  drawPoints(imgd, p2, 2);

  vector<Point> pid;
  Transform(tr, p1, pid);
  drawPoints(imgd, pid, 1);

  Trafo res1 = MatchPointlists(p1, p2, TRM_AFFINE);
  cout << res1.getMatrix() << endl;
  vector<Point> pres1;
  Transform(res1, p1, pres1);
  drawPoints(imgd, pres1, 3);
  cout << "Abweichung: " << getDistance(pid, pres1) << endl;

  Trafo res2 = MatchPointlists(p1, p2, TRM_PROJECTIVE);
  cout << res2.getMatrix() << endl;
  vector<Point> pres2;
  Transform(res2, p1, pres2);
  drawPoints(imgd, pres2, 4);

  GetChar();
  return OK;
}

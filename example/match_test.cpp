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
  transform(tr, p1, p2);
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
  m[0][2] =   300;
  m[1][0] =  0.1;
  m[1][2] =   250;
  m[2][0] = 1e-5;
  m[2][1] = -9e-4;
  m[2][2] = 1.7;

  Trafo tr(m);
  cout << tr.getMatrix() << endl;

  mkPointLists(tr, p1, p2, 1500, 1);

  addNoise(p2, 15);

  vector<Point> pid;
  transform(tr, p1, pid);

  vector<int> modelist{TRM_SHIFT, TRM_SCALE_SHIFT,
                       TRM_SIMILARITY_NOR,
                       TRM_AFFINE,
                       TRM_PROJECTIVE};

  vector<string> modename{"TRM_SHIFT",
                          "TRM_SCALE_SHIFT",
                          "TRM_SIMILARITY_NOR",
                          "TRM_AFFINE",
                          "TRM_PROJECTIVE"};

  cout << "point list 1 - red" << endl;
  cout << "point list 2 - green" << endl;
  cout << "ideally transformed list 1 - red " << endl;
  cout << "matched point list 1 - blue" << endl;
  cout << "--------------------------------------------" << endl;
  for (unsigned int i = 0; i < modelist.size(); i++)
    {
      drawPoints(imgd, p1, 1);
      drawPoints(imgd, p2, 2);
      drawPoints(imgd, pid, 1);

      Trafo res1 = matchPointLists(p1, p2, modelist[i]);

      vector<Point> pres1;
      transform(res1, p1, pres1);
      drawPoints(imgd, pres1, 3);

      cout << "mode " << modelist[i] << ": " << modename[i] << endl;
      cout << res1.getMatrix() << endl;
      cout << "Abweichung: " << getDistance(pid, pres1) << endl;
      cout << "--------------------------------------------" << endl;
      GetChar();
      imgd.set(0);
    }

  return OK;
}

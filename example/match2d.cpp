#include <image.h>

namespace ice
{
  Trafo matchPointlistsProjective(const vector<Point>& p1,
                                  const vector<Point>& p2);

  Trafo iterateProjective(const Trafo& tr,
                          const vector<Point>& p1, const vector<Point>& p2);

}

const int steps = 300;

void makePointlists(const Trafo& tr, vector<Point>& pl1, vector<Point>& pl2)
{
  pl1.clear();
  pl2.clear();
  for (int x = 0; x < steps; x++)
    for (int y = 0; y < steps; y++)
      {
        Point p1(x, y);
        Point p2;
        transform(tr, p1, p2);
        pl1.push_back(p1);
        pl2.push_back(p2);
      }
}

int main(int argc, char** argv)
{
#if 1
  matrix<double> tm(3, 3, {1, 2, 3,  4, 5, 6,  0, 0, 1});
  Trafo trafo(tm);
#else
  Trafo trafo;
  trafo.rotate(0, 0, M_PI);
#endif
  vector<Point> pl1;
  vector<Point> pl2;
  makePointlists(trafo, pl1, pl2);

  for (int i = 0; i < pl1.size(); i++)
    {
      pl1[i] = pl1[i] + Point(0.01 * drand48(), 0.01 * drand48());
    }

  // projective transformation, linearisation
  Trafo res = matchPointlistsProjective(pl1, pl2);

  cout << setw(8) << setprecision(5) << fixed << res.getMatrix() << endl;
  cout << setw(9) << setprecision(2) << scientific << res.getMatrix() - tm << endl;

// iterative, start with identity
  Trafo res1;
  Trafo resIter = iterateProjective(res1, pl1, pl2);

  cout << setw(8) << setprecision(5) << fixed << resIter.getMatrix() << endl;

  cout << setw(9) << setprecision(2) << scientific << resIter.getMatrix() - tm << endl;

}

#include <image.h>

int main(int ragc, char** argv)
{
  Contur c(111, 111);
  c.Add(0);
  c.Add(0);
  c.Add(2);
  c.Add(2);
  c.Add(4);
  c.Add(4);
  c.Add(6);
  c.Add(6);

  cout << c.Number() << " direction codes in contur " << endl;

  Matrix pl = ConturPointlist(c, 1, true);
  cout << pl << endl;
  cout << pl.rows() << " points from start to start " << endl << endl;

  vector<IPoint> vpl;
  c.getPoints(vpl, true);

  cout << vpl.size() << " points in pointlist " << endl;
  for (int i = 0; i < vpl.size(); i++)
    cout << vpl[i].x << "," << vpl[i].y << endl;

  return OK;
}

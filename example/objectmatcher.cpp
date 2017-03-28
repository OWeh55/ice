#include <image.h>

void draw(vector<Point>& pl, int color, const Image& img)
{
  for (int i = 0; i < (int)pl.size(); ++i)
    {
      int i1 = i + 1;
      if (i1 >= (int)pl.size())
        i1 = 0;
      Line(pl[i], pl[i1], color, img);
    }
}

int main(int argc, char** argv)
{
  Image img;
  img.create(1000, 800, 255);
  img.set(0);

  Image mark;
  mark.create(1000, 800, 7);
  mark.set(0);

  Show(OVERLAY, img, mark);

  ObjectMatcher om(TRM_PROJECTIVE);
  //  ObjectMatcher om(TRM_AFFINE);

  // objekt 1 als Punktliste vorgeben

  vector<Point> pl1;
  pl1.push_back(Point(100, 100));
  pl1.push_back(Point(600, 100));
  pl1.push_back(Point(600, 700));
  pl1.push_back(Point(400, 700));
  pl1.push_back(Point(300, 200));
  pl1.push_back(Point(200, 700));
  pl1.push_back(Point(100, 700));

  om.setObject(1, pl1);
  draw(pl1, 1, mark);
  // eine Transformation
  matrix<double> tm(3, 3, 1);
  tm[0][0] = 0.8;
  tm[0][1] = 0.2;
  tm[0][2] = 50;
  tm[1][0] = -0.2;
  tm[1][1] = 0.8;
  tm[1][2] = 80;

#if 1
  tm[2][0] = -0.0001;
  tm[2][1] = -0.0001;
#endif

  Trafo tro(tm);
  cout << tro.getMatrix() << endl;

  // objekt 2 durch transformieren erzeugen
  vector<Point> pl2;
  Transform(tro, pl1, pl2);

  om.setObject(0, pl2);
  draw(pl2, 2, mark);

  // interpolieren um genügend Punkte zu haben
  om.interpolObject(0, 3);
  om.interpolObject(1, 3);

  //  om.setDistanceMeasure(D_SQUARE);
  om.setDistanceMeasure(D_EUCLID);

  // Referenzen abfragen
  vector<Point> opl1;
  vector<Point> opl2;
  om.getOrderedLists(opl1, opl2);
  cout << opl1.size() << " Punkte" << endl;
  for (int i = 0; i < (int)opl1.size(); i += 3)
    Line(opl1[i], opl2[i], 1, mark);

  matrix<double> dm;
  om.calcDistances(dm);
  Image distimg;
  distimg.create(dm.cols(), dm.rows(), 255);
  double maxdist = 0;
  for (int y = 0; y < distimg.ysize; ++y)
    for (int x = 0; x < distimg.xsize; ++x)
      if (dm[y][x] > maxdist)
        maxdist = dm[y][x];

  for (int y = 0; y < distimg.ysize; ++y)
    for (int x = 0; x < distimg.xsize; ++x)
      distimg.setPixel(x, y, 255 * dm[y][x] / maxdist);

  Show(ON, distimg);

  Trafo tr = om.getTrafo(MM_SQUARE);

  cout << tr.getMatrix() << endl;

  vector<Point> pl2a;
  Transform(tr, pl2, pl2a);
  om.setObject(0, pl2a);
  draw(pl2a, 3, mark);
#if 0
  om.interpolObject(0, 3);

  om.getOrderedLists(opl1, opl2);
  cout << opl1.size() << " Punkte" << endl;
  for (int i = 0; i < (int)opl1.size(); i += 3)
    Line(opl1[i], opl2[i], 2, mark);

  Trafo tr2 = om.getTrafo(MM_SQUARE);
  tr = tr * tr2;
  cout << tr.getMatrix() << endl;

  Transform(tr, pl2, pl2a);
  draw(pl2a, 4, mark);
  //  Contur c3 = Transform(tr, pl1);
  //  MarkContur(c3, 127, img);
#endif
  GetChar();
  return 0;
}

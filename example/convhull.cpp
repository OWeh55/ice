#include <image.h>

int main(int argc, char** argv)
{
#if 1
  Image img;
  img.create(1000, 1000, 255);
  clearImg(img);
  Show(ON, img);

  vector<Point> pl;
  for (int i = 0; i < 100000; i++)
    {
#if 1
      double fi = RandomD() * M_PI * 2;
      double r = RandomD() * 300;
      double x = r * cos(fi) + 500;
      double y = r * sin(fi) + 500;
#else
      int x = Random(500) + 250;
      int y = Random(500) + 250;
#endif
      pl.push_back(Point(x, y));
      //  pl.push_back(Point(x, y)); // gleiche Punkte
      PutVal(img, x, y, 127);
    }
  Printf("Berechne..");
  ConvexHull(pl, pl);
  Printf("fertig\n");

  for (unsigned int i = 1; i < pl.size(); i++)
    Line(pl[i - 1], pl[i], 255, img);
  Line(pl[0], pl.back(), 255, img);

  GetChar();
  return 0;
#else
#if 1
  vector<IPoint> pl;
  pl.push_back(IPoint(0, 0));
  pl.push_back(IPoint(1, 30));
  pl.push_back(IPoint(2, 0));
  pl.push_back(IPoint(2, 10));
  pl.push_back(IPoint(2, 40));
  pl.push_back(IPoint(2, 20));
  pl.push_back(IPoint(2, 20));
  pl.push_back(IPoint(2, 30));
  pl.push_back(IPoint(3, -40));
  pl.push_back(IPoint(3, 30));
  pl.push_back(IPoint(4, -2));

  ConvexHull(pl, pl);
  for (unsigned int i = 0; i < pl.size(); i++)
    cout << pl[i] << endl;

#else
  Image img;
  img.create(512, 512, 255);
  Image mrk;
  mrk.create(512, 512, 8);
  Show(OVERLAY, img, mrk);
  Contur c = SelContur(img, true);
  cout << "Originalkontur:" << endl;
  cout << (c.isClosed() ? "closed" : "open") << endl;
  cout << (c.isHole() ? "hole" : "object") << endl;
  Contur ch = ConvexHull(c);
  cout << "Konvexe Hülle:" << endl;
  cout << (ch.isClosed() ? "closed" : "open") << endl;
  cout << (ch.isHole() ? "hole" : "object") << endl;
  MarkContur(ch, 1, mrk);
  GetChar();
  return OK;
#endif
#endif
}

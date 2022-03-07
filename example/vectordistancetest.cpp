#include <image.h>

const int ns = 15;

int main(int argc, char** argv)
{
  vector<double> soll(5);
  soll[0] = 0;
  soll[1] = 1;
  soll[2] = 2;
  soll[3] = 3;
  soll[4] = 4;

  EuclideanVectorDistance ed;
  CityBlockVectorDistance md;
  EarthMoverVectorDistance emd;
  EarthMoverUnNormalizedVectorDistance emud;

  Matrix m(5, 5);

  m[0][0] = 1.0;
  m[0][1] = 0.0;
  m[0][2] = 0.0;
  m[0][3] = 0.0;
  m[0][4] = 0.0;
  m[1][0] = 0.0;
  m[1][1] = 1.0;
  m[1][2] = 0.0;
  m[1][3] = 0.0;
  m[1][4] = 0.0;
  m[2][0] = 0.0;
  m[2][1] = 0.0;
  m[2][2] = 1.0;
  m[2][3] = 0.0;
  m[2][4] = 0.0;
  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;
  m[3][4] = 0.0;
  m[4][0] = 0.0;
  m[4][1] = 0.0;
  m[4][2] = 0.0;
  m[4][3] = 0.0;
  m[4][4] = 1.0;

  QuadraticFormVectorDistance qd(m);

#if 0

  while (true)
    {
      vector<double> test(5);

      for (int i = 0; i < 5; i++)
        {
          cout << i << ": ";
          cin >> test[i];
        }

      cout << "Euklidische Distanz: " << ed.distance(soll, test) << endl;
      cout << "Manhattan Distanz: " << md.distance(soll, test) << endl;
      cout << "Earth Mover Distanz: " << emd.distance(soll, test) << endl;
      cout << "Quadratische Form-Distanz: " << qd.distance(soll, test) << endl;
    }

#endif

  Image grw = ReadImg("test_gray.jpg");
  Show(ON, grw);
  Image dist1 = NewImg(grw);
  Image dist2 = NewImg(grw);
  Image dist3 = NewImg(grw);
  Show(ON, dist1, "Euklidische Distanz");
  Show(ON, dist2, "City Block Distanz");
  Show(ON, dist3, "Earth Mover Distanz");
  clearImg(dist1);
  clearImg(dist2);
  clearImg(dist3);

  while (true)
    {
      IPoint testpoint = SelPoint(grw);
      Histogram h(grw(Window(testpoint.x - ns, testpoint.y - ns,
                             testpoint.x + ns, testpoint.y + ns)));
      vector<double> vtest;
      h.getRelative(vtest);

      for (int y = ns; y < grw->ysize - ns; y++)
        for (int x = ns; x < grw->xsize - ns; x++)
          {
            Histogram h1(grw(Window(x - ns, y - ns, x + ns, y + ns)));
            vector<double> vtest1 = h1.getRelative();
            PutVal(dist1, x, y, limited(ed(vtest, vtest1) * 255, dist1));
            PutVal(dist2, x, y, limited(md(vtest, vtest1) * 255, dist2));
            PutVal(dist3, x, y, limited(emd(vtest, vtest1), dist3));
          }
    }

  return 0;
}

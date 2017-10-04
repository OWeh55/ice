#include <image_nonvis.h>
#include <calibc.h>
#include <analygeo3d.h>
#include <Vector.h>

int main(int argc, char* argv[])
{
  vector<Vector3d> world
  {
    {-100, -100, 0},
    {-100, 100, 0},
    {100, -100, 0},
    {100, 100, 0},

    {-100, -100, -100},
    {-100, 100, -100},
    {100, -100, -100},
    {100, 100, -100}
  };

  vector<Point> bild
  {
    {-1,     -1},
    {-1,    1},
    {1,      -1},
    {1,    1},

    {-0.9, -0.9},
    {-0.9, 0.9},
    {0.9, -0.9},
    {.9, 0.9}
  };

  vector<Point> bild2
  {
    {-1 + 0.01,     1},
    {-1 + 0.01,    -1},
    {1 + 0.01,      1},
    {-1 + 0.01,    -1},
    {-0.9 + 0.009, 0.9},
    {-0.9 + 0.009, -0.9},
    {0.9 + 0.009,  0.9},
    {-0.9 + 0.009, -0.9}
  };

  cout.precision(4);
  cout.setf(ios_base::fixed | ios_base::showpos);

  Camera cam1;
  cout << cam1.makeVectorDouble() << endl;
  cout << cam1.toString(" ") << endl;

  for (int i = 0; i < world.size(); i++)
    cout << cam1.transform(world[i]) << endl;


  calibrate(cam1, world, bild, 3);
  cout << cam1.makeVectorDouble() << endl;
  cout << cam1.toString(" ") << endl;

  for (int i = 0; i < world.size(); i++)
    cout << bild[i] << " == " << cam1.transform(world[i]) << endl;

#if 0
  calibrate(cam2, world, bild2, 3);
  cout << cam2.toString(" ") << endl;

  //  for (i=0;i<8;i++)
  //   {
  //     cout << bild[i] << " ? " << cam.Transform(welt[i]) << " " ;
  //     cout << (bild[i]-cam.Transform(welt[i])).Length() << endl;
  //   }

  // Test der Rekonstruktion
  for (i = 0; i < 8; i++)
    {
      Line3d ray1 = cam.Ray(bild[i]);
      Line3d ray2 = cam2.Ray(bild2[i]);
      Vector3d p1;
      double dist;
      Vector para;
      int rc = Intersection(ray1, ray2, p1, dist, para);
      cout << bild[i] << bild2[i] << (p1 - world[i]).Length() << endl;
      cout << "rc: " << rc << endl;
      //     cout << "Schnittpunkt: " << p1 << endl;
      //     cout << "Abstand: " << dist << endl;
      //     cout << "Parameter: " << para << endl;
    }
#endif
  return OK;
}

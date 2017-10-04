#include <image_nonvis.h>
#include <calibc.h>
#include <analygeo3d.h>
#include <Vector.h>

int main(int argc, char* argv[])
{
  vector<Vector3d> world;
#if 0
  {
    {-100, -100, 100},
    {-100, 100, 100},
    {100, -100, 100},
    {100, 100, 100},

    {-100, -100, -100},
    {-100, 100, -100},
    {100, -100, -100},
    {100, 100, -100}
  };
#endif
  for (int x = -100; x <= 100; x += 10)
    for (int y = -100; y <= 100; y += 10)
      for (int z = -100; z <= 100; z += 10)
        world.push_back({(double)x, (double)y, (double)z});

  int nPoints = world.size();

  cout.precision(4);
  cout.setf(ios_base::fixed | ios_base::showpos);

  cout << "-- camera 1 ------------------------" << endl;
  Camera cam1;
  cam1.set(1000, 1, 0, 300, 300);
  cam1.setExt(-10, 0, 1000, 0, 0, 0);
  //  cout << cam1.makeVectorDouble() << endl;
  cout << cam1.toString(" ") << endl;

  vector<Point> p(nPoints);

  for (int i = 0; i < nPoints; i++)
    p[i] = cam1.transform(world[i]);

  Camera cam1r;
  calibrate(cam1r, world, p);
  cout << cam1r.toString(" ") << endl;

  double pError1 = 0.0;
  for (int i = 0; i < nPoints; i++)
    {
#if 0
      cout << p[i] << " == " << cam1r.transform(world[i]) << endl;
#endif
      pError1 += (p[i] - cam1r.transform(world[i])).Length();
    }
  cout << scientific << "mittlerer Fehler der Projektion: " << pError1 / nPoints << endl;

  cout << "-- camera 2 ------------------------" << endl;

  cout.precision(4);
  cout.setf(ios_base::fixed | ios_base::showpos);

  Camera cam2;
  vector<double> cam2p = cam2.makeVectorDouble();
  cam2p[5] = 10; // x pos
  cam2.set(cam2p);
  //  cout << cam2.makeVectorDouble() << endl;
  cout << cam2.toString(" ") << endl;

  vector<Point> p2(nPoints);

  for (int i = 0; i < nPoints; i++)
    p2[i] = cam2.transform(world[i]);

  Camera cam2r;
  calibrate(cam2r, world, p2);
  //  cout << cam2r.makeVectorDouble() << endl;
  cout << cam2r.toString(" ") << endl;

  double pError2 = 0.0;
  for (int i = 0; i < nPoints; i++)
    {
#if 0
      cout << p2[i] << " == " << cam2r.transform(world[i]) << endl;
#endif
      pError2 += (p2[i] - cam2r.transform(world[i])).Length();
    }
  cout << scientific << "mittlerer Fehler der Projektion: " << pError2 / nPoints << endl;
  cout << "-- 3D-Rekonstruktion --------------------" << endl;
  // Test der 3D-Rekonstruktion
  double error = 0.0;
  for (int i = 0; i < nPoints; i++)
    {
      Line3d ray1 = cam1.Ray(p[i]);
      Line3d ray2 = cam2.Ray(p2[i]);
      Vector3d p1;
      double dist;
      Vector para;
      int rc = Intersection(ray1, ray2, p1, dist, para);
#if 0
      cout << p[i] << p2[i] << " ";
      cout << p1 << " " << world[i] ;
      cout << " (" << (p1 - world[i]).Length() << ")" << endl;
      cout << "rc: " << rc << endl;
#endif

      error += (p1 - world[i]).Length();
      //     cout << "Schnittpunkt: " << p1 << endl;
      //     cout << "Abstand: " << dist << endl;
      //     cout << "Parameter: " << para << endl;
    }
  cout << scientific << "mittlere Fehler der 3d-Rekonstruktion: " << error / nPoints << endl;
  return OK;
}

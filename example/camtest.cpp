#include <image_nonvis.h>
#include <calibc.h>
#include <analygeo3d.h>
#include <Vector.h>

Camera test;

int main(int argc, char* argv[])
{
  int i;
  Camera cam;
  Camera cam2;

  Matrix welt =
    Vector(-100, -100, 0) &&
    Vector(-100, 100, 0) &&
    Vector(100, -100, 0) &&
    Vector(-100, 100, 0) &&
    Vector(-100, -100, -100) &&
    Vector(-100, 100, -100) &&
    Vector(100, -100, -100) &&
    Vector(-100, 100, -100);

  Matrix bild =
    Vector(-1,     1) &&
    Vector(-1,    -1) &&
    Vector(1,      1) &&
    Vector(-1,    -1) &&
    Vector(-0.9, 0.9) &&
    Vector(-0.9, -0.9) &&
    Vector(0.9,  0.9) &&
    Vector(-0.9, -0.9);

  Matrix bild2 =
    Vector(-1 + 0.01,     1) &&
    Vector(-1 + 0.01,    -1) &&
    Vector(1 + 0.01,      1) &&
    Vector(-1 + 0.01,    -1) &&
    Vector(-0.9 + 0.009, 0.9) &&
    Vector(-0.9 + 0.009, -0.9) &&
    Vector(0.9 + 0.009,  0.9) &&
    Vector(-0.9 + 0.009, -0.9);

  cout.precision(4);
  cout.setf(ios_base::fixed | ios_base::showpos);

  cout << cam.toString(" ") << endl;

  for (int i = 0; i < welt.rows(); i++)
    cout << cam.transform(welt[i]) << endl;


  Calib(cam, welt, bild, 3);
  cout << cam.toString(" ") << endl;

  Calib(cam2, welt, bild2, 3);
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
      cout << bild[i] << bild2[i] << (p1 - Vector3d(welt[i])).Length() << endl;
      cout << "rc: " << rc << endl;
//     cout << "Schnittpunkt: " << p1 << endl;
//     cout << "Abstand: " << dist << endl;
//     cout << "Parameter: " << para << endl;
    }

  return OK;
}

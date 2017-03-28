/*
 * Test geometrischer 3D-Objekte und 3D analytischer Geometrie
 *
 */

#include <image_nonvis.h>
#include <geo3d.h>
#include <analygeo3d.h>

void PunktAbstand(Vector3d p1, Vector3d p2)
{
  cout << "Abstand der Punkte " << Vector(p1) << " und " << Vector(p2) << " = " << Distance(p1, p2);
  cout << " = " << p1.Distance(p2);
  cout << endl;
}

void PunktGeradeAbstand(Vector3d p1, Line3d l2)
{
  cout << "Abstand des Punktes " << Vector(p1);
  cout << " von der Geraden " << Vector(l2.P1()) << ".." << Vector(l2.P2()) << ": " ;
  cout << Distance(p1, l2) << " = " << Distance(l2, p1) << " = ";
  cout << l2.Distance(p1) << endl;
}

void GeradenAbstand(Line3d l1, Line3d l2)
{
  cout << "Abstand der Geraden " << Vector(l1.P1()) << ".." << Vector(l1.P2());
  cout << " und " << Vector(l2.P1()) << ".." << Vector(l2.P2()) << ": " ;

  cout << Distance(l1, l2) << endl;

  Vector3d schnittpunkt;
  double dist;
  Vector res;

  int rc = Intersection(l1, l2, schnittpunkt, dist, res);

  if (rc == OK)
    {
      cout << "\"Schnittpunkt\" der Geraden: ";
      cout << Vector(schnittpunkt) << endl;
      cout << "Abstand des \"Schnittpunktes\" von den Geraden: " << dist << endl;
      cout << "Relative Lage der nächsten Punkte auf den Geraden:" << res << endl;
      cout << "Nächste Punkte auf den Geraden: " ;
      cout << Vector(l1.P1() + res[0]*l1.DP()) << ", " <<  Vector(l2.P1() + res[0]*l2.DP()) << endl;
    }
  else
    cout << "Schnittpunkt existiert nicht (parallele Geraden)" << endl;
}

void KreisGeradenSchnitt(const Sphere& s, const Line3d& l)
{
  Vector3d s1, s2;
  cout << "Schnitt Kreis " << Vector(s.Pos()) << " R=" << s.R() << "  Gerade " << Vector(l.P1()) << ".." << Vector(l.P2()) << endl;
  int i = Intersection(l, s, s1, s2);

  if (i == OK)
    cout << "Schnittpunkte: " << Vector(s1) << " " << Vector(s2) << endl;

  if (i == NO_INTERSECTION)
    cout << "kein Schnittpunkt, nächster Punkt: " << Vector(s1) << "=" << Vector(s2) << endl;

  if (i == TANGENT)
    cout << "Tangente - Berührungspunkt: " << Vector(s1) << "=" << Vector(s2) << endl;
}

int main(int arg, char* argv[])
{
  // Eckpunkte eines Würfels
  Vector3d p000(0, 0, 0);
  Vector3d p001(0, 0, 1);
  Vector3d p010(0, 1, 0);
  Vector3d p011(0, 1, 1);
  Vector3d p100(1, 0, 0);
  Vector3d p101(1, 0, 1);
  Vector3d p110(1, 1, 0);
  Vector3d p111(1, 1, 1);

  Line3d l(p000, p110);
  // --
  PunktAbstand(p000, p001);
  PunktAbstand(p000, p011);
  PunktAbstand(p000, p111);
  // --
  PunktGeradeAbstand(p000, l);
  PunktGeradeAbstand(p010, l);
  PunktGeradeAbstand(p001, l);
  PunktGeradeAbstand(p011, l);
  // --
  Line3d l2(p011, p101);
  GeradenAbstand(l, l2);
  // --
  Vector3d p1a = p010;
  Vector3d p1b = p100;
  Vector3d p2a = p101;
  Vector3d p2b = p011;
  GeradenAbstand(Line3d(p1a, p1b), Line3d(p2a, p2b));

// --
  Sphere s(0, 0, 0, sqrt(2.0));
  Line3d ls(Vector3d(1, -1, 0), Vector3d(1, 1, 0));
  Vector3d s1, s2;
  KreisGeradenSchnitt(s, ls);
  s.setR(1.0);
  KreisGeradenSchnitt(s, ls);
  s.setR(0.9);
  KreisGeradenSchnitt(s, ls);
  return OK;
}

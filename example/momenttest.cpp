// tests zur Klasse Moments

#include <iomanip>
using namespace std;

#include <getopt.h>
#include <image_nonvis.h>

// Ausgabe der Momente
void pmom(const Moments& m)
{
  int i, j;

  for (j = 0; j < 5; j++)
    {
      for (i = 0; i < 5 - j; i++)
        {
          cout << m(i, j) << "\t ";
        }

      cout << endl;
    }
}

int main(int argc, char* argv[])
{
  enum test_t {all, constructors, trafos, normal, fit};
  test_t was_testen = all;
  cout.precision(4);
  cout.setf(ios::fixed);

  int option;

  while ((option = getopt(argc, argv, "taknf")) > 0)
    {
      switch (option)
        {
        case 'a':
          was_testen = all;
          break;
        case 'k':
          was_testen = constructors;
          break;
        case 't':
          was_testen = trafos;
          break;
        case 'n':
          was_testen = normal;
          break;
        case 'f':
          was_testen = fit;
          break;
        }
    }

  cout.precision(3);
  cout.setf(ios::fixed);

  // Objekte zur Momentbestimmung
  Matrix q1 = Vector(10, 10) && Vector(-10, 10)
              && Vector(-10, -10) && Vector(10, -10);
  Matrix q2 = Vector(0, 0.0) && Vector(10, 0.0)
              && Vector(10, 10) && Vector(0, 10);

  Region r;
  r.add(-10, -10, 10, 10);

  Contur c(-10, -10);
  c.add(10, -10);
  c.add(10, 10);
  c.add(-10, 10);
  c.add(-10, -10);

  Moments m1, m2, m3;
  Moments m1n, m2n, m3n;

  if (was_testen == all || was_testen == constructors)
    {
      cout << "Konstruktortest" << endl;
      cout << "Zwei gleiche Objekte" << endl;
      // Zwei Wege der Initialisierung
      m1 = Moments(q1); // Konstruktor mit Objekt
      pmom(m1);

      m2.Init();      // "leerer" Konstruktor
      m2.add(q1);     // Hinzufügen eines Objekt
      pmom(m2);
      cout << "Differenz: " << m1.Diff(m2) << endl;

      cout << "Ein diskretes Objekt als Region " << endl;
      m1 = Moments(r);
      pmom(m1);
      cout << "Ein diskretes Objekt als Contur " << endl;
      m2 = Moments(c);
      pmom(m2);
      cout << "Differenz: " << m1.Diff(m2) << endl;
    }

  if (was_testen == all || was_testen == trafos)
    {
      cout << "Zwei unterschiedliche Quadrate ineinander transformieren" << endl;
      m1 = Moments(q1); // Quadrat um Nullpunkt
      m2 = Moments(q2); // Verschoben und Skaliert
      m1 = m1.Scale(0.5); // Größe anpassen
      m1 = m1.Translate(5, 5); // verschieben
      pmom(m1);
      pmom(m2);
      cout << "Differenz: " << m1.Diff(m2) << endl;
      cout << "komplexe Trafo hin und sofort zurück" << endl;
      m2 = m1.Rotate(3).Scale(0.75).XShear(0.1).XShear(-0.1).Scale(1 / 0.75).Rotate(-3);
      pmom(m1);
      pmom(m2);
      cout << "Differenz: " << m1.Diff(m2) << endl;

      cout << "Eine Transformation als 'Trafo'" << endl;
      Trafo tr;
      tr.rotate(0, 0, 3);
      tr.scale(0, 0, 0.75);
      tr.shearX(0.1);
      Trafo tri = tr.inverse();
      m2 = m1.AffineTransform(tr);
//    pmom(m1); pmom(m2);
      cout << "..und die inverse Transformation" << endl;
      m2 = m2.AffineTransform(tri);
      pmom(m1);
      pmom(m2);
      cout << "Differenz: " << m1.Diff(m2) << endl;
    }

  if (was_testen == all || was_testen == normal)
    {
      // Zwei affin-äquivalente Objekte
      Matrix q1 =
        Vector(11, 11) &&
        Vector(-10, 10) &&
        Vector(-10, -10) &&
        Vector(10, -10);

      Matrix q2 =
        Vector(10.5, 10.5) &&
        Vector(0, 10) &&
        Vector(0, 0.0) &&
        Vector(10, 0.0);

      // ein abweichendes Objekt
      Matrix q3 =
        Vector(0, 0.0) &&
        Vector(10, 1) &&
        Vector(7, 10) &&
        Vector(3, 1);

      m1 = Moments(q1);
      m2 = Moments(q2);
      m3 = Moments(q3);
      Trafo tr1;
      Trafo tr2;
      Trafo tr3;
      cout << "------Normalisierung (Standardmethode) -----------" << endl;
      m1n = m1.Normalize(tr1, Moments::standard);
      m2n = m2.Normalize(tr2, Moments::standard);
      m3n = m3.Normalize(tr3, Moments::standard);
      pmom(m1n);
      pmom(m2n);
      cout << "Differenz: " << m1n.Diff(m2n) << endl;
      pmom(m3n);
      cout << "------Polynom-Methode-----------" << endl;
      m1n = m1.Normalize(tr1, Moments::polynom);
      m2n = m2.Normalize(tr2, Moments::polynom);
      m3n = m3.Normalize(tr3, Moments::polynom);
      pmom(m1n);
      pmom(m2n);
      cout << "Differenz: " << m1n.Diff(m2n) << endl;
      pmom(m3n);
      cout << "------Iterations-Methode-----------" << endl;
      m1n = m1.Normalize(tr1, Moments::iteration);
      m2n = m2.Normalize(tr2, Moments::iteration);
      m3n = m3.Normalize(tr3, Moments::iteration);
      pmom(m1n);
      pmom(m2n);
      cout << "Differenz: " << m1n.Diff(m2n) << endl;
      pmom(m3n);
    }

  if (was_testen == all || was_testen == fit)
    {
      cout << "Test Fitting" << endl;
      Matrix drei = Vector(10, 0.0) && Vector(10, 10) && Vector(0, 10);
      cout << "Dreieck->Dreieck" << endl;
      cout << drei << FitTriangle(Moments(drei)) << endl;

      //    Matrix recht=Vector(-20,10)&&Vector(20,10)&&Vector(20,-10)&&Vector(-20,-10);
      Matrix recht = Vector(10, 0.0) && Vector(0, 10) && Vector(20, 30) && Vector(30, 20);
//    Moments mr(recht);
//    pmom(mr);
      cout << "Rechteck->Rechteck" << endl;
      cout << recht << FitRectangle(Moments(recht)) << endl;
      cout << "Rechteck->Parallelogramm" << endl;
      cout << recht << FitParallelogram(Moments(recht)) << endl;
      //    cout << "Rechteck->Viereck" << endl;
      //    cout << recht << Moments(recht).FitQuadrangle() << endl;

      Matrix CircA(0, 2);

      for (int i = 0; i < 360; i += 1)
        {
          CircA.Append(Vector(10.0 * cos(Arcus(i)) + 9.0, 10.0 * sin(Arcus(i)) + 8.0));
        }

      cout << "fast Kreis -> Kreis" << endl;
      double x0, y0, rad;
      FitCircle(Moments(CircA), x0, y0, rad);
      cout << "(" << x0 << "," << y0 << ") " << rad << endl;

      cout << "fast Ellipse -> Ellipse" << endl;
      Trafo tr;
      tr.scale(0.0, 0.0, 2.0, 1.0);
      tr.rotate(0.0, 0.0, 2);
      Moments mel(CircA);
      mel = mel.AffineTransform(tr);
      Ellipse el = FitEllipse(mel);
      cout << "(" << el.getPos().x << "," << el.getPos().y << ") ";
      cout << "r1:" << el.getR() << " r2:" << el.getR2() << " fi:" << el.getPhi() << endl;

      cout << "Region->Rechteck" << endl;
      Region r1;
      r1.add(-10, -10, 10, 10);
      cout << FitRectangle(Moments(r1)) << endl;
      pmom(Moments(FitRectangle(Moments(r1))));
      pmom(Moments(r1));
    }

#if 0
  // test zur normalisierung der Rotation
  double fi;
  m2 = m1.NormalizeRotation(fi);
  cout << fi << endl;
  m2.NormalizeRotation(fi);
  cout << fi << endl;
  m1 = m2.Rotate(2.1);
  m1.NormalizeRotation(fi);
  cout << fi << endl;
  m2 = m1.Rotate(fi);
  m2.NormalizeRotation(fi);
  cout << fi << endl;
#endif

  return 0;
}

/*
 *  tests for lmdif (functor version)
 *  lineare Regression
 */

#include <stdlib.h>
#include <image_nonvis.h>
#include <float.h>
#include <lmdif.h>

// functor

class LinReg: public LMFunctionObject
{
private:
  const Vector& x; // Referenz-Paare (Argument,Funktionswert)
  const Vector& y;
  Vector& fpara; // Parameter der linearen Funktion
public:
  // Konstruktor
  // übernimmt (als Referenz) die zu optimierenden
  // Parameter der linearen Funktion und die Referenz-Liste
  LinReg(Vector& pp, const Vector& xp, const Vector& yp):
    x(xp), y(yp), fpara(pp) {} ;
  // Dimension der Fehlerfunktion
  int funcdim() const
  {
    return x.size(); // Ein Fehlerwert pro Funktionswert
  }
  // Fehler-Funktion
  int operator()(Vector& result) const
  {
    for (unsigned int i = 0; i < x.size(); i++) // für jeden Funktionswert
      result[i] = fpara[1] * x[i] + fpara[0] - y[i];

    return 1;
  }
};

int testLMDif4()
{
  int info;
  int inumber = 10000;
  int i;

  Vector x;
  Vector y;

  for (i = 0; i < 100000; i++)
    {
      double fx = RandomD() * 100.0;
      double fy = fx * 5 - 3 + RandomD() - 0.5;
      x.Append(fx);
      y.Append(fy);
    }

  Vector n(2); // Parameter der linearen Funktion

  vector<double*> op(2);  // Pointer auf zu optimierende Parameter
  op[0] = &n[0];
  op[1] = &n[1];

  info = LMDif(op,
               LinReg(n, x, y),
               1000,
               inumber);

  printf("Nach %d Iterationen: ", inumber);
  printf("Funktion: y= %g*x + %g\n", n[1], n[0]);
  return OK;
}

int main(int argc, char* argv[])
{
  testLMDif4();
  return OK;
}

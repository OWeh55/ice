#include <iostream>
#include <iomanip>
#include <vector>

#include "image.h"

class SinReg: public LMFunctor
{
private:
  const vector<double>& x; // Referenz-Paare (Argument x,Funktionswert y)
  const vector<double>& y;

public:
  // Konstruktor
  // übernimmt (als Referenz) die zu optimierenden
  // Parameter der linearen Funktion und die Referenz-Liste
  SinReg(const vector<double>& xp, const vector<double>& yp):
    x(xp), y(yp) { } ;

  // Dimension der Fehlerfunktion
  int getDimension() const
  {
    return y.size();    // Ein Fehlerwert pro Funktionswert
  }

  // Fehler-Funktion
  int operator()(const vector<double>& p, vector<double>& result) const
  {
    for (unsigned int i = 0; i < x.size(); i++) // für jede stützstelle
      result[i] = p[2] * cos(x[i] * p[1] + p[0]) - y[i];

    return 1;
  }
};

int main(int argc, char** argv)
{
  int nValues = 0;
  if (argc > 1)
    nValues = atoi(argv[1]);

  vector<double> x{1.0, 2.0, 3.0, 4.0, 5.0}; // Die Stützstellen

  // eventuell mehr stützstellen
  for (int i = x.size(); i < nValues; ++i)
    x.push_back(drand48()*M_PI * 2);

  vector<double> y(x.size());

  for (unsigned int i = 0; i < x.size(); ++i)
    y[i] = 3.107 * cos(x[i] * 1.111 + 0.5);

  vector<double> n(3); // geschätzte Parameter der Funktion
  n[0] = 0;
  n[1] = 1;
  n[2] = 1;

  SinReg sr(x, y);     // LMFunctor für Fehlerfunktion

  LMSolver lms(sr);

  lms.solve(n);        // nichtlineares System lösen

  cout << "Nach " << lms.getNIterations() << " Iterationen " << endl;
  cout << "Kode für Beendigung: " << lms.getInfo() << endl;
  cout << "Restfehler: " << lms.getErrorValue() << endl;

  // Winkel normalisieren
  while (n[0] > 2 * M_PI)
    n[0] -= 2 * M_PI;
  while (n[0] < 0)
    n[0] += 2 * M_PI;

  cout << "Funktion: y = " << n[2] << " * cos( x * " << n[1] << " + " << n[0] << " )" << endl;

  // bis zu 20 Stützstellen approximierte Werte ausgeben
  for (unsigned int i = 0; i < x.size() && i < 20; ++i)
    {
      double nval = n[2] * cos(x[i] * n[1] + n[0]);
      cout << fixed << setw(10) << x[i] << " -> " << setw(9) << y[i] << " =? " << setw(9) << nval << " diff: " << scientific << setw(10) << (nval - y[i]) << endl;
    }
  return OK;
}

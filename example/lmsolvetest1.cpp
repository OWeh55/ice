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

const int xsize = 1200;
const int ysize = 800;

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
    //    y[i] = 3.107 * cos(x[i] * 1.111 + 0.5);
    y[i] = 2 * cos(x[i] * 2 + 0.5);

  Image img;
  img.create(xsize, ysize, 255);
  img.set(127);
  Show(GRAY, img);

  Image imgi;
  imgi.create(xsize, ysize, 16);
  imgi.set(0);
  Show(OVERLAY, imgi);
  for (int k1 = 0; k1 < xsize; ++k1)
    for (int k2 = 0; k2 < ysize; ++k2)
      {
        vector<double> n(3); // Startwerte
        n[0] = k1 * 12.56 / xsize;
        n[1] = 0.5 + k2 * 5.0 / ysize;
        n[2] = 1.0;

        SinReg sr(x, y);     // LMFunctor für Fehlerfunktion

        LMSolver lms(sr);
        // void setStopConditions(double ftol, double xtol, double gtolp, int maxIter)
        lms.setStopConditions(0, 0, 0, 255);
        lms.solve(n);        // nichtlineares System lösen
        img.setPixelLimited(k1, k2, lms.getErrorValue() * 200);
        //img.setPixelLimited(k1, k2, lms.getNIterations() * 5);
        imgi.setPixelLimited(k1, k2, lms.getInfo());
        // imgi.setPixelLimited(k1, k2, lms.getInfo() & 8 ? 1 : 0);
      }

  // HistogramEqualization(img);
  // img.write("nIter.jpg");
  GetChar();
  return OK;
}

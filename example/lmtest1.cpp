#include <iostream>
#include <iomanip>
#include <vector>

#include "image.h"

class SinReg: public LMFunctionObject
{
private:
  const vector<double>& x; // Referenz-Paare (Argument x,Funktionswert y)
  const vector<double>& y;
  Vector& para;
public:
  // Konstruktor
  // übernimmt (als Referenz) die zu optimierenden
  // Parameter der linearen Funktion und die Referenz-Liste
  SinReg(Vector& para, const vector<double>& xp, const vector<double>& yp):
    x(xp), y(yp), para(para) { } ;

  // Dimension der Fehlerfunktion
  int funcdim() const
  {
    return x.size();    // Ein Fehlerwert pro Funktionswert
  }

  // Fehler-Funktion
  int operator()(Vector& result) const
  {
    for (unsigned int i = 0; i < x.size(); i++) // für jede stützstelle
      result[i] = para[2] * cos(x[i] * para[1] + para[0]) - y[i];

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
    //    y[i] = 3.107 * cos(x[i] * 1.111 + 0.5);
    y[i] = 5 * cos(x[i] * 5 + 0.5);

  Image img;
  img.create(1000, 1000, 255);
  img.set(127);
  Show(GRAY, img);

  Image imgi;
  imgi.create(1000, 1000, 16);
  imgi.set(0);
  Show(OVERLAY, imgi);
  for (int k1 = 0; k1 < 1000; ++k1)
    for (int k2 = 0; k2 < 1000; ++k2)
      {
        Vector n(3); // Startwerte
        //        n[0] = 0;
        //        n[1] = k2 * 0.01;
        //        n[2] = k1 * 0.01;
        n[0] = k1 * 12.56 / 1000;
        n[1] = 0.5 + k2 * 5.0 / 1000;
        n[2] = 1.0;

        vector<double*> op(3);
        op[0] = &n[0];
        op[1] = &n[1];
        op[2] = &n[2];

        SinReg sr(n, x, y);     // LMFunctionObject für Fehlerfunktion

        int inumber = 255;
        int info = LMDif(op, sr, 255, inumber);

        double sum = 0;
        for (int i = 0; i < x.size(); ++i)
          {
            double fv = n[2] * cos(x[i] * n[1] + n[0]);
            double error = fv - y[i];
            sum += error * error;
          }
        sum = sqrt(sum * 1000);
        img.setPixelLimited(k1, k2, sum);
        //        img.setPixelLimited(k1, k2, inumber);
        imgi.setPixelLimited(k1, k2, info);
        //imgi.setPixelLimited(k1, k2, lms.getInfo() & 8 ? 1 : 0);
      }
  GetChar();
  return OK;
}

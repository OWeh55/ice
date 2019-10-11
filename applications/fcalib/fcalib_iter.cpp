#include <image.h>
#include <lmdif.h>
//#include <radial.h>
#include <omatch.h>
#include <lseg.h>

#include <string>
#include <vector>
#include <fstream>
#include "camera.h"

using namespace std;
using namespace ice;

//========= Kalibrierung =============
class KalibFunk: public LMFunctor
{
  const vector<Matrix>& pl;
public:
  KalibFunk(const vector<Matrix>& pls): pl(pls) {};
  int operator()(const Vector& xv, Vector& result) const
  {
    int anz = pl.size();
//  cout << anz << " Kameras" << endl;
    int i, j; // laufvariablen "kameras" "referenzpunkte"
    int id = 0; // index in ergebnis-matrix
    for (i = 0; i < anz; i++)
      {
        Camera cam;
//    cout << "cam lesen" << endl;
        int xo = 9 + 6 * i;
        cam.Set(xv(0, 8), Camera::internal);
        cam.Set(xv(xo, xo + 5), Camera::external);
//    cout << "referenzpunkte berechnen" << endl;
        int ref = pl[i].rows();
//    cout << ref << "Referenzpunkte" << endl;
        for (j = 0; j < ref; j++)
          {
            Vector w(3);
            Vector b(2);
            Vector d(2);
            b[0] = pl[i][j][0];
            b[1] = pl[i][j][1]; // Bildkoordinaten der Marker
            w[0] = pl[i][j][2];
            w[1] = pl[i][j][3];
            w[2] = 0; // Weltkoordinaten der Vorlage
//    cout << w << endl;
            d = b - cam.Transform(w);
//      cout << "Ergebnis schreiben " << i << " " << j << endl;
            result[id++] = d[0];
            result[id++] = d[1];
          }

      }
    return 1;
  }
};

int KalibFehler(const Vector& xv, Vector& result) // Fehlerfunktion (von LMDif)
{
  int anz = RoundInt(xv[0]);
//  cout << anz << " Kameras" << endl;
  int id = 0;
  int iref = 10 + 6 * anz;
  int i, j;
//  cout << "Fehlerfunktion" << endl;
  for (i = 0; i < anz; i++)
    {
      Camera cam;
//    cout << "cam lesen" << endl;
      int xo = 10 + 6 * i;
      cam.Set(xv(1, 9), Camera::internal);
      cam.Set(xv(xo, xo + 5), Camera::external);
//    cout << "referenzpunkte berechnen" << endl;
      int ref = RoundInt(xv[iref++]);
//    cout << ref << "Referenzpunkte" << endl;
      for (j = 0; j < ref; j++)
        {
          Vector w(3);
          Vector b(2);
          Vector d(2);
          b[0] = xv[iref++];
          b[1] = xv[iref++]; // Bildkoordinaten der Marker
          w[0] = xv[iref++];
          w[1] = xv[iref++];
          w[2] = 0; // Weltkoordinaten der Vorlage
          d = b - cam.Transform(w);
//      cout << "Ergebnis schreiben " << i << " " << j << endl;
          result[id++] = d[0];
          result[id++] = d[1];
        }
    }
  return 1;
}

int DoKalibO(const vector<Matrix>& pl, int u0, int v0)
{
  if (pl.size() < 3)
    {
      Printf("Zu wenig Referenzen");
      return ERROR;
    }

  vector<Camera> cam(pl.size());

  Vector param(0); // Vektor für alle Parameter der Optimierung
  IVector oparam(0); // zu optimierende Parameter

  int i = 0;

  cam[0].Set(Vector(u0, v0), Camera::hauptpunkt);
  param.Append(cam[0].MakeVector(Camera::internal)); // interne Parameter (0..9)
  i = param.size();
  unsigned int j;
  for (j = 0; j < cam.size(); j++)
    {
      cam[j].Set(Vector(0, 0, -1000, 0, 0, 0), Camera::external); // Startwerte für ext. Parameter
      param.Append(cam[j].MakeVector(Camera::external));
    }
  while (i < param.size()) oparam.Append(i++); // externe Parameter sofort optimieren

//  cout << param << endl;
//  cout << oparam << endl;

  int references = 0;
  for (i = 0; i < (int)pl.size(); i++) references += pl[i].rows(); // Referenzpunkt-Anzahl

  Print("LMDiff nur extern: ");

  int iterationen = 10000;
  int info;
  KalibFunk kf(pl);
  info = LMDif(param, oparam, kf, references * 2, iterationen);


  LMResult(info);

//   for (j=0;j<cam.size();j++)
//   {
//     cam[j].Set(param(1,9),Camera::internal);
//     cam[j].Set(param(10+6*j,10+6*j+5),Camera::external);
//     cout << cam[j].ToString("\n") << endl;
//   }

  // im zweiten Lauf innere Kameraparameter dazu
  Print("LMDiff extern+f: ");
  oparam.Append(0); // f
//  oparam.Append(3); // u0
//  oparam.Append(4); // v0

  iterationen = 10000;
  info = LMDif(param, oparam, kf, references * 2, iterationen);

  LMResult(info);

// im nächsten Lauf Verzeichnung dazu

  Print("LMDiff extern+f+Verzeichung: ");
  oparam.Append(5); //
  oparam.Append(6); //
  oparam.Append(7); //
  oparam.Append(8); //

  iterationen = 10000;
  info = LMDif(param, oparam, kf, references * 2, iterationen);

  LMResult(info);

// im nächsten Lauf Hauptpunkt dazu
  Print("LMDiff extern+f+Hauptpunkt+Verzeichnung: ");
  oparam.Append(3); // u0
  oparam.Append(4); // v0

  iterationen = 10000;
  info = LMDif(param, oparam, kf, references * 2, iterationen);

  LMResult(info);


  cout << "------------------------------" << endl;
  for (j = 0; j < cam.size(); j++)
    {
      cam[j].Set(param(0, 8), Camera::internal);
      cam[j].Set(param(9 + 6 * j, 9 + 6 * j + 5), Camera::external);
      cout << cam[j].toString("\n") << endl;
    }
  cout << "------------------------------" << endl;
  if (wait) GetChar();
  return OK;
}

int DoKalib(const vector<Matrix>& pl)
{
  if (pl.size() < 3)
    {
      Printf("Zu wenig Referenzen");
      return ERROR;
    }

  vector<Camera> cam(pl.size());

  Vector param(0); // "Riesen"-Vektor für alle Parameter der Optimierung
  IVector oparam(0); // zu optimierende Parameter
  int i = 0;
  int references = 0;

  param.Append(pl.size()); // zahl der Positionen (Bilder)

  i = param.size();
  param.Append(cam[0].MakeVector(Camera::internal));

  i = param.size();
  unsigned int j;
  for (j = 0; j < cam.size(); j++)
    {
      cam[j].Set(Vector(0, 0, -1000, 0, 0, 0), Camera::external);
      param.Append(cam[j].MakeVector(Camera::external));
    }
  while (i < param.size()) oparam.Append(i++);

  cout << param << endl;
  cout << oparam << endl;

  for (j = 0; j < pl.size(); j++)
    {
      // cout << pl[j] << endl;
      param.Append(pl[j].rows()); // Anzahl der Referenzpunktpaare
      param.Append(MakeVector(pl[j]));
      references += pl[j].rows(); // Zahl der Referenzpunktpaare
    }

  cout << "LMD-Aufruf 1 " << endl;
  int iterationen = 10000;
  int info;
  info = LMDif(param, oparam, KalibFehler, references * 2, iterationen);

  cout << "LMD 1 fertig: "  << endl;
  LMResult(info);

//   for (j=0;j<cam.size();j++)
//   {
//     cam[j].Set(param(1,9),Camera::internal);
//     cam[j].Set(param(10+6*j,10+6*j+5),Camera::external);
//     cout << cam[j].ToString("\n") << endl;
//   }

  // im zweiten Lauf innere Kameraparameter dazu

  oparam.Append(1); // f
  oparam.Append(4); // u0
  oparam.Append(5); // v0

  cout << "LMD-Aufruf 2 " << endl;
  iterationen = 10000;
  info = LMDif(param, oparam, KalibFehler, references * 2, iterationen);

  cout << "LMD fertig: "  << endl;
  LMResult(info);

// im dritten Lauf Verzeichnung dazu

  oparam.Append(6); //
  oparam.Append(7); //
  oparam.Append(8); //
  oparam.Append(9); //

  cout << "LMD-Aufruf 3 " << endl;
  iterationen = 10000;
  info = LMDif(param, oparam, KalibFehler, references * 2, iterationen);

  cout << "LMD fertig: "  << endl;
  LMResult(info);

  for (j = 0; j < cam.size(); j++)
    {
      cam[j].Set(param(1, 9), Camera::internal);
      cam[j].Set(param(10 + 6 * j, 10 + 6 * j + 5), Camera::external);
      cout << cam[j].toString("\n") << endl;
    }
  return OK;
}

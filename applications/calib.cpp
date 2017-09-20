#include <fstream>
#include <sstream>
#include <image.h>
#include <Distortion.h>
#include <float.h>
#include <lmdif.h>
#include <qrdecomp.h>
#include <math.h>
#include <camera.h>
#include <lineseg.h>

#define SSIZE 256
#define SS2 (SSIZE/2)
#define SS2a (SSIZE/2-1)

#define F_START_WERT 2131

#define ReferenzPunkte 37

// index of next/previous point in reference point list
int nextpoint(int i)
{
  return (i + 1) %  ReferenzPunkte;
}
int prevpoint(int i)
{
  return (i + ReferenzPunkte - 1) % ReferenzPunkte;
}

using namespace ice;

// sets of parameters for optimization

IVector ps_extern = IVector(CAM_DX, CAM_DY, CAM_DZ, CAM_ALPHA, CAM_BETA, CAM_GAMMA);
IVector ps1 = IVector(0) && CAM_BRENNWEITE && CAM_DX && CAM_DY && CAM_DZ &&
              CAM_ALPHA && CAM_BETA && CAM_GAMMA;
IVector ps2 = IVector(0) && CAM_HP_U && CAM_HP_V && CAM_DX && CAM_DY && CAM_DZ &&
              CAM_ALPHA && CAM_BETA && CAM_GAMMA && CAM_ANISOTROPIE && CAM_SCHERUNG && CAM_BRENNWEITE;
IVector ps3 = IVector(0) && CAM_DX &&  CAM_DY && CAM_DZ && CAM_ALPHA && CAM_BETA && CAM_GAMMA &&
              CAM_HP_U && CAM_HP_V && CAM_BRENNWEITE && CAM_DIST_X && CAM_DIST_Y && CAM_DIST_D2 && CAM_DIST_D4;
IVector ps4 = IVector(0) && CAM_DX &&  CAM_DY && CAM_DZ && CAM_ALPHA && CAM_BETA && CAM_GAMMA &&
              CAM_HP_U && CAM_HP_V && CAM_ANISOTROPIE && CAM_SCHERUNG && CAM_BRENNWEITE &&
              CAM_DIST_X && CAM_DIST_Y && CAM_DIST_D2 && CAM_DIST_D4;

Camera Calib_Lin_A30(int dtyp);

void LMResult(int info)
{
  int ok = ((info >= 1) && (info <= 4));
  Printf(" Ergebnis (%d): ", info);
  if (ok) Print("OK\n");
  else Print("Failed\n");
  if (!ok) Print(LMDifMessage(info) + "\n");
}

// ***  use of IVector as set ***
// is val Element of p
int has(const IVector &p, int val)
{
  unsigned int i = 0;
  while (i < p.size())
    {
      if (p[i] == val) return true;
      i++;
    }
  return false;
}

// add pi to set p
void add(IVector &p, int pi)
{
  if (!has(p, pi)) p.Append(pi);
}

// add elements if c to set p
void add(IVector &p, const IVector &c)
{
  for (unsigned int i = 0; i < c.size(); i++) add(p, c[i]);
}

// remove pi from set p
void sub(IVector &p, int pi)
{
  if (has(p, pi))
    {
      int j = 0;
      while (p[j] != pi) j++;
      p.Delete(j);
    }
}

void sub(IVector &p, const IVector &c)
{
  for (unsigned int i = 0; i < c.size(); i++) sub(p, c[i]);
}

// ----------------------------------

void showcamera(const Camera &c)
{
  Print(c.toString("\n"));
}
//-------------------------------------------------------
// coordinates of markers
//
const Matrix CalibPoint = Matrix(0, 3) &&
                          Vector(20.0, 60.0,  0.0) &&
                          Vector(100.0, 60.0,  0.0) &&
                          Vector(180.0, 60.0,  0.0) &&
                          Vector(260.0, 60.0,  0.0) &&
                          Vector(20.0, 140.0,  0.0) &&
                          Vector(100.0, 140.0,  0.0) &&
                          Vector(180.0, 140.0,  0.0) &&
                          Vector(260.0, 140.0,  0.0) &&
                          Vector(20.0, 220.0,  0.0) &&
                          Vector(100.0, 220.0,  0.0) &&
                          Vector(180.0, 220.0,  0.0) &&
                          Vector(260.0, 220.0,  0.0) &&
                          Vector(20.0,  0.0, 20.0) &&
                          Vector(20.0,  0.0, 100.0) &&
                          Vector(20.0,  0.0, 180.0) &&
                          Vector(20.0,  0.0, 260.0) &&
                          Vector(100.0,  0.0, 20.0) &&
                          Vector(100.0,  0.0, 100.0) &&
                          Vector(100.0,  0.0, 180.0) &&
                          Vector(100.0,  0.0, 260.0) &&
                          Vector(180.0,  0.0, 20.0) &&
                          Vector(180.0,  0.0, 100.0) &&
                          Vector(180.0,  0.0, 180.0) &&
                          Vector(180.0,  0.0, 260.0) &&
                          Vector(260.0,  0.0, 20.0) &&
                          Vector(260.0,  0.0, 100.0) &&
                          Vector(260.0,  0.0, 180.0) &&
                          Vector(260.0,  0.0, 260.0) &&
                          Vector(0.0, 60.0, 60.0) &&
                          Vector(0.0, 60.0, 140.0) &&
                          Vector(0.0, 60.0, 220.0) &&
                          Vector(0.0, 140.0, 60.0) &&
                          Vector(0.0, 140.0, 140.0) &&
                          Vector(0.0, 140.0, 220.0) &&
                          Vector(0.0, 220.0, 60.0) &&
                          Vector(0.0, 220.0, 140.0) &&
                          Vector(0.0, 220.0, 220.0);

Point Transform2Point(Camera &k, const Vector &v)
{
  Vector vh = k.Transform(v);
  return Point(vh);
}

// Fehler-Funktion für LMDiff
int CalibFehler(const Vector &xv, Vector &result)
{
  int i;
  int referenzen = (xv.Size() - 15) / 2; // Zahl der vorliegenden Referenzpunkte

  // innere Kameraparameter
  Camera cam(xv(0, 14));

  Vector uv;
//  cout << "K "<< cam.toString() <<  endl;
  int hindex = 15; // zeiger auf Koordinaten
  int rindex = 0; // Index im Ergebnis-Fehler-Vektor
  for (i = 0; i < referenzen; i++)
    {
      // 3D-Punkt transformieren

      if (xv[hindex] >= 0.0)
        {
          uv = cam.Transform(CalibPoint[i]);
          result[rindex++] = uv[0] - xv[hindex++];
          result[rindex++] = uv[1] - xv[hindex++];
        }
      else hindex += 2;
    }
//  cout << ".K" << endl;
  return 1;
}

Vector vektor(const Matrix &xy, const Camera &camera)
{
  int n;
  Vector res = camera.MakeVector();

  for (n = 0; n < xy.rows(); n++)
    {
      res.Append(xy[n][0]);
      res.Append(xy[n][1]);
    }
  return res;
}

// Zahl (gültiger) Referenz-Punkte
int validpoints(const Matrix &xy)
{
  int num = 0;
  for (int ct = 0; ct < xy.rows(); ct++)
    {
      if (xy[ct][0] >= 0.0)
        {
          num++;
        }
    }
  return num;
}

void Solve(const Matrix &xy_o, Camera &camera, const IVector &var)
{
  int numref;
  int ct;
  int info;
  int inumber;

  numref = validpoints(xy_o);

  Vector para = vektor(xy_o, camera);

  Printf("\nOptimierung ");

  ct = 0;
  do
    {
      ct++;
      info = LMDif(para, var, CalibFehler, numref * 2, inumber, 100000);
      Print(".");
    }
  while ((ct < 79) && (info == 5) && (GetKey() == 0));
  ct--;
  inumber += ct * 10000;

  if (info != 2)
    {
      LMResult(info);
      Printf("\n nach %d Iterationen\n", inumber);

      GetChar();
    }
  camera = Camera(para);
  return;
}

int inside(int x, int y, const Image &img)
{
  if (x < 0) return false;
  if (y < 0) return false;
  if (x >= img->xsize) return false;
  if (y >= img->ysize) return false;
  return true;
}

int inside(double x, double y, const Image &img)
{
  if (x < 0.0) return false;
  if (y < 0.0) return false;
  if (x >= img->xsize - 1) return false;
  if (y >= img->ysize - 1) return false;
  return true;
}

// Marker eines referenzpunktes in kleines Bild kopieren
void copyref(const Matrix &xy, int n, const Image &big, Image &small)
{
  int x1, y1;
  int x, y;
  x1 = RoundInt(xy[n][0] - SS2);
  y1 = RoundInt(xy[n][1] - SS2);
  for (y = 0; y < small->ysize; y++)
    for (x = 0; x < small->xsize; x++)
      {
        if (inside(x + x1, y + y1, big))
          PutVal(small, x, y, GetVal(big, x + x1, y + y1));
        else
          PutVal(small, x, y, 0);
      }
}

#if 1
void RectifyMarker(Image i, const Matrix &ecken, Image &res)
{
  Matrix orig(0, 2);
  orig.Append(Vector(10, 10));
  orig.Append(Vector(30, 10));
  orig.Append(Vector(30, 30));
  orig.Append(Vector(10, 30));
  Trafo Tr = MatchPointlists(ecken, orig);
  Transform(Tr, i, res, INTERPOL);
}

double Validate(Image i, const Matrix &ecken, Image &himg)
{
  int ii;
  int ct = 0;
  int gsum = 0;
  RectifyMarker(i, ecken, himg);
  for (ii = 1; ii < 40; ii++)
    {
      gsum = gsum + GetVal(himg, 10, ii);
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      gsum = gsum + GetVal(himg, 30, ii);
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      gsum = gsum + GetVal(himg, ii, 10);
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      gsum = gsum + GetVal(himg, ii, 30);
      ct++;
    }
//  Print("*"+NumberString(((double)gsum)/ct));
//  GetChar();
  return ((double)gsum) / ct;
}
#endif

double Validate(const Image &img, const Matrix &ecken)
{
  Matrix orig(0, 2);
  orig.Append(Vector(10, 10));
  orig.Append(Vector(30, 10));
  orig.Append(Vector(30, 30));
  orig.Append(Vector(10, 30));
  Trafo Tr = MatchPointlists(ecken, orig);
  Tr.Invert();
  int ii;
  int ct = 0;
  double gsum = 0;
  double x, y;
  for (ii = 1; ii < 40; ii++)
    {
      Transform(Tr, 10, ii, x, y);
      if (inside(x, y, img))
        {
          gsum = gsum + GetInterpolVal(img, x, y);
        }
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      Transform(Tr, 30, ii, x, y);
      if (inside(x, y, img))
        {
          gsum = gsum + GetInterpolVal(img, x, y);
        }
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      Transform(Tr, ii, 10, x, y);
      if (inside(x, y, img))
        {
          gsum = gsum + GetInterpolVal(img, x, y);
        }
      ct++;
    }
  for (ii = 1; ii < 40; ii++)
    {
      Transform(Tr, ii, 30, x, y);
      if (inside(x, y, img))
        {
          gsum = gsum + GetInterpolVal(img, x, y);
        }
      ct++;
    }
//  Print("*"+NumberString(((double)gsum)/ct));
//  GetChar();
  return gsum / ct;
}

int Better(const Image &i, Matrix &ecke, double &bestval,
           int k1, int k2, double dval)
{
  Matrix hecke = ecke;
  ecke[k1][k2] += dval;
  double val = Validate(i, ecke);
  if (val > bestval)
    {
//    Print("+");
      bestval = val;
      return true;
    }
  else
    {
//    Print("-");
      ecke = hecke; // Wiederherstellen
      return false;
    }
}

void Optimize(const Image &i, Matrix &ecke)
{
  double cval = 0.5;
  double bestval = Validate(i, ecke);
//  Print("Bewertung vorher:" + NumberString(bestval)+"\n");
  while (cval > 0.04)
    {
      int changed = true;
      while (changed)
        {
          changed = false;
          int k1, k2;
          for (k1 = 0; k1 < 4; k1++)
            for (k2 = 0; k2 < 2; k2++)
              {
                if (Better(i, ecke, bestval, k1, k2, -cval))
                  {
                    changed = true;
                    while (Better(i, ecke, bestval, k1, k2, -cval));
                  }
                else
                  {
                    if (Better(i, ecke, bestval, k1, k2, cval))
                      {
                        changed = true;
                        while (Better(i, ecke, bestval, k1, k2, cval));
                      }
                  }
              }
        }
//Print("###");
      cval /= 2;
    }
//Print("Bewertung hinterher:" + NumberString(bestval)+"\n");
//GetChar();
}

int FindMarkerFine(Image i, Image m, Camera &camera, int idx, Vector &xy1)
{
//  cout << "find.." << fmode << endl;
  // Kalibrier-Punkt aus Liste lesen
  double x = CalibPoint[idx][0];
  double y = CalibPoint[idx][1];
  double z = CalibPoint[idx][2];

  double dd = 10.0;
  Matrix uv(4, 2);
  // Je nach der Ebene des Punktes Lage der Markierung
  // rekonstruieren
  if (z == 0.0)
    {
      uv[0] = camera.Transform(Vector(x - dd, y - dd, z));
      uv[1] = camera.Transform(Vector(x + dd, y - dd, z));
      uv[2] = camera.Transform(Vector(x + dd, y + dd, z));
      uv[3] = camera.Transform(Vector(x - dd, y + dd, z));
    }
  if (y == 0.0)
    {
      uv[0] = camera.Transform(Vector(x - dd, y, z - dd));
      uv[1] = camera.Transform(Vector(x + dd, y, z - dd));
      uv[2] = camera.Transform(Vector(x + dd, y, z + dd));
      uv[3] = camera.Transform(Vector(x - dd, y, z + dd));
    }
  if (x == 0.0)
    {
      uv[0] = camera.Transform(Vector(x, y - dd, z - dd));
      uv[1] = camera.Transform(Vector(x, y - dd, z + dd));
      uv[2] = camera.Transform(Vector(x, y + dd, z + dd));
      uv[3] = camera.Transform(Vector(x, y + dd, z - dd));
    }

  // Entsprechend Aufnahme anpassen
  Optimize(i, uv);

  // Mittelpunkt berechnen
  Intersect(uv[0], uv[2], uv[1], uv[3], xy1);

//  cout << uv << endl;
//  cout << xy1 << endl;

  double dist = (camera.Transform(CalibPoint[idx]) - xy1).Length();
  Print(NumberString(dist) + " ");
  // Abstandswerte Werte über 3.0 als falsch bewerten
  if (dist < 3.0)
    return OK;
  return ERROR;
}

int FindMarker(const Image &i, const Image &m, Vector &xy1, bool finetuning)
{
//  SetWindowImg(i, RoundInt(xy1[0]-32),RoundInt(xy1[1]-32),
//	        RoundInt(xy1[0]+32),RoundInt(xy1[1]+32));
//  int thrs=CalcThreshold(i);
//  thrs=(thrs+GetVal(i,RoundInt(xy1[0]),RoundInt(xy1[1])))/2;

  int thrs = GetVal(i, RoundInt(xy1[0]), RoundInt(xy1[1])) + 15;

//	Printf("Schwelle:%d\n",thrs);
//	GetChar();
//  SetWindowImg(i);
  int ps[2];
  ClearImg(m);
  ps[0] = RoundInt(xy1[0]);
  ps[1] = RoundInt(xy1[1]);
  if (OK != SearchStart(i, m, NULL, thrs, 1, ps)) return ERROR;
  Contur c = CalcContur(i, m, NULL, thrs, ps);
  if (!c.isValid()) return ERROR;
  if (!c.isClosed()) return ERROR;
  double moment[15];
  double s[2];
  MomentRegion(c, moment, s);
  if (!finetuning)
    {
      xy1[0] = s[0];
      xy1[1] = s[1];
      return OK;
    }

  Matrix ecke;
  FitParallelogramMoments(moment, ecke);
#if 0
  Image himg = NewImg(41, 41, 255);
  RectifyMarker(i, ecke, himg);
  Show(ON, himg);
  Zoom(himg);
#endif
  Optimize(i, ecke);
  Intersect(ecke[0], ecke[2], ecke[1], ecke[3], xy1);

//  GetChar();
  Zoom(i);
//  Show(OFF,himg);
//  FreeImg(himg);
  return OK;
}

void MarkRefs(const Matrix &xy, Image mark, Camera cam, int aref)
{
  int i;
  int size;
  Vector c1, c2;
  Point p1, p2;
  ClearImg(mark);
  for (i = 0; i < xy.rows(); i++)
    {
      c2 = cam.Transform(CalibPoint[i]);
      p2 = Point(c2);
      if (aref == i) size = 19;
      else size = 9;

      OffMessage();
      Marker(3, p2, 2, size, mark);
      SetOk();
      OnMessage();

      if (xy[i][0] >= 0.0)
        {
          c1 = xy[i];
          p1 = Point(c1);
          OffMessage();
          Marker(3, Point(c1), 1, size, mark);
          SetOk();
          OnMessage();
//      cout << c1 << c2 << endl;
          if ((c2 - c1).Length() < 10000)
            {
              if ((c2 - c1).Length() < 2)
                {
                  Draw(LineSeg(p1, Point((c2 - c1) * 20 + c1)), mark, 4);
                }
              else
                Draw(LineSeg(p1, p2), mark, 3);
            }
        }
    }
}

// ---- Globale Variablen --------------------------------------------

Matrix xy(ReferenzPunkte, 2);
int changed = false;

string fn;
string model = "Unbekannt";
double f = 0.0;

int apunkt = 0;
int pmode = 1; // Modus Punkteingabe: manuell, contur, Finetuning (default: contur)

Image i;
Image m;
Image hi;
Image hm;

// ------------------- "Globale Funktionen" ---------------------------

void ShowBig()
{
  Show(OFF, hi);
  Show(OVERLAY, i, m);
  Zoom(hi, 0, 0, 0);
}

void ShowSmall()
{
  Show(OFF, i);
  Show(OVERLAY, hi, hm);
  Zoom(hi, 0, 0, 0);
}

void ShowRef(const Matrix &xy, int ref)
{
  copyref(xy, ref, i, hi);
  copyref(xy, ref, m, hm);
}

Camera startcamera(Image img, int typ)
{
  // Startwerte für Kameraparameter
  Camera start;

  start.Set(F_START_WERT,   // Brennweite
            1.0,           // Anisotropie-Faktor
            0.0,           //Scherung
            img->xsize / 2, img->ysize / 2 // Hauptpunkt
           );

  start.SetExt(1000.0, 1000.0, 1000.0,// Verschiebung der Kamera
               0.0, Arcus(135), Arcus(147)); // Rotation (Euler-Winkel)


  switch (typ)
    {
    case 1:
      start.SetDist(Distortion1(img->xsize / 2, img->ysize / 2, 0.0, 0.0));
      break;
    case 2:
      start.SetDist(Distortion2(img->xsize / 2, img->ysize / 2, 0.0, 0.0, 0.0));
      break;
    case 3:
      start.SetDist(Distortion3(img->xsize / 2, img->ysize / 2, 0.0, 0.0, 0.0));
      break;
    }

  return start;
}

int readproject(const string &fnp)
{
  ifstream prj;
  prj.open(fnp.c_str());

  if (!prj.fail())
    {
      IfFailed(prj >> xy)
      {
        SetOk();
        xy = Matrix(ReferenzPunkte, 2);
        xy.Set(-1);
      }
    }
  else
    {
      xy = Matrix(ReferenzPunkte, 2);
      xy.Set(-1);
      return ERROR;
    }
  return OK;
}

int writeproject(const string &fnp)
{
  ofstream prj;
  prj.open(fnp.c_str());
  if (!prj.fail())
    {
      prj << xy;
      return OK;
    }
  else return ERROR;
}

void ParMenu()
{
  Printf("Zu optimierende Parameter\n\n");
  Printf(" 0 - Set 0 = nur ext.\n");
  Printf(" 1 - Set 1 = ext. + Brennweite\n");
  Printf(" 2 - Set 2 = ext. + innere\n");
  Printf(" 3 - Set 3 = alles bis auf Scherung und Anisotropie\n");
  Printf(" 4 - Set 4 = alles\n");
  Printf("Einzelne Parameter schalten:\n");
  Printf(" E/e - Externe Parameter an/aus\n");
  Printf(" A/a - Anisotropie an/aus\n");
  Printf(" S/s - Scherung an/aus\n");
  Printf(" H/h - Hauptpunkt an/aus\n");
  Printf(" D/d - Verzeichnung an/aus\n\n");
  Printf(" X,Q,<ENTER> - Exit\n\n");
}

void ParAuswahl(const Camera &camera, IVector &para)
{
  int abort = false;
  while (!abort)
    {
      ClearAlpha();
      ParMenu();
      showcamera(camera);
      int c = GetChar();
      switch (c)
        {
        case '0':
          para = ps_extern;
          break;
        case '1':
          para = ps1;
          break;
        case '2':
          para = ps2;
          break;
        case '3':
          para = ps3;
          break;
        case '4':
          para = ps4;
          break;
        case 'A':
          add(para, CAM_ANISOTROPIE);
          break;
        case 'a':
          sub(para, CAM_ANISOTROPIE);
          break;
        case 'F':
          add(para, CAM_BRENNWEITE);
          break;
        case 'f':
          sub(para, CAM_BRENNWEITE);
          break;
        case 'S':
          add(para, CAM_SCHERUNG);
          break;
        case 's':
          sub(para, CAM_SCHERUNG);
          break;
        case 'E':
          add(para, ps_extern);
          break;
        case 'e':
          sub(para, ps_extern);
          break;
        case 'H':
          add(para, IVector(CAM_HP_U, CAM_HP_V));
          break;
        case 'h':
          sub(para, IVector(CAM_HP_U, CAM_HP_V));
          break;
        case 'D':
          add(para, IVector(CAM_DIST_X, CAM_DIST_Y, CAM_DIST_D2, CAM_DIST_D4));
          break;
        case 'd':
          sub(para, IVector(CAM_DIST_X, CAM_DIST_Y, CAM_DIST_D2, CAM_DIST_D4));
          break;
        case 'q':
        case 'Q':
        case 13:
        case 'x':
        case 'X':
          abort = true;
          break;
        }
    }
}

void MainMenu(int pmode)
{
  int i, ct;

  for (i = 0, ct = 0; i < xy.rows(); i++)
    if (xy[i][0] >= 0.0) ct++;

  SetAlphaCursor(0, 0);
  Print("Bild " + fn + " Punkt: " + NumberString(apunkt) + "\n");
  Print("Kamera: " + model + " Brennweite: " + NumberString(f) + "\n");

  switch (pmode)
    {
    case 0:
      Print("manuell      ");
      break;
    case 1:
      Print("contur       ");
      break;
    case 2:
      Print("fine         ");
      break;
    }

  Printf("%d/%d Referenzpunkte\n\n", ct, xy.rows());

  Printf("A - aktuellen Punkt neu eingeben\n");
  Printf("M - Punkt automatisch finden\n");
  Printf("D - aktuellen Punkt löschen\n");
//  Printf("+/- - nächster/vorhergehender Punkt\n");
  Printf("C - Löschen der Punkt-Liste\n");
  Printf("N - Alle Punkte \"verfeinern\"\n");

  Printf("B - Berechnung Kameraparameter\n");
  Printf("L - Berechnung Kameraparameter nach linearer Methode\n");
  Printf("V - Variable Parameter für Optimierung\n");
  Printf("S - Setze Startwerte Kamera\n");

  Printf("W - Schreiben des Projektes\n");
//  Printf("R - Projekt neu lesen\n");
  Printf("P - Parameter in \"calib.txt\" schreiben\n");

  Printf("X,Q - Exit\n\n");
}

void usage(const string &pn)
{
  cout << pn << " - Kalibrieren mit Kalibrierkörper" << endl;
  cout << "Aufruf : " << endl;
  cout << pn << " <options> <dateiname>" << endl;
  cout << "Optionen:" << endl;

  cout << "-e <fn> Ausgabe des entzeichneten Bildes als Datei fn nach Programmende" << endl;

  cout << "Vorverarbeitung:" << endl;
  cout << "-s nn SmearImg mit Umgebung nn" << endl;
  cout << "-b nn DoB-Filter anwenden, Aussenmaß nn" << endl;
  cout << "-B nn Innenmaß DoB-Filter nn (default:1)" << endl;
  cout << "-d nn Dilatation Umgebung nn" << endl;

  cout << "Ausgabe:" << endl;
  cout << "-e <fn> Ausgabe des entzerrten Bildes als Datei fn nach Programmende" << endl;
  cout << "-h      Diese Hilfe" << endl;

  exit(1);
}


int main(int argc, char *argv[])
{
  bool ready = FALSE;
  double dist = 100.0;
  int ctyp = 1;
  string efn;

  int dobs = 0;
  int dobi = 1;
  int dis = 0;
  int sms = 0;

  Vector xy1, xy2;
  IVector parset = ps3; // Parameter, die zu optimieren sind
  // Standard: Alles bis auf Anisotropie und Scherung
  Camera camera;
  char input;
  string project;

  //  cout << ps1 << endl;
  //  cout << ps2 << endl;
  //  cout << ps3 << endl;
  Alpha(ON);
  ClearAlpha();

  string::size_type pos;

  xy.Set(-1); // Alle korrespondenzen sind ungültig

  int rc;
  while ((rc = getopt(argc, argv, "he:b:B:d:s:")) >= 0)
    {
      switch (rc)
        {
        case 'e':
          efn = optarg;
          break;
        case 'd':
          dis = atol(optarg);
          break;
        case 'b':
          dobs = atol(optarg);
          break;
        case 'B':
          dobi = atol(optarg);
          break;
        case 's':
          sms = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  if (argc > optind)
    fn = argv[optind];
  else
    {
      fn = FileSel("*.jpg");
    }

  pos = fn.rfind(".");
  if (pos == string::npos) project = fn + ".prj";
  else project = fn.substr(0, pos) + ".prj";

  readproject(project);

  char line[300];
  string sline;
  FILE *p = popen(("jhead " + fn).c_str(), "r");
  while (fgets(line, 299, p) != NULL)
    {
      string::size_type spos;
      line[strlen(line)-1] = 0;
      sline = line;
      spos = sline.find(":");
      if (spos != string::npos)
        {
          string tag = sline.substr(0, spos - 1);
          string value = sline.substr(spos + 1);
          if (tag.find("Camera model") != string::npos)
            model = value;
          if (tag.find("Focal length") != string::npos)
            f = atof(value.c_str());
          //      cout << tag <<  "-*-" << value << endl;
        }
    }
  pclose(p);

  i = ReadImg(fn);
  if (dobs > 0)
    DoBImg(i, i, dobi, dobs);

  if (dis > 0)
    DilateImg(i, dis, dis, i);

  if (sms > 0)
    SmearImg(i, sms);

  camera = startcamera(i, ctyp);

  m = NewImg(i);
  ClearImg(m);
  hi = NewImg(SSIZE, SSIZE, 255);
  hm = NewImg(hi);

  Display(ON);
  Show(OVERLAY, i, m);
  Zoom(i, 0, 0, 0);
  Alpha(ON);
  ClearAlpha();
  do
    {
      ClearAlpha();
      MarkRefs(xy, m, camera, apunkt);
      MainMenu(pmode);
      showcamera(camera);
      if (validpoints(xy) > 0)
        {
          int ct = 0;
          dist = 0.0;
          for (int i = 0; i < ReferenzPunkte; i++)
            {
              if (xy[i][0] >= 0.0)
                {
                  ct++;
                  dist += (camera.Transform(CalibPoint[i]) - xy[i]).Length();
                }
            }
          dist = dist / ct;
          Printf("Distanz: %5.2f\n", dist);
        }

      input = GetChar();

      ClearAlpha();
      MainMenu(pmode);

      switch (input)
        {
          // Rücksetzen auf Kamera-Startwerte
        case 's':
        case 'S':
          camera = startcamera(i, ctyp);
          break;

          //
        case 'v':
        case 'V':
          ParAuswahl(camera, parset);
          break;

        case 'b':
        case 'B':
        {
          int p = validpoints(xy);
          if (p > 3)
            {
              //	  cout << parset << endl;
              Solve(xy, camera, parset);
            }
        }
        break;

        case 'l':
        case 'L':
        {
          Camera c1 = camera;
          camera = Calib_Lin_A30(ctyp);
          //	camera.alpha=c1.alpha;
          //	camera.beta=c1.beta;
          //	camera.gamma=c1.gamma;
          break;
        }

        //------- Punktauswahl --

        case '+':
        case '\t':
          apunkt = nextpoint(apunkt);
          break;

        case '-':
          apunkt = prevpoint(apunkt);
          break;

          // Schnellauswahl einzelner Punkte
        case '1':
          apunkt = 15;
          break;
        case '2':
          apunkt = 27;
          break;
        case '3':
          apunkt = 24;
          break;
        case '5':
          apunkt = 0;
          break;
        case '6':
          apunkt = 3;
          break;
        case '7':
          apunkt = 36;
          break;
        case '8':
          apunkt = 8;
          break;
        case '9':
          apunkt = 11;
          break;
          // -----------------------------------------

        case 'd':
        case 'D':
          xy[apunkt].Set(-1);
          changed = true;
          break;

        case 8:
          xy[prevpoint(apunkt)].Set(-1);
          break;

        case 'f':
        case 'F':
          pmode++;
          if (pmode > 2) pmode = 0;
          break;

        case 'a':
        case 'A':
        {
          Zoom(i);
          xy1 = (Vector)SelVector(DEFAULT, i);

          if (pmode == 0)
            {
              xy[apunkt] = xy1;
              apunkt = (apunkt + 1) % ReferenzPunkte;
              changed = true;
            }
          else

            if (FindMarker(i, m, xy1, pmode) == OK)
              {
                xy[apunkt] = xy1;
                apunkt = (apunkt + 1) % ReferenzPunkte;
                changed = true;
              }
          break;
        }

        case 'm':
        case 'M':
        {
          xy1 = camera.Transform(CalibPoint[apunkt]);
          {
            if (FindMarker(i, m, xy1, pmode == 2) == OK)
              {
                xy[apunkt] = xy1;
                apunkt = (apunkt + 1) % ReferenzPunkte;
                changed = true;
              }
          }
          break;
        }

        case 'N':
        case 'n':
          // Automatische Feindetektion für alle Punkte
        {
          if (dist < 4)
            {
              int idx;
              for (idx = 0; idx < ReferenzPunkte; idx++)
                {
                  if (FindMarkerFine(i, m, camera, idx, xy1) == OK)
                    {
                      Print("+");
                      xy[idx] = xy1;
                      changed = true;
                    }
                  else Print("-");
                  //	    usleep(1000000);
                }
            }
          else
            {
              Print("Ungenügender Grobabgleich!\n");
              GetChar();
            }
        }
        break;

        //      case 'k':
        //	SmearImg(i,i,25,25);
        //	break;

        case 'c':
        case 'C':
        {
          int c;
          Print("Punktliste löschen?");
          c = GetChar();
          if ((c == 'j') || (c == 'J') || (c == 'y') || (c == 'Y'))
            xy.Set(-1);
          changed = true;
        }
        break;
        //------------------------------
        case 'p':
        case 'P':
        {
          ofstream par;
          par.open("calib.txt", ofstream::out | ofstream::app);
          par << model << " f:" << f ;
          par << " file:" << fn << endl;
          par << "  " << camera.toString(" ") << endl;
        }
        break;
        //------------------------------
        case 'r':
        case 'R':
          readproject(project);
          changed = false;
          break;

        case 'w':
        case 'W':
        {
          writeproject(project);
          changed = false;
        }
        break;

        //------------------------------
        case 'e':
        case 'E':
          SmearImg(i, i, 3, 3);
          break;
        case 'q':
        case 'Q':
        case 'x':
        case 'X':
          ready = true;
          break;
        }
    }
  while (!ready);

  if (changed)
    {
      int c;
      Print("Punktliste speichern?");
      do
        {
          c = GetChar();
          if ((c == 'j') || (c == 'J') || (c == 'y') || (c == 'Y'))
            writeproject(project);
          changed = false;
        }
      while ((c != 'j') && (c != 'J') && (c != 'y') && (c != 'Y') && (c != 'n') && (c != 'N'));
    }
  Image h = camera.Dist().RectImg(i, INTERPOL);
  if (!efn.empty())
    WriteImg(h, efn);
  return 0;
}

int RotF(const Vector &xv, Vector &result)
{
  int i;
  Trafo tr(3, 3);

  tr.RotateZ(xv[2]);
  tr.RotateY(xv[1]);
  tr.RotateX(xv[0]);
//  cout << tr.Tmatrix() << endl;
  Vector r(3);
  r = tr * Vector(1, 0, 0);
  for (i = 0; i < 3; i++)
    result[i] = r[i] - xv[i+3];
  r = tr * Vector(0, 1, 0);
  for (i = 0; i < 3; i++)
    result[i+3] = r[i] - xv[i+6];
  r = tr * Vector(0, 0, 1);
  for (i = 0; i < 3; i++)
    result[i+6] = r[i] - xv[i+9];
//  cout << xv << endl;
//  cout << result << endl;
  return 1;
}

Camera Calib_Lin_A30(int ctyp)
{
  Camera res(ctyp);
  double f, an, s, u0, v0;
  double alpha, beta, gamma;
  double dx, dy, dz;
  int i;

  // Matrix xy(refrenzPunkte,2) ist global für die Bildpunkte
  // Matrix CalibPoint(RefrenzPunkte,3) enthält die 3D Koordinaten

  Matrix A(3, 3); // Kalibriermatrix 1. Teil
  Vector a(3);   // vektorieller Anteil der Kalibriermatrix (3,4)

  Matrix R_TRANS(3, 3); // Rotationsmatrix (transponiert)
  Matrix K(3, 3); // Obere Dreiecksmatrix

  Vector A_hilf(11); // Kalibriermatrix als Hilfsvektor

  // Zur linearen Kalibrierung wird der Parameter a(2) (a30) auf 1 gesetzt!
  // Dies ist die einfachste Restriktion

  Matrix Koeff_matrix(0, 11); //Meßdaten für alle 11 Parameter
  Vector Zeile_1(11); // Zeile zum Anhängen an die Matrix
  Vector Zeile_2(11); // Zeile zum Anhängen an die Matrix

  // Ein Referenzpunktepaar ergibt zwei Zeilen der Meßmatrix
  Vector B(0); // Vektor der rechten Seite der Meßdaten

  //  printf("Anzahl Referenzen: %d\n",xy.rows());
  for (i = 0; i < xy.rows(); ++i)
    {
      Zeile_1.Set(0.0);
      Zeile_2.Set(0.0);

      Zeile_1[0] = xy[i][0] * CalibPoint[i][0];
      Zeile_1[1] = xy[i][0] * CalibPoint[i][1];
      Zeile_1[2] = xy[i][0] * CalibPoint[i][2];
      Zeile_1[3] = -CalibPoint[i][0];
      Zeile_1[4] = -CalibPoint[i][1];
      Zeile_1[5] = -CalibPoint[i][2];
      Zeile_1[6] = -1.0;

      Zeile_2[0] = xy[i][1] * CalibPoint[i][0];
      Zeile_2[1] = xy[i][1] * CalibPoint[i][1];
      Zeile_2[2] = xy[i][1] * CalibPoint[i][2];
      Zeile_2[7] = -CalibPoint[i][0];
      Zeile_2[8] = -CalibPoint[i][1];
      Zeile_2[9] = -CalibPoint[i][2];
      Zeile_2[10] = -1.0;

      Koeff_matrix.Append(Zeile_1);
      Koeff_matrix.Append(Zeile_2);
      B.Append(-xy[i][0]);
      B.Append(-xy[i][1]);
    }

  A_hilf = SolveLinEqu(Koeff_matrix, B); // Ausgleichsproblem lösen

  A[2][0] = A_hilf[0];
  A[2][1] = A_hilf[1];
  A[2][2] = A_hilf[2];
  A[0][0] = A_hilf[3];
  A[0][1] = A_hilf[4];
  A[0][2] = A_hilf[5];
  A[1][0] = A_hilf[7];
  A[1][1] = A_hilf[8];
  A[1][2] = A_hilf[9];

  a[0] = A_hilf[6];
  a[1] = A_hilf[10];
  a[2] = 1.0;

//  cout << "L:" << A << Determinant(A) << endl;
//  cout << a << endl;

  {
    // RQ Decomposition
    Matrix K_INV;
    QRDecomposition(Inverse(A), R_TRANS, K_INV);
    K = Inverse(K_INV);

    Vector shift = R_TRANS * K_INV * a;
    dx = -shift[0];
    dy = -shift[1];
    dz = -shift[2];    // Verschiebung der Kamera
  }

//  cout << R_TRANS << Determinant(R_TRANS) << endl;

// cout << R_TRANS << Determinant(R_TRANS) << endl;
// cout << K_INV*A << endl;
//  cout << K << Determinant(K) << endl;

  // Normierung K-Matrix auf K33 = 1.0
  int  j;
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      {
        K[i][j] /= K[2][2];
      }

//  normalisierung
  if (K[1][1] > 0)
    {
      // zweite Zeile R negieren == zweite Spalte von R_TRANS
      R_TRANS[0][1] = -R_TRANS[0][1];
      R_TRANS[1][1] = -R_TRANS[1][1];
      R_TRANS[2][1] = -R_TRANS[2][1];
      K[0][1] = -K[0][1]; // zweite Spalte von K negieren
      K[1][1] = -K[1][1];
    }

  if (Determinant(R_TRANS) < 0) R_TRANS = -1 * R_TRANS;

  u0 = K[0][2];
  v0 = K[1][2];

  f = K[0][0];
  an = -K[1][1] / K[0][0];
  s = K[0][1] / K[0][0];

  // Rotation Kamera (Euler-Winkel) aus R_Trans ermitteln
  Vector Par(12);

  Par[0] = Arcus(151);
  Par[1] = Arcus(134);
  Par[2] = 0.1;
// Startwerte gamma, beta, alpha

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      Par[3+3*i+j] = R_TRANS[i][j];

  int inumber;
  int info = LMDif(Par, 3, RotF, 9, inumber, 10000);

  LMResult(info);
//  cout << info << endl;
//  cout << Par(0,2)*(180.0/3.14159) << endl;

  alpha = Par[2];
  beta = Par[1];
  gamma = Par[0];
  res.Set(f, an, s, u0, v0);
  res.Set(Vector(dx, dy, dz, alpha, beta, gamma), Camera::external);
  return res;
}

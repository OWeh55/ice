#include <fstream>
#include <float.h>
#include <lmdif.h>

#include <macro.h>
#include <image.h>

#define SSIZE 256
#define SS2 (SSIZE/2)
#define SS2a (SSIZE/2-1)

Image i1, i2;
Image m1, m2;
Image hi1, hm1;
Image hi2, hm2;

#define Z_START_WERT 10000
#define F_START_WERT 1000

void Ergebnis(int info)
{
  int ok = ((info >= 1) && (info <= 4));
  Printf("\nErgebnis (%d):", info);

  if (ok) Print("OK\n");
  else Print("Failed\n");

  if (!ok)
    {
      switch (info)
        {
        case 0:
          Print("improper input parameters");
          break;
        case 1:
          Print("both actual and predicted relative reductions in "
                "the sum of squares are at most ftol.");
          break;
        case 2:
          Print("relative error between two consecutive iterates is at most xtol.");
          break;
        case 3:
          Print("conditions for info = 1 and info = 2 both hold.");
          break;
        case 4:
          Print("the cosine of the angle between fvec and any column of the"
                " jacobian is at most gtol in absolute value.");
          break;
        case 5:
          Print("number of calls to fcn has reached or exceeded maxfev.");
          break;
          break;
        case 6:
          Print("ftol is too small. no further reduction in the sum of squares"
                " is possible.");
          break;
        case 7:
          Print("xtol is too small. no further improvement in the approximate "
                "solution x is possible.");
          break;
        case 8:
          Print("gtol is too small. fvec is orthogonal to the columns of the "
                "jacobian to machine precision.");
          break;
        }

      Print("\n");
    }
}

void showcamera(Vector& var)
{
  Printf(" f:%f ", var[0]);
  Printf(" s:%f ", var[1]);
  Printf(" a:%f ", var[2]);
  Printf(" u0:%f ", var[3]);
  Printf(" v0:%f\n", var[4]);

  Printf(" dx:%f ", var[5]);
  Printf(" dy:%f ", var[6]);
  Printf(" dz:%f\n", var[7]);

  Printf(" fx:%f ", Degree(var[8]));
  Printf(" fy:%f ", Degree(var[9]));
  Printf(" fz:%f\n\n", Degree(var[10]));
}

Trafo Kamera1(const Vector& par)
{
  Trafo tr(3, 3);

  // innere Kameraparameter
  double f = par[0];
  double s = par[1];
  double a = par[2];
  double u0 = par[3];
  double v0 = par[4];

  tr.projective();
  tr.shearX(s);
  tr.scale(0, 0, f, a * f);
  tr.shift(u0, v0);
  return tr;
}

Trafo Kamera2(const Vector& par)
{
  Trafo tr(3, 3);
  // innere Kameraparameter
  double f = par[0];
  double s = par[1];
  double a = par[2];
  double u0 = par[3];
  double v0 = par[4];

  // verschiebung der zweiten Kamera
  double dx = par[5];
  double dy = par[6];
  double dz = par[7];

  // Rotation der zweiten Kamera
  double gamma = par[8]; // gamma - fx
  double beta = par[9]; // beta - fy
  double alpha = par[10]; // alpha - fz

  // Rotation Kamera
  tr.rotateZ(alpha);
  tr.rotateY(beta);
  tr.rotateX(gamma);

  tr.shift(dx, dy, dz);

  tr.projective();
  tr.shearX(s);
  tr.scale(0, 0, f, a * f);
  tr.shift(u0, v0);
  return tr;
}

int StereoFehler(const Vector& x, Vector& result)
{
  int i;
  int referenzen = (x.Size() - 11) / 7;
  Trafo tr1 = Kamera1(x);
  Trafo tr2 = Kamera2(x);

  Vector x1(3);
  Vector y1(2), y2(2);
  Vector ym1(2), ym2(2);

  int hindex = 11; // zeiger auf 3d-Koordinaten
  int rindex = 11 + 3 * referenzen; // zeiger auf 2d-Koordinaten
  int vindex = 0;

  for (i = 0; i < referenzen; i++)
    {
      // 3D-Punkt auslesen
      x1[0] = x[hindex++];
      x1[1] = x[hindex++];
      x1[2] = x[hindex++];

      y1 = tr1 * x1;
      y2 = tr2 * x1;
      //      Printf("%f %f - %f %f\n",u1,v1,u2,v2);

      result[vindex++] = y1[0] - x[rindex++];
      result[vindex++] = y1[1] - x[rindex++];
      result[vindex++] = y2[0] - x[rindex++];
      result[vindex++] = y2[1] - x[rindex++];
      //if (*iflag==0)
      //  Printf("%e %e %e %e\n",u1-uu1[i],v1-vv1[i],u2-uu2[i],v2-vv2[i]);
    }

  return 1;
}

void Solve(Matrix xy12, Matrix& d3d, Vector& camera)
{
  int numref;
  int ct;
  numref = xy12.rows();

  Vector para(11 + numref * 7);

  IVector optvar(0);
  int info;
  int inumber;
  int i, n;

//  Printf("Parametervektor aufbauen (%d Elemente)\n",para.Size());
//  Printf("Startwerte für Iteration festlegen\n");
//  Printf(" Kamera-Parameter\n");
  // Startwerte für Kameraparameter
  para[0] = camera[0]; // Brennweite
  // optvar=optvar && 0;
  para[1] = camera[1]; //Scherung
  // optvar=optvar && 1;
  para[2] = camera[2]; // Anisotropie-Faktor
  // optvar=optvar && 2;

  para[3] = camera[3];
  para[4] = camera[4]; // Hauptpunkt
  optvar = optvar && 3;
  optvar = optvar && 4;

  para[5] = camera[5];
  para[6] = camera[6];
  para[7] = camera[7]; // Verschiebung der 2. Kamera
  optvar = optvar && 5;
  optvar = optvar && 6;
  optvar = optvar && 7;

  para[8] = camera[8];
  para[9] = camera[9];
  para[10] = camera[10]; // Rotation 2.Kamera (Euler-Winkel)
  optvar = optvar && 8;
  optvar = optvar && 9;
  optvar = optvar && 10;

  // Startwerte der 3d-Punkte
  // Printf(" 3D-Punkte\n");

  i = 11;

  for (n = 0; n < numref; n++)
    {
      if (n > -1)
        {
          optvar = optvar && i;
          para[i++] = d3d[n][0];
          optvar = optvar && i;
          para[i++] = d3d[n][1];
          optvar = optvar && i;
          para[i++] = d3d[n][2];
        }
      else
        {
          optvar = optvar && i;
          para[i++] = d3d[n][0];
          optvar = optvar && i;
          para[i++] = d3d[n][1];
          para[i++] = d3d[n][2]; // z-wert des ersten punktes festsetzen!
        }
    }

  // Gegebene Punktreferenzen
  for (n = 0; n < numref; n++)
    {
      para[i++] = xy12[n][0];
      para[i++] = xy12[n][1];
      para[i++] = xy12[n][2];
      para[i++] = xy12[n][3];
    }

  Printf("\nOptimierung ");

  ct = 0;

  do
    {
      ct++;
      inumber = 10000;
      info = LMDif(para, optvar, StereoFehler, numref * 4, inumber);
      Print(".");
    }
  while ((ct < 79) && (info == 5) && (GetKey() == 0));

  inumber += ct * 10000;

  Ergebnis(info);
  Printf("\nFertig nach %d Iterationen\n", inumber);

  for (i = 0; i < 11; i++) camera[i] = para[i];

  i = 11;

  for (n = 0; n < numref; n++)
    {
      d3d[n][0] = para[i++];
      d3d[n][1] = para[i++];
      d3d[n][2] = para[i++];
    }

  showcamera(camera);
  return;
}

int write3d(const Matrix& d3d)
{
  ofstream ref;
  ref.open("3d.txt");

  if (!ref.fail())
    {
      int i, j;

      for (i = 0; i < d3d.rows(); i++)
        {
          for (j = 0; j < 3; j++)
            ref << d3d[i][j] << " ";

          ref << endl;
        }

      return OK;
    }
  else return ERROR;
}

void ShowBig()
{
//  Display(RESET);  Display(ON);
  Show(OFF, hi1);
  Show(OFF, hi2);
  Show(OVERLAY, i1, m1);
  Show(OVERLAY, i2, m2);
}

void ShowSmall()
{
//  Display(RESET); Display(ON);
  Show(OFF, i1);
  Show(OFF, i2);
  Show(OVERLAY, hi1, hm1);
  Show(OVERLAY, hi2, hm2);
}

int inside(int x, int y, Image img)
{
  if (x < 0) return false;

  if (y < 0) return false;

  if (x >= img->xsize) return false;

  if (y >= img->ysize) return false;

  return true;
}

void copyref(const Matrix& xy, int ref, int n, Image big, Image small)
{
  int x1, y1;
  int x, y;
  x1 = RoundInt(xy[ref][n] - SS2);
  y1 = RoundInt(xy[ref][n + 1] - SS2);

  for (y = 0; y < small->ysize; y++)
    for (x = 0; x < small->xsize; x++)
      {
        if (inside(x + x1, y + y1, big))
          PutVal(small, x, y, GetVal(big, x + x1, y + y1));
        else
          PutVal(small, x, y, 0);
      }
}

#define BSIZE 32
#define BETA 0.1

int DetectShift1(Image img1, Image img2,
                 Vector& dxy, double& val)
{
  Image imgr;
  int x1, y1, x2, y2;
  dxy = Vector(2);
  int sx = BSIZE;
  int sy = BSIZE;
  x1 = img1->xsize / 2 - sx / 2;
  y1 = img1->ysize / 2 - sy / 2;
  x2 = x1 + BSIZE - 1;
  y2 = y1 + BSIZE - 1;
  Window w1(x1, y1, x2, y2);
  x1 = img2->xsize / 2 - sx / 2;
  y1 = img2->ysize / 2 - sy / 2;
  x2 = x1 + BSIZE - 1;
  y2 = y1 + BSIZE - 1;
  Window w2(x1, y1, x2, y2);

  imgr = NewImg(sx, sy, 255);

//  Show(ON,imgr);
  calcInvConvolution(img1(w1), img2(w2), imgr, 0, BETA, MD_IGNORE_BIAS);

  val = peakEvaluation(imgr, dxy[0], dxy[1]);

  dxy[0] -= sx / 2;
  dxy[1] -= sy / 2;
//  GetChar();
  return OK;
}

void MarkRefs(const Matrix& xy, Image m1, Image m2)
{
  int i;
  clearImg(m1);
  clearImg(m2);

  for (i = 0; i < xy.rows(); i++)
    {
      Marker(1, RoundInt(xy[i][0]), RoundInt(xy[i][1]), 1, 5, m1);
      Marker(1, RoundInt(xy[i][2]), RoundInt(xy[i][3]), 1, 5, m2);
    }
}

void ShowRef(const Matrix& xy, int ref)
{
  copyref(xy, ref, 0, i1, hi1);
  copyref(xy, ref, 0, m1, hm1);
  copyref(xy, ref, 2, i2, hi2);
  copyref(xy, ref, 2, m2, hm2);
}

// ---- Globale Variablen --------------------------------------------

Matrix xy12(0, 4);
Matrix d3d(0, 3);
string fn1, fn2;
string project = "stereo.prj";

// ------------------- "Globale Funktionen" ---------------------------

int readproject(const string& fn)
{
  ifstream ref;
  ref.open(fn.c_str());

  if (!ref.fail())
    {
      if (!(ref >> fn1))
        {
          fn1 = "pict_l.jpg";
        }
      if (!(ref >> fn2))
        {
          fn2 = "pict_r.jpg";
        }
      if (!(ref >> xy12))
        {
          xy12 = Matrix(0, 4);
        }

      if (xy12.rows() == 0)
        {
          xy12 = Matrix(0, 4);
        }

      if (!(ref >> d3d))
        {
          d3d = Matrix(0, 3);
          int i;

          for (i = 0; i < xy12.rows(); i++)
            d3d.append(Vector(0.0, 0.0, Z_START_WERT));
        }

      if (d3d.rows() == 0)
        {
          d3d = Matrix(0, 3);
        }

      return OK;
    }
  else
    {
      fn1 = "pict_l.jpg";
      fn2 = "pict_r.jpg";
      xy12 = Matrix(0, 4);
      d3d = Matrix(0, 3);
      return ERROR;
    }
}

int writeproject(const string& fn)
{
  ofstream ref;
  ref.open(fn.c_str());

  if (!ref.fail())
    {
      ref << fn1 << endl;
      ref << fn2 << endl;
      ref << xy12;
      ref << d3d;
      return OK;
    }
  else return ERROR;
}

void Modify()
{
  int i;
  int ready = FALSE;
  char c;
  Vector dxy;
  double val;

  Matrix xy12m(xy12);
  Matrix d3dm(d3d);
  ShowSmall();
  i = 0;

  while (!ready)
    {
      MarkRefs(xy12m, m1, m2);
      Printf("Referenz %d: %5.2f,%5.2f %5.2f,%5.2f\n",
             i, xy12m[i][0], xy12m[i][1], xy12m[i][2], xy12m[i][3]);
      ShowRef(xy12m, i);
      c = GetChar();

      switch (c)
        {
        case 's':
        case 'S':
          DetectShift1(hi1, hi2, dxy, val);
          Printf("Shiftdetection %5.2f dx=%5.2f dy=%5.2f\n", val, dxy[0], dxy[1]);

          if (val > 0.6)
            {
              xy12m[i] = Vector(xy12m[i][0], xy12m[i][1],
                                xy12m[i][2] + dxy[0], xy12m[i][3] + dxy[1]);
            }

          break;
        case 'd':
        case 'D':
          xy12m.DeleteRow(i);
          d3d.DeleteRow(i);

          if (i >= d3d.rows()) i--;

          if (i < 0) ready = true;

          Printf("Referenz gelöscht\n");
          break;
        case '2':
          clearImg(hm2);
          dxy = Vector(SelVector(DEFAULT, hi2));
          xy12m[i] = Vector(xy12m[i][0], xy12m[i][1],
                            xy12m[i][2] + dxy[0] - SS2, xy12m[i][3] + dxy[1] - SS2);
          break;
        case '1':
          clearImg(hm1);
          dxy = Vector(SelVector(DEFAULT, hi1));
          xy12m[i] = Vector(xy12m[i][0] + dxy[0] - SS2, xy12m[i][1] + dxy[1] - SS2,
                            xy12m[i][2], xy12m[i][3]);
          break;
        case 'n':
        case 'N':
        case '+':
        case ' ':

          if (i < d3d.rows() - 1) i++;

          break;
        case 'P':
        case 'p':
        case '-':

          if (i > 0) i--;

          break;
        case 'q':

          if (Ask("Änderungen übernehmen", "YN"))
            {
              xy12 = xy12m; //  Änderungen zuweisen
              d3d = d3dm;
            }

          ready = true;
          break;
        }
    }

  ClearAlpha();
  ShowBig();
}

Vector startcamera()
{
  Vector para(11);
  // Startwerte für Kameraparameter
  para[0] = F_START_WERT; // Brennweite
  para[1] = 0.0; //Scherung
  para[2] = 1.0; // Anisotropie-Faktor

  para[3] = 800;
  para[4] = 600; // Hauptpunkt

  para[5] = -10.0;
  para[6] = 0.0;
  para[7] = 0.0; // Verschiebung der 2. Kamera

  para[8] = 0.0;
  para[9] = 0.0;
  para[10] = 0.0; // Rotation 2.Kamera (Euler-Winkel)
  return para;
}

#define TESTSIZE 50
void TestKonfig(const Vector& cam)
{
  Trafo tr1 = Kamera1(cam);
  Trafo tr2 = Kamera2(cam);
  int i;
  d3d = Matrix(TESTSIZE, 3);

  for (i = 0; i < TESTSIZE; i++)
    {
      d3d[i][0] = (i % 5) * 100;
      d3d[i][1] = ((i / 5) % 5) * 100;
      d3d[i][2] = (i / 25) * 100 + 1000;
    }

  Vector p(1, 2);
  xy12 = Matrix(TESTSIZE, 4);

  for (i = 0; i < TESTSIZE; i++)
    {
      p = tr1 * d3d[i];
      xy12[i][0] = p[0];
      xy12[i][1] = p[1];
      p = tr2 * d3d[i];
      xy12[i][2] = p[0];
      xy12[i][3] = p[1];
    }

  project = "test.prj";
}

void MainMenu()
{
  SetAlphaCursor(0, 0);
  Print("Projekt " + project + "\n\n");
  Print("Bilder " + fn1 + " und " + fn2 + "\n\n");
  Printf("%d Referenzpunkte\n\n", xy12.rows());
  Printf("3 - 3D-Punkte neu initialisieren\n");
  Printf("A - Neuer Referenzpunkt\n");
  Printf("B - Berechnung\n");
  Printf("M - Modifizieren der Liste\n");
  Printf("W - Schreiben der 3D-Punkte in '3d.txt'\n");
  Printf("T - Test-Konfiguration anlegen\n");
  Printf("X - Exit\n");
}

int main(int argc, char* argv[])
{
  bool ready = FALSE;
  Vector xy1, xy2;
  Vector camera = startcamera();
  char input;

  if (argc > 1)
    project = argv[1];

  readproject(project);
  i1 = ReadImg(fn1);
  m1 = NewImg(i1->xsize, i1->ysize, 7);
  clearImg(m1);
  hi1 = NewImg(SSIZE, SSIZE, 255);
  hm1 = NewImg(SSIZE, SSIZE, 7);

  i2 = ReadImg(fn2);
  m2 = NewImg(i2->xsize, i2->ysize, 7);
  clearImg(m2);
  hi2 = NewImg(SSIZE, SSIZE, 255);
  hm2 = NewImg(SSIZE, SSIZE, 7);
  Display(ON);
  Show(OVERLAY, i1, m1);
  Show(OVERLAY, i2, m2);
  Alpha(ON);
  ClearAlpha();

  do
    {
      MarkRefs(xy12, m1, m2);
      MainMenu();
      input = GetChar();
      ClearAlpha();
      MainMenu();

      switch (input)
        {
        case '3':

          if (Ask("3D-Punkte neu initialisieren?", "yn"))
            {
              int i;

              for (i = 0; i < d3d.rows(); i++)
                {
                  d3d[i][0] = 0.0;
                  d3d[i][1] = 0.0;
                  d3d[i][2] = Z_START_WERT;
                }
            }

          break;
        case 'b':
        case 'B':
          Solve(xy12, d3d, camera);
          break;
        case 'a':
        case 'A':
          Zoom(i1, 0, 0, 0);
          xy1 = Vector(SelVector(DEFAULT, i1));
          Zoom(i2, 0, 0, 0);
          xy2 = Vector(SelVector(DEFAULT, i2));
          xy12 = xy12 && Vector(xy1[0], xy1[1], xy2[0], xy2[1]);
          d3d = d3d && Vector(0.0, 0.0, Z_START_WERT);
          break;
        case 't':
        case 'T':
          TestKonfig(camera);
          break;
        case 'w':
        case 'W':
          write3d(d3d);
          break;
        case 'm':
        case 'M':
          Modify();
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

  writeproject(project);
  return 0;
}

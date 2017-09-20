#include <stdio.h>
#include <iostream>
#include <image.h>

// vorgabe der Kontur ?
#define CONT1

// Testdarstellung, Ausgabe und Halt bei Timewarping
#define TWTEST

// Timing
#define TWTIME

// noise in test-kontur
#define CNOISE 3

#define ANOISE 0.04

#ifdef TWTIME

double startt[10];
void Begin(int n, const char *s)
{

  Printf("Begin %s\n", s);
  startt[n] = TimeD();
}
void End(int n, const char *s)
{
  Printf("End %s (%6.2f)\n", s, TimeD() - startt[n]);
}
#define BEGIN(n,x) Begin(n,x)
#define END(n,x) End(n,x)
#else
#define BEGIN(n,x) /*Begin(x)*/
#define END(n,x) /*End(x)*/
#endif

int disturbation_contur(Contur &cont, int noise,
                        int outlier, int outlier_intensity);

int TimeWarping(const Matrix &pl1, const Matrix &pl2,
                const Matrix &hu1, const Matrix &hu2,
                Matrix &opl1, Matrix &opl2);

void ReduceLists(Matrix &pl1, Matrix &pl2, int n);

//
int MyMod(int a, int b)
{
  while (a < 0) a += b;
  return a % b;
}

// ************************************************************

void CalcFeatures(const Contur &cont, const Matrix &pl, Matrix &hu_cont)
{
  int i;

  Moments moment(cont);

  for (i = 0; i < pl.rows(); i++)
    {
      Moments smoments(moment.Translate(-pl[i][0], -pl[i][1]));
      hu_cont[i] = smoments.AffineHuInvariants(false);
    }
//  cout << hu_cont << endl;
}

double CheapestWay(const Matrix &abstand, int maindir, IMatrix &ref)
{
  // find cheapest way
  // abstand.cols() muß größer als abstand.rows() sein
  Image dir;
  int ny = abstand.cols();
  int nx = abstand.rows();

  Vector min1(ny);
  Vector min2(ny);

  int x;
  int y, y1, y2;
  int dy, dy1, dy2;

  double vy0, vy1, vy2;
  dir = NewImg(nx, ny, 7);

  for (y = 0; y < ny; y++) // first column, all rows
    {
      min1[y] = abstand[0][y];
    }

  if (maindir > 0)
    {
      dy1 = -1;
      dy2 = -2;
    }
  else
    {
      dy1 = 1;
      dy2 = 2;
    }

  for (x = 1; x < nx; x++) // all columns
    {
      if (RandomD() > ((double)ny / (double)nx))
        {
          vy0 = 0;
          vy1 = ANOISE;
          vy2 = ANOISE;
        }
      else
        {
          vy0 = 1;
          vy1 = 0;
          vy2 = ANOISE;
        }
      for (y = 0; y < ny; y++) // all rows
        {
          y1 = MyMod(y + dy1, ny);
          y2 = MyMod(y + dy2, ny);
          min2[y] = min1[y1] + vy1;
          dy = dy1;
          if (min1[y] + vy0 < min2[y])
            {
              min2[y] = min1[y] + vy0;
              dy = 0;
            }
          if (min1[y2] + vy2 < min2[y])
            {
              min2[y] = min1[y2] + vy2;
              dy = dy2;
            }
          min2[y] += abstand[x][y];
          PutVal(dir, x, y, dy + 3);
        }

      x++;
      if (x < nx)
        {
          if (RandomD() > ((double)ny / (double)nx))
            {
              vy0 = 0;
              vy1 = ANOISE;
              vy2 = ANOISE;
            }
          else
            {
              vy0 = 1;
              vy1 = 0;
              vy2 = ANOISE;
            }
          for (y = 0; y < ny; y++) // all rows
            {
              y1 = MyMod(y + dy1, ny);
              y2 = MyMod(y + dy2, ny);
              min1[y] = min2[y1] + vy1;
              dy = dy1;
              if (min2[y] + vy0 < min1[y])
                {
                  min1[y] = min2[y] + vy0;
                  dy = 0;
                }
              if (min2[y2] + vy2 < min1[y])
                {
                  min1[y] = min2[y2] + vy2;
                  dy = dy2;
                }
              min1[y] += abstand[x][y];
              PutVal(dir, x, y, dy + 3);
            }
        }
    }
//  cout << endl;

  double minimum = min2[0];
  int miny = 0;
  for (y = 1; y < ny; y++)
    {
      if (min2[y] < minimum)
        {
          minimum = min2[y];
          miny = y;
        }
    }

  ref = IMatrix(nx, 2);

  for (x = nx - 1; x >= 0; x--)
    {
      ref[x][0] = x;
      ref[x][1] = miny;
      dy = GetVal(dir, x, miny); // "direction" to go
      miny = MyMod(miny + dy - 3, ny); // -> next point (x-1,miny)
    }

  return minimum;
}

void ReduceLists(Matrix &pl1, Matrix &pl2, int n)
{
  int i;
  while (pl1.rows() > n)
    {
      i = Random(pl1.rows() - 1);
      pl1.DeleteRow(i);
      pl2.DeleteRow(i);
    }
}


Trafo RandomTrafo(int xz, int yz, double ddet)
{
  Trafo trafo1, trafo2, ret;
  Matrix tr(3, 3);
  Randomize();
  double z11, z12, z21, z22;
  double det;

  do
    {
      z11 = (RandomD() - 0.5) * 2.5;
      z12 = (RandomD() - 0.5) * 2.5;
      z21 = (RandomD() - 0.5) * 2.5;
      z22 = (RandomD() - 0.5) * 2.5;
      det = z11 * z22 - z12 * z21;
    }
  while (!((fabs(det) > (1. - ddet) && fabs(det) < 1. + ddet)));

  tr[0][0] = z11;
  tr[0][1] = z12;
  tr[0][2] = 0.0;
  tr[1][0] = z21;
  tr[1][1] = z22;
  tr[1][2] = 0.0;
  tr[2][0] = 0.0;
  tr[2][1] = 0.0;
  tr[2][2] = 1.0;

  ret = Trafo();
  ret.Shift(-xz, -yz);
  ret = (Trafo(tr)) * ret;
  ret.Shift(xz, yz);
  return ret;
}
//-----------------------------------------
void usage(const string &p)
{
  cout << p << endl;
  cout << "Usage:" << endl;
  cout << "    " << p << " <options>" << endl;
  cout << "options:" << endl;
  cout << "-k       Circle contur" << endl;
  cout << "-s       Special predefined contur" << endl;
  cout << "-n <val> Noise level" << endl;
  cout << "-o <val> Number of outliers" << endl;
  cout << "-x <w>   Width of image" << endl;
  cout << "-y <h>   Height of image" << endl,
       cout << "-h       (this) help" << endl;
  exit(1);
}
//----------------------------------------
int main(int argc, char *argv[])
{
  int dimx = 480, dimy = 480;
  int N1, N2;
  int noise = CNOISE;
  int outlier = 0;
  int outlier_intensity = 5;

  enum gentype {circle, interactive, special};

  gentype contgen = interactive;

  int rc;

  while ((rc = getopt(argc, argv, "hksn:x:y:o:i:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          dimx = atol(optarg);
          break;
        case 'y':
          dimy = atol(optarg);
          break;
          // test contur generation
        case 'k':
          contgen = circle;
          break;
        case 's':
          contgen = special;
          break;
          // noise level
        case 'n':
          noise = atol(optarg);
          break;
        case 'o':
          outlier = atol(optarg);
          break;
        case 'i':
          outlier_intensity = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }
  Image pic1, pic2, mark1, mark2;
  Contur cont1, cont2;
  Contur cont_h;
  Matrix pl1, pl2, pl2a;
  Matrix opl1, opl2;
  Matrix tr(3, 3);
  Trafo trafo;

  // Initialisierung ICE-Textterminal
  OpenAlpha("ICE Text Server");
  SetAttribute(0, 7, 0, 0);
  ClearAlpha();
  // Ende Terminal

  pic1 = NewImg(dimx, dimy, 255);
  mark1 = NewImg(dimx, dimy, 255);
  pic2 = NewImg(dimx, dimy, 255);
  mark2 = NewImg(dimx, dimy, 255);

  ClearImg(pic1);
  ClearImg(mark1);

  ClearImg(pic2);
  ClearImg(mark2);

  Display(ON);

  Show(OVERLAY, pic1, mark1, "Input");
  Show(OVERLAY, pic2, mark2, "Output");

  switch (contgen)
    {
    case interactive:
      // ********************************
      cont1 = SelContur(mark1, TRUE);
      // **********************
      trafo = RandomTrafo(dimx / 2, dimy / 2, 0.1);
      cont2 = Transform(trafo, cont1);
      break;

    case special:
      cont1 = Contur(55, 55);
      cont1.Add(6, 366);
      cont1.Add(366, 366);
      cont1.Add(366, 66);
      cont1.Add(55, 55);

      cont2 = Contur(400 - 55, 55);
      cont2.Add(400 - 366, 66);
      cont2.Add(400 - 366, 366);
      cont2.Add(400 - 6, 366);
      cont2.Add(400 - 55, 55);
      break;

    case circle:
    {
      double circle[3];
      circle[0] = dimx / 2;
      circle[1] = dimy / 2;
      circle[2] = dimy / 4;
      cont1 = CircleContur(circle);
      disturbation_contur(cont1, 0, 10, 15);
      // **********************
      trafo = RandomTrafo(dimx / 2, dimy / 2, 0.1);
      cont2 = Transform(trafo, cont1);
    }
    break;
    } // switch(gettype)

  // ***************************************************
  // Störung der transformierten Kontur

  disturbation_contur(cont2, noise, outlier, outlier_intensity);
  MarkContur(cont1, 1, mark1);
  MarkContur(cont2, 2, mark1);

  N1 = cont1.Number();
  N2 = cont2.Number();

  pl1 = ConturPointlist(cont1);
  pl2 = ConturPointlist(cont2);
  GetChar();

  Matrix hu_cont1(pl1.rows(), 6);
  Matrix hu_cont2(pl2.rows(), 6);

  BEGIN(0, "Calculating features");
  CalcFeatures(cont1, pl1, hu_cont1);
  CalcFeatures(cont2, pl2, hu_cont2);
  END(0, "Calculating Features");

//  Show(OFF,pic1);
//  Show(OFF,pic2);

  // Jetzt time-warping
  BEGIN(1, "Time warping");
  if (pl1.rows() >= pl2.rows())
    TimeWarping(pl1, pl2, hu_cont1, hu_cont2, opl1, opl2);
  else
    TimeWarping(pl2, pl1, hu_cont2, hu_cont1, opl2, opl1);

  END(1, "Time warping");

//  Show(OVERLAY,pic1,mark1);
//  Show(OVERLAY,pic2,mark2);

  DrawPolygon(opl1, 1, mark2);
  //  DrawPolygon(opl2,3,mark2);

  for (int i = 0; i < opl1.rows(); i += 20)
    {
      Line(opl1[i][0], opl1[i][1], opl2[i][0], opl2[i][1], 255, DEFAULT, pic1);
    }

#if 0
  BEGIN(2, "Reducelists");
  ReduceLists(opl1, opl2, 300);
  END(2, "Reducelists");

  BEGIN(3, "MatchPointlistsLinOpt");
  trafo = MatchPointlistsLinOpt(opl2, opl1, TRM_AFFINE);
  END(3, "MatchPointlistsLinOpt");
#else
  BEGIN(3, "MatchPointlists");
  trafo = MatchPointlists(opl2, opl1, TRM_AFFINE);
  END(3, "MatchPointlists");
#endif

  TransformList(trafo, opl2, pl2a);
  DrawPolygon(pl2a, 2, mark2);

  GetChar();
  return 0;
}

//*****************************************************************
int disturbation_contur(Contur &cont, int noise, int outlier,
                        int outlier_intensity)
{
  int i;
  int x, y, z;
  int x_s, y_s;
  Contur cont_h;

  int N = cont.Number();

  cont.getPoint(0, x_s, y_s);
  cont_h.SetStart(x_s, y_s);

  int outfirst = Random(N);

  int xm = 0, ym = 0; // centroid
  for (i = 1; i < N; ++i)
    {
      int x, y;
      cont.getPoint(i, x, y);
      xm += x;
      ym += y;
    }

  xm /= N;
  ym /= N;

  for (i = 1; i < N; ++i)
    {
      cont.getPoint(i, x, y);

      // vector to "outside"
      double dx = x - xm;
      double dy = y - ym;
      if (dx == 0.0 && dy == 0.0) dx = 1.0;

      // normalized len
      double len = sqrt(dx * dx + dy * dy);
      dx = dx / len;
      dy = dy / len;

      // outlier ?
      int di = i - outfirst;
      if (di < 0) di += N;
      if (di < outlier)
        {
          x = RoundInt(x + dx * outlier_intensity);
          y = RoundInt(y + dy * outlier_intensity);
        }

      // noise
      z = Random(noise) - noise / 2;
      x = RoundInt(x + dx * z);
      y = RoundInt(y + dy * z);

      // add point to contur
      cont_h.Add(x, y);
    }
  cont_h.Add(x_s, y_s);
  cont = cont_h;
  return 0;
}

// ***************************************************************************
// ***************************************************************************
// ***************************************************************************

int TimeWarping(const Matrix &pl1, const Matrix &pl2,
                const Matrix &hu1, const Matrix &hu2,
                Matrix &opl1, Matrix &opl2
               )
{
  int i, j;
  int n1, n2;
  double max, min;
  double dist;

#ifdef TWTEST
  int g;
  Image pic, mark;
#endif

  // auf pl1 steht auf jeden Fall die längere Liste
  n1 = pl1.rows();
  n2 = pl2.rows();
  Matrix abstand(n1, n2);

#ifdef TWTEST
  Printf("Dimension der Matrix %d %d\n", n1, n2);
#endif
  // Nun in die Matrix abstand die "Abstände" zwischen den Hu-Invarianten
  // aller Punkte eintragen, später dann im Bild visualisieren
  BEGIN(5, "Calculate distances");
  max = 0.000001;
  min = 1e99;
  for (i = 0; i < n1; ++i)
    {
      for (j = 0; j < n2; ++j)
        {
          dist = (hu1[i] - hu2[j]).Length();
          abstand[i][j] = dist;
          if (dist > max) max = dist;
          if (dist < min) min = dist;
        }
    }
  END(5, "Calculate distances");

#ifdef TWTEST
  Printf("Größter Abstand %f\n", max);
  Printf("Kleinster Abstand %f\n", min);

  pic = NewImg(n1, n2, 255);
  mark = NewImg(n1, n2, 3);

  Show(OVERLAY, pic, mark, "Distance field and Assignment");
//  Zoom(pic,0,0,0);

  for (i = 0; i < n1; ++i)
    for (j = 0; j < n2; ++j)
      {
        g = RoundInt(abstand[i][j] / max * 255.);
        if (g > 255) PutVal(pic, i, j, 255);
        else PutVal(pic, i, j, g);
      }

#endif

  IMatrix ref1, ref2;
  double min1, min2;
  BEGIN(6, "Searching way(1)");
  min1 = CheapestWay(abstand, 1, ref1);
  END(6, "Searching way(1)");
  BEGIN(6, "Searching way(2)");
  min2 = CheapestWay(abstand, -1, ref2);
  END(6, "Searching way(2)");

  // Für testphase: Wege darstellen
#ifdef TWTEST
  for (i = 0; i < n1; i++)
    PutVal(mark, ref1[i][0], ref1[i][1], 1);

  for (i = 0; i < n1; i++)
    PutVal(mark, ref2[i][0], ref2[i][1], 2);

  if (min1 < min2)
    {
      Printf("min1: %g\n", min1);
      Printf("min1/n: %g\n", min1 / n1);
      Printf("min1/n/max: %g\n", min1 / n1 / max);
    }
  else
    {
      Printf("min2: %g\n", min2);
      Printf("min2/n: %g\n", min2 / n1);
      Printf("min2/n/max: %g\n", min2 / n1 / max);
    }

#endif

  opl1 = Matrix(n1, 2);
  opl2 = Matrix(n1, 2);

  if (min1 < min2)
    {
      for (i = 0; i < n1; i++)
        {
          opl1[i] = pl1[ref1[i][0]];
          opl2[i] = pl2[ref1[i][1]];
        }
    }
  else
    {
      for (i = 0; i < n1; i++)
        {
          opl1[i] = pl1[ref2[i][0]];
          opl2[i] = pl2[ref2[i][1]];
        }
    }
#ifdef TWTEST
  Printf("End time warping\n");
  GetChar();
  FreeImg(pic);
  FreeImg(mark);
#endif
  return 0;
}

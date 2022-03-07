/*
 *  f3d.cpp
 *  erzeugt dreidimensionale Bilder aus Fraktalen
 */

#include <string>
#include <iomanip>
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include <image.h>

using namespace std;
using namespace ice;

int super = 1;

#define SFAK 1.002

int xs = 512, ys = 512, zs = 512, maxval = 255;

typedef double DOUBLE;

// eine kleine Klasse Komplex
class Komplex
{
  DOUBLE re;
  DOUBLE im;
public:
  Komplex(DOUBLE r, DOUBLE i): re(r), im(i) {}
  Komplex(DOUBLE r): re(r), im(0.0) {}
  Komplex(): re(0.0), im(0.0) {}

  Komplex operator*(double r) const
  {
    return Komplex(re * r, im * r);
  }

  static Komplex cos(const Komplex& c)
  {
    return Komplex(std::cos(c.re) * std::cosh(c.im), -std::sin(c.re) * std::sinh(c.im));
  }

  double abs2() const
  {
    return re * re + im * im;
  }

  friend Komplex operator+(const Komplex& a, const Komplex& b);
  friend Komplex operator-(const Komplex& a, const Komplex& b);
  friend Komplex operator*(const Komplex& a, const Komplex& b);
};

Komplex operator+(const Komplex& a, const Komplex& b)
{
  return Komplex(a.re + b.re, a.im + b.im);
}

Komplex operator-(const Komplex& a, const Komplex& b)
{
  return Komplex(a.re - b.re, a.im - b.im);
}

Komplex operator*(const Komplex& a, const Komplex& b)
{
  return Komplex(a.re * b.re - a.im * b.im, a.im * b.re + a.re * b.im);
}

// Funktionen für Iteration

typedef int iterfunc(DOUBLE x0, DOUBLE y0, DOUBLE a, DOUBLE b, int mc);

iterfunc iterapfel;
iterfunc iterapfel4;
iterfunc iterapfelcc;
iterfunc itership;
iterfunc iterexp;
iterfunc iter3_2;
iterfunc itercollatz;

const iterfunc* fractals[] = {&iterapfel, &iterapfel4, &iterapfelcc, &itership,
                              &iterexp, &iter3_2, &itercollatz, NULL
                             };

int display = true;

string findFilename(const string& mask);
void SaveToImage(const string& bfn, const Image3d<Image>& ptr);

void getRange(const string& opt, DOUBLE& v1, DOUBLE& v2)
{
  unsigned int pos_comma = opt.find(",");
  if (pos_comma == string::npos)
    {
      cerr << "Keine Bereichsangabe: " << opt << endl;
      exit(1);
    }
  v1 = atof(opt.substr(0, pos_comma).c_str());
  v2 = atof(opt.substr(pos_comma + 1).c_str());
  cout << v1 << " .. " << v2 << endl;
}

class DRange
{
private:
  DOUBLE v1, v2;
  int nSteps;
  int step;
public:
  DRange(DOUBLE val1, DOUBLE val2, int nstep): v1(val1), v2(val2), nSteps(nstep), step(0) {}
  void setStep(int astep, int nstep)
  {
    step = astep;
    nSteps = nstep;
  };
  DOUBLE getValue()
  {
    int istep = nSteps - step;
    return (step * v2 + istep * v1) / nSteps;
  }
};

//------------------- Main -----------------------------------------
int Main(int argc, char* argv[])
{
  Image img;// Bild für Darstellung
  Image3d<Image> img3;// Bild für Werte (3d)

  const iterfunc* currentFractal = &iterapfel;
  int maxcount = 64;

  DOUBLE x1 = -1, x2 = 1;
  DOUBLE y1 = -1, y2 = 1;
  DOUBLE a1 = -2, a2 = 1;
  DOUBLE b1 = -1.5, b2 = 1.5;

  string vars = "ab";

  string basefilename = "f3d";

  /* Auswertung der Kommandozeile */

  int rc;
  while ((rc = getopt(argc, argv, "X:Y:Z:M:x:y:a:b:dV:o:")) >= 0)
    {
      switch (rc)
        {
        case 'd':
          display = false;
          break;
        case 'X':
          xs = atoi(optarg);
          break;
        case 'Y':
          ys = atoi(optarg);
          break;
        case 'Z':
          zs = atoi(optarg);
          break;
        case 'M':
          maxcount = atoi(optarg);
          break;

        case 'x':
          getRange(optarg, x1, x2);
          break;
        case 'y':
          getRange(optarg, y1, y2);
          break;
        case 'a':
          getRange(optarg, a1, a2);
          break;
        case 'b':
          getRange(optarg, b1, b2);
          break;

        case 'V':
          vars = optarg;
          break;
        case 'o':
          basefilename = optarg;
          break;
#if 0
        default:
          ParameterMessage(); /* falscher Parameter */
#endif
        }
    }

  DRange xx(x1, x2, maxcount);
  DRange yy(y1, y2, maxcount);
  DRange aa(a1, a2, maxcount);
  DRange bb(b1, b2, maxcount);

  vars += "ab"; // make sure, that vars has at least 2 chars

  DRange* X;
  DRange* Y;
  vector<DRange*> var;

  switch (vars[0])
    {
    case 'x':
      X = &xx;
      break;
    case 'y':
      X = &yy;
      break;
    case 'a':
      X = &aa;
      break;
    case 'b':
      X = &bb;
      break;
    }

  switch (vars[1])
    {
    case 'x':
      Y = &xx;
      break;
    case 'y':
      Y = &yy;
      break;
    case 'a':
      Y = &aa;
      break;
    case 'b':
      Y = &bb;
      break;
    }

  var.push_back(&xx);
  var.push_back(&yy);
  var.push_back(&aa);
  var.push_back(&bb);

  // Bild für Darstellung
  img = NewImg(xs, ys, maxval, "Fraktal");
  clearImg(img);
  Show(ON, img);

  img3.create(xs, ys, zs);

  for (int z = 0; z < zs; z++)
    {
      for (unsigned int i = 0; i < var.size(); i++)
        var[i]->setStep(z, zs);
      for (int y = 0; y < ys; y++)
        {
          Y->setStep(y, ys);
          for (int x = 0; x < xs; x++)
            {
              X->setStep(x, xs);
              DOUBLE fa = aa.getValue();
              DOUBLE fb = bb.getValue();
              DOUBLE fx = xx.getValue();
              DOUBLE fy = yy.getValue();
              //  cout << fa << " " << fb << " " << fx << " " << fy << endl;
              int iter = (*currentFractal)(fx, fy, fa, fb, maxcount);
              img3.setPixel(x, y, z, iter * maxval / maxcount);
              img.setPixel(x, y, iter * maxval / maxcount);
            }
        }
      // GetChar();
    }

  SaveToImage(basefilename, img3);

  return OK;
}

int iterapfel(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE fx = x0, fy = y0;
  DOUBLE x2, y2;

  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2 + bp;
      fx = x2 - y2 + ap;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int iter3_2(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fx = x0; // startwert
  DOUBLE fy = y0;
  DOUBLE r = 0;
  while ((z < maxcount) && (r < 16))
    {
      r = sqrt(fx * fx + fy * fy);
      DOUBLE fi = atan2(fy, fx);
      if (fi < 0) fi = fi + M_PI;

      r = r * sqrt(r);
      fi = fi * 1.5;

      fy = r * cos(fi) + ap;
      fx = r * sin(fi) + bp;
      z++;
    }
  return z;
}

int iterexp(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE fx = x0, fy = y0;
  while ((z < maxcount) && (fd < 16))
    {
      DOUBLE ex = exp(fx);
      fx = ex * cos(fy) - fx - ap;
      fy = ex * sin(fy) - fy - bp;
      fd = fx * fx + fy * fy;
      z++;
    }
  return z;
}

int iterapfelcc(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE fx = x0, fy = y0;
  DOUBLE x2, y2;
  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = - fx * fy * 2 + bp;
      fx = x2 - y2 + ap;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int itership(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE fx = x0, fy = y0;
  DOUBLE x2, y2;
  while ((z < maxcount) && (fd < 16))
    {
      if (fx < 0) fx = -fx;
      if (fy < 0) fy = -fy;
      x2 = fx * fx;
      y2 = fy * fy;
      fy = - fx * fy * 2 + bp;
      fx = x2 - y2 + ap;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int iterapfel4(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE fx = x0, fy = y0;
  DOUBLE x2, y2;
  while ((z < maxcount) && (fd < 40))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2;
      fx = x2 - y2;
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2 + bp;
      fx = x2 - y2 + ap;
      fd = x2 + y2;
      z++;
    }
  return z;
}


int itercollatz(DOUBLE x0, DOUBLE y0, DOUBLE ap, DOUBLE bp, int maxcount)
{
  int ct = 0;
  Komplex z(x0, y0);
  Komplex c(ap, bp);
  DOUBLE fd = 0;
  while ((ct < maxcount) && (fd < 440))
    {
      // z = (1.0 + z * 4 - (1 + z * 2) * Komplex::cos(z * M_PI)) * 0.25;
      z = (2.0 + z * 7 - (2.0 + z * 5) * Komplex::cos(z * M_PI)) * 0.25 + c;
      fd = z.abs2();
      ct++;
    }
  return ct;
}

void SaveToImage(const string& fn, const Image3d<Image>& img3)
{
  if (fn.empty())
    {
      cerr << "Bildname leer" << endl;
      exit(1);
    }
  else
    {
      for (int z = 0; z < img3.zsize; z++)
        {
          string fullfilename = fn + NumberString(z, -4) + ".jpg";
          img3[z].write(fullfilename);
        }
    }
}

string toString(int e)
{
  bool sign = e < 0;
  if (sign) e = -e;
  string res;
  do
    {
      int z = e % 10;
      e = e / 10;
      res = char(z + '0') + res;
    }
  while (e > 0);
  if (sign) res = '-' + res;
  return res;
}

#define BASE 10
string toString(DOUBLE v)
{
  //    cout << v << " " << sizeof(v) << " " << sizeof(long long int) << endl;
  int e = 0;
  string res = "";
  if (v == 0.0) return "0.0";
  if (v < 0.0)
    {
      v = -v;
      res = "-";
    }
  res += "0.";
  while (v < 1.0)
    {
      v = v * BASE;
      e--;
    }
  while (v >= 1.0)
    {
      v = v / BASE;
      e++;
    }
  while (v != 0.0)
    {
      v = BASE * v;
      int z = int(v);
      res += char(z + '0');
      v -= z;
    }
  //    cout << v << " done" << endl;
  return res + "e" + toString(e);
}

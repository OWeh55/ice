/*
 *  vis2.cpp
 *  testet bildspeicherverwaltung, visualisierung
 *  testet Alphanumerischen Bildschirm
 *  macht Spass
 */

#include <string>
#include <cmath>
#include <fstream>
#include <iomanip>

#include <image.h>

using namespace std;
using namespace ice;

int super = 1;

#define HELLDUNKELMODIFIER(i) ((i) * 90 / 100)

#define SFAK 1.002

int sx = 720 * 4 / 3, sy = 720, maxval;

//typedef long double DOUBLE;
//typedef float DOUBLE;
typedef double DOUBLE;

#define MAXCOUNT_DEFAULT (1024*16)
#define MAXCOUNT_BENCH (1024*128)
#define MAXCOUNT_MAX (256*256*16)

#define CT_GREY 0
#define CT_BLUE_YELLOW 1
#define CT_BW 2
#define CT_RAINBOW 3
#define CT_RED_YELLOW 4

#define CT_ALT 16
#define CT_NORM 32

// Alpha
int xse = 0, yse = 0; /* Bildschirmgroesse (Alpha) */
//--------------------------------------------------------

/*
 * DRange beschreibt einen Ausschnitt der euklidischen Ebene.
 * es ist möglich ihn zu skalieren, neu zu setzen und
 * in einer gegebenen Schrittweite auf einen Zielbereich hin
 * anzunaehern
 */

class DRange
{
private:
  static DOUBLE interpol(DOUBLE x1, DOUBLE x2, DOUBLE rel)
  {
    return (1.0 - rel) * x1 + rel * x2;
  }

protected:
  DOUBLE x0, dx;
  DOUBLE y0, dy;

public:
  DRange(): x0(0.0), dx(1.0), y0(0.0), dy(1.0) {}
  DRange(DOUBLE x0p, DOUBLE dxp, DOUBLE y0p, DOUBLE dyp):
    x0(x0p), dx(dxp), y0(y0p), dy(dyp)
  {}

// verändert den Bereich in Richtung Zielbereich dest
// um einen Schritt mit einer Groessenaenderung um sfac
  virtual bool stepTo(const DRange& dest, DOUBLE scaleFactor)
  {
    bool finish = false;
    DOUBLE area1 = dx * dy;
    DOUBLE area2 = dest.dx * dest.dy;
    DOUBLE areaFactor = sqrt(area2 / area1);
    // cout << "factor: " << areaFactor << endl;
    DOUBLE trel = (scaleFactor - 1) / (areaFactor - 1);
    if (trel >= 1.0)
      {
        finish = true;
        trel = 1.0;
      }
    // cout << trel << endl;
    x0 = interpol(x0, dest.x0, trel);
    y0 = interpol(y0, dest.y0, trel);
    dx = interpol(dx, dest.dx, trel);
    dy = interpol(dy, dest.dy, trel);
    return !finish;
  }

  virtual void Scale(DOUBLE fac)
  {
    dx *= fac;
    dy *= fac;
  }

  virtual bool operator==(const DRange& other) const
  {
    return x0 == other.x0 &&
           dx == other.dx &&
           y0 == other.y0 &&
           dy == other.dy;
  }
};

/*
 * FRange beschreibt einen Ausschnitt der euklidischen Ebene und
 * seine Transformation in die diskreten Koordinaten eines Bildes.
 */

class FRange: public DRange
{
private:
  int xs, ys;

  DOUBLE a0, fa;
  DOUBLE b0, fb;

  virtual void init()
  {
    a0 = x0 - dx;
    b0 = y0 - dy;
    fa = 2 * dx / xs;
    fb = 2 * dy / ys;
  }

public:
  FRange(int xs, int ys): DRange(), xs(xs), ys(ys)
  {
    squarePixel();
  }

  FRange(const DRange& dr, int xs, int ys): DRange(dr),
    xs(xs), ys(ys)
  {
    squarePixel();
  }

  virtual bool operator==(const FRange& other) const
  {
    return (DRange) * this == (DRange)other &&
           y0 == other.y0 &&
           dy == other.dy;
  }

  virtual void Scale(DOUBLE fac)
  {
    DRange::Scale(fac);
    init();
  }

  virtual void ScreenToAB(DOUBLE x, DOUBLE y, DOUBLE& a, DOUBLE& b) const
  {
    a = a0 + x * fa;
    b = b0 + y * fb;
  }

  virtual void zoomIn(int x1, int y1, int x2, int y2)
  {
    // parameter neu links oben
    DOUBLE a1, b1;
    ScreenToAB(x1, y1, a1, b1);
    // parameter neu rechts unten
    DOUBLE a2, b2;
    ScreenToAB(x2, y2, a2, b2);
    // setzen auf ganzes bild
    x0 = (a1 + a2) / 2;
    y0 = (b1 + b2) / 2;
    dx = (a2 - a1) / 2;
    dy = (b2 - b1) / 2;
    init();
  }

  virtual void squarePixel()
  {
    DOUBLE fx = dx / xs;
    DOUBLE fy = dy / ys;
    if (fx > fy)
      dy = dy * fx / fy;
    else
      dx = dx * fy / fx;
    init();
  }

  // verändert den Bereich in Richtung Zielbereich dest
  // um einen Schritt mit einer Groessenaenderung um sfac
  virtual bool stepTo(const FRange& dest, DOUBLE sfac)
  {
    bool continue_ = DRange::stepTo(dest, sfac);
    init();
    return continue_;
  }

  virtual DOUBLE a1() const
  {
    return a0;
  }
  virtual DOUBLE b1() const
  {
    return b0;
  }
  virtual DOUBLE a2() const
  {
    return a0 + (xs - 1) * fa;
  }
  virtual DOUBLE b2() const
  {
    return b0 + (ys - 1) * fb;
  }
};

// eine kleine Klasse Komplex
class Komplex
{
  DOUBLE re;
  DOUBLE im;
public:
  Komplex(DOUBLE r, DOUBLE i): re(r), im(i) {}
  Komplex(DOUBLE r): re(r), im(0.0) {}
  Komplex(): re(0.0), im(0.0) {}

  Komplex operator*(DOUBLE r) const
  {
    return Komplex(re * r, im * r);
  }

  static Komplex cos(const Komplex& c)
  {
    return Komplex(std::cos(c.re) * std::cosh(c.im),
                   -std::sin(c.re) * std::sinh(c.im));
  }

  DOUBLE abs2() const
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

typedef int iterfunc(DOUBLE a, DOUBLE b, int mc);

iterfunc iterapfel;
iterfunc iterapfel4;
iterfunc iterapfelcc;
iterfunc itership;
iterfunc iterexp;
iterfunc iterapfelfalsch;
iterfunc iter3_2;
iterfunc itercollatz;
iterfunc iterC2;
iterfunc iterCS;

class Fractal
{
public:
  iterfunc* iter;
  DRange initialRange;
  string name;
  Fractal(iterfunc* iterf, DRange range, const string& name):
    iter(iterf), initialRange(range), name(name) {}
};

Fractal Apfel(iterapfel, DRange(-0.7, 1.5, 0.0, 1.5), "Apfel");
Fractal Apfel4(iterapfel4, DRange(-0.2, 1.2, 0.0, 1.3), "Apfel4");
Fractal ApfelCC(iterapfelcc, DRange(-0.2, 1.8, 0.0, 1.8), "tricon");
Fractal BurningShip(itership, DRange(-0.3, 2.0, 0.4, 2.0), "Burning Ship");
Fractal ExpZ(iterexp, DRange(1.0, 4.1, 0.0, 4.1), "Exp(z)");
Fractal Apfel3_2(iter3_2, DRange(0.0, 3.0, 0.0, 3.0), "Apfel^3/2");
Fractal Collatz(itercollatz, DRange(0.0, 11.0, 0.0, 6.0), "Collatz");
Fractal ApfelX(iterapfelfalsch, DRange(-0.2, 3.0, 0.0, 3.0), "Falscher Apfel");
Fractal C2(iterC2, DRange(-0.2, 3.0, 0.0, 3.0), "CC");
Fractal CS(iterCS, DRange(-0.2, 3.0, 0.0, 3.0), "CosSin");

const Fractal* fractals[] = {&Apfel, &Apfel4, &ApfelCC, &BurningShip,
                             &ExpZ, &Apfel3_2, &Collatz, &ApfelX, &C2, &CS, nullptr
                            };

int iters(const Fractal& fractal, const FRange& r,
          int maxcount,
          DOUBLE x, DOUBLE y)
{
  // iteration für supersampling
  DOUBLE a, b;
  r.ScreenToAB(x, y, a, b);
  return (*fractal.iter)(a, b, maxcount - 1);
}

// benchmark options
bool bench = false;
bool display = true;
bool cyclic = false;
bool worldTime = true;

//int ColorMode;

//string toString(DOUBLE v);

void frame_(int, int, int, int);
void itersel(const Fractal*& fractals);
void selectDepth(int& maxdepth);
void SelectColor(int& ColorMode);
void setcolor(int ColorMode);
string findFilename(const string& mask);
void SaveToImage(const string& bfn, const Image& ptr);
void SaveToVideo(VideoFile& vout, const Image& ptr);
void Save3d(const string& fn, const Image& height, const Image& ptr);
void PrintA(string);
void ParameterMessage(void);
int gc();

void Printpp(int x, int& y, const string& text)
{
  SetAlphaCursor(x, y++);
  PrintA(text);
}

void PrintHelp()
{
  int y1 = 2;
  int ya = y1 + 1;
  int x1 = 40;

  SetAttribute(-1, -1, -1, 0);

  Printpp(x1, ya, "~Typ des Fraktals");
  Printpp(x1, ya, "~Iterationstiefe");
  Printpp(x1, ya, "~Farbtabellen");
  Printpp(x1, ya, "~Speichern");
  Printpp(x1, ya, "~Grösserer Ausschnitt");
  Printpp(x1, ya, "~Angleichen Ausdehnungen x/y");
  Printpp(x1, ya, "~Rücksetzen der Geometrie");
  Printpp(x1, ya, "~Kino - Sequenzberechnung");
//  Printpp(x1, ya, "S~upersampling ein/aus");
  Printpp(x1, ya, "~Quittieren");

  frame_(x1 - 2, y1, xse - 3, ya);

  SetAttribute(-1, -1, -1, 1);
  Printpp(x1, y1, "Tasten-Funktionen:");
}

void PrintStatus(const string& msg = "")
{
  if (!msg.empty())
    SetAttribute(-1, -1, -1, 1);
  else
    SetAttribute(-1, -1, -1, 0);
  SetAlphaCursor(40, yse - 3);
  string mmsg = msg;
  while (mmsg.length() < 35)
    mmsg = " " + mmsg;
  Print(mmsg);
}

void PrintParameter(const FRange& r, int maxcount)
{
  int x1 = 5;
  int y1 = 3;
  int ya = y1;

  SetAttribute(-1, -1, -1, 0);
  SetAlphaCursor(x1, ya++);
  Printf("X: %f ... %f", (double)r.a1(), (double)r.a2());
  SetAlphaCursor(x1, ya++);
  Printf("X0: %+8.3e  DX: %g  ", (double)(r.a2() + r.a1()) / 2, (double)(r.a2() - r.a1()) / 2);
  ya++;
  SetAlphaCursor(x1, ya++);
  Printf("Y: %f ... %f", (double)r.b1(), (double)r.b2());
  SetAlphaCursor(x1, ya++);
  Printf("Y0: %+8.3e  DY: %g  ", (double)(r.b2() + r.b1()) / 2, (double)(r.b2() - r.b1()) / 2);
  ya++;
  SetAlphaCursor(x1, ya++);
  Printf("Max: %6d", maxcount);
  SetAlphaCursor(x1, ya++);
  Printf("Supersampling: %d-fach", super);
}
//------------------- Main -----------------------------------------
int Main(int argc, char* argv[])
{
  //  cout << sizeof(DOUBLE) << endl;
  Image dimg;// Bild für _d_isplay
  Image vimg;// Bild für _v_alue
  //  int x, y, z;
  int x1, y1, xe, ye;

  int valc = 255; // zahl verfügbarer Farben
  int klicks;
  int c = 0;
  string basefilename = "frac%05d";
  string infilename = "";

  bool no_input;
  bool sequence = false;
  bool video = false;

  DOUBLE talt, tneu;
  DOUBLE tp;

  VideoFile vout;

  int ColorMode = CT_BLUE_YELLOW + CT_ALT;
  const Fractal* currentFractal = &Apfel;
  int maxcount = MAXCOUNT_DEFAULT;

  /* Auswertung der Kommandozeile */

  int rc;
  while ((rc = getopt(argc, argv, "bBcC:x:y:o:O:i:s:p")) >= 0)
    {
      switch (rc)
        {
        case 'b':
          bench = true;
          display = false;
          maxcount = MAXCOUNT_BENCH;
          break;
        case 'B':
          bench = true;
          maxcount = MAXCOUNT_BENCH;
          break;
        case 'c':
          bench = true;
          display = false;
          cyclic = true;
          maxcount = MAXCOUNT_BENCH;
          break;
        case 'p':
          worldTime = false;
          break;
        case 'x':
          sx = atol(optarg);
          break;
        case 'y':
          sy = atol(optarg);
          break;
        case 's':
          super = atol(optarg);
          break;

        case 'i':
          infilename = optarg;
          break;// Parameterübername aus Bildfile

        case 'O':
          basefilename = optarg;
          video = true;
          break;

        case 'o':
          basefilename = optarg;
          if (basefilename.find('%') == string::npos)
            basefilename = basefilename + "%05d";
          video = false;
          break;

        default:
          ParameterMessage(); /* falscher Parameter */
        }
    }

  /*
    cout << InfVis(MAXX) << " x " << InfVis(MAXY) << endl;
    sx=InfVis(MAXX);
    sy=InfVis(MAXY);
  */

  if (display)
    {
      OpenGraphic("Fraktal-Wunder");
      OpenAlpha("Fraktal-Wunder");
      valc = InfVis(TABLE_LEN);
    }

  maxval = valc - 1; // Defaultwert entsprechend Darstellungsfarben

  // Bild für Darstellung (mit beschränktem Wertumfang)
  dimg = NewImg(sx, sy, maxval, "Fraktal");
  dimg.set(0);

  // Bild für Speicherung der Werte im Original
  vimg = NewImg(sx, sy, MAXCOUNT_MAX, "Fraktalwerte");

  if (display)
    {
      Show(ON, dimg);
      Display(ON);

      Alpha(ON);

      xse = AlphaSizeX() - 1;
      yse = AlphaSizeY() - 1;
      SetAttribute(C_YELLOW, C_RED, 0, 0);
      ClearAlpha();
      // äusserer Zierrahmen
      frame_(1, 1, xse - 1, yse - 1);

      PrintHelp();
    }

  setcolor(ColorMode);

  FRange baseRange(currentFractal->initialRange, sx, sy);
  FRange currentRange = baseRange;

#if 0
  if (!infilename.empty())
    {
      string cmd = "exiftool -comment " + infilename;
      FILE* ip = popen(cmd.c_str(), "r");
      char ibuffer[3000];
      char* h = fgets(ibuffer, 3000, ip);
      if (h != NULL)
        {
          if (sscanf(h, "Comment : %lg %lg %lg", &a0, &b0, &da) == 3)
            {
              a0 = a0 - da / 2;
              db = da * sy / sx;
              b0 = b0 - db / 2;
            }
        }
      pclose(ip);
    }
#endif


  bool done = false;
  bool neue_rechnung = true;
  bool neue_darstellung = true;

  while (!done)
    {
      if (neue_rechnung)
        {
          if (display)
            {
              PrintParameter(currentRange, maxcount);
              PrintStatus("Rechne...");
            }

          if (worldTime)
            talt = TimeD(TM_WORLD);
          else
            talt = TimeD(TM_PROCESS);

          // Berechnung und Darstellung
#ifdef OPENMP
          #pragma omp parallel for schedule(dynamic,30)
#endif
          for (int yi = 0; yi < sy; yi++)
            {
              for (int xi = 0; xi < sx; xi++)
                {
                  int z = 0;
                  if (super != 1)
                    {
                      for (int i = 0; i < super; i++)
                        for (int j = 0; j < super; j++)
                          z += iters(*currentFractal, currentRange, maxcount,
                                     xi + (double)i / super, yi + (double)j / super);
                      z /= (super * super);
                    }
                  else
                    z = iters(*currentFractal, currentRange, maxcount,
                              xi, yi);

                  vimg.setPixelUnchecked(xi, yi, z);

                  if ((ColorMode & CT_NORM) == 0)
                    PutValUnchecked(dimg, xi, yi, z % valc);              // modulo
                  else
                    PutValUnchecked(dimg, xi, yi, z * maxval / maxcount); // normiert
                }
            }

          if (worldTime)
            tneu = TimeD(TM_WORLD);
          else
            tneu = TimeD(TM_PROCESS);

          if (display)
            {
              PrintStatus();
            }

          tp = (tneu - talt) * 1000000.0 / sx / sy;

          if (display)
            {
              SetAlphaCursor(3, yse - 4);
              Printf("%g us/Pixel\n", tp);
            }

          if (bench)
            printf("%3.2f (%3.2f)", (double)(50.0 / tp), (double)tp);

          if ((currentRange == baseRange)
              && (maxcount == MAXCOUNT_DEFAULT))
            {
              if (display)
                {
                  SetAlphaCursor(3, yse - 2);
                  Printf("Leistungsindex: %3.2f", (double)(50.0 / tp));
                }
            }
          neue_rechnung = false;
          neue_darstellung = false;
        } // if neue_rechnung

      if (neue_darstellung)
        {
          // neue Darstellung (bei veränderten Parametern)
          for (int yi = 0; yi < sy; yi++)
            {
              for (int xi = 0; xi < sx; xi++)
                {
                  int z = GetValUnchecked(vimg, xi, yi);

                  if ((ColorMode & CT_NORM) == 0)
                    PutValUnchecked(dimg, xi, yi, z % valc);              // modulo
                  else
                    PutValUnchecked(dimg, xi, yi, z * maxval / maxcount); // normiert
                }
            }

          neue_darstellung = false;
        }

      klicks = 1;

      x1 = sx;
      y1 = sy;
      xe = 0;
      ye = 0;

      no_input = true;
      if (bench)
        {
          no_input = false;
          neue_rechnung = true;
          if (!cyclic) done = true;
          else printf("\n");
        }

      while (no_input)
        {
          klicks = 0;
          int x, y, z;
          while ((((z = Mouse(dimg, &x, &y)) & M_RIGHT_DOWN) == 0) && (no_input))
            {
              Cursor(SET, dimg, x, y);
              if (z & M_LEFT_DOWN)
                {
                  if (x < x1)
                    x1 = x;
                  if (y < y1)
                    y1 = y;
                  if (x > xe)
                    xe = x;
                  if (y > ye)
                    ye = y;
                  klicks++;
                }

              c = gc();

              if (sequence)
                c = (c != 'Q' && c != 'q') ? 'Z' : 'z' ;

              if (c != 0)
                {

                  switch (c)
                    {
                    case '+':
                      super++;
                      no_input = false;
                      neue_darstellung = true;
                      neue_rechnung = true;
                      break;

                    case '-':
                      super--;
                      if (super < 1) super = 1;
                      no_input = false;
                      neue_rechnung = true;
                      neue_darstellung = true;
                      break;

                    case 'T':
                    case 't':   /* select new fractal */
                      itersel(currentFractal);
                      baseRange = currentRange = FRange(currentFractal->initialRange, sx, sy);
                      no_input = false;
                      neue_rechnung = true;
                      break;

                    case 'R':
                    case 'r':
                      currentRange = baseRange;

                      no_input = false;
                      klicks = 0;
                      neue_rechnung = true;
                      break;

                    case 'f':
                    case 'F':
                      SelectColor(ColorMode);
                      no_input = false;
                      neue_darstellung = true;
                      break;

                    case 'I':
                    case 'i':
                      selectDepth(maxcount);
                      no_input = false;
                      neue_rechnung = true;
                      break;

                    case 'q':
                    case 'Q':
                      no_input = false;
                      done = true;
                      break;

                    case 'o':
                      Alpha(OFF);
                      break;

                    case 'O':
                      Alpha(ON);
                      break;

                    case 'G':
                    case 'g':
                      // greater
                      currentRange.Scale(2);
                      no_input = false;
                      klicks = 0;
                      neue_rechnung = true;
                      break;

                    case 'K':
                    case 'k':
                    {
                      // "kino" - start sequence
                      bool ok = true;

                      if (video)
                        {
                          string vfn = findFilename(basefilename);
                          if (vfn.empty())
                            {
                              ok = false;
                              PrintStatus("Kann " + basefilename + " nicht anlegen");
                            }
                          else
                            {
                              vout.open(vfn, ios_base::out);
                              vout.setPara(dimg.xsize, dimg.ysize, 255, 25, 10000000);
                            }

                        }
                      if (ok)
                        {
                          sequence = true;
                          no_input = false;
                          klicks = 0;
                          neue_rechnung = true;
                        }
                      break;
                    }

                    case 'z':   /* laufende Sequence abbrechen ?*/
                    case 'Z':   /* laufende Sequence */
                    {
                      bool abortSequence = false;
                      if (c == 'z')
                        abortSequence = Ask("Abort Sequenz?");
                      if (video)
                        SaveToVideo(vout, dimg);
                      else
                        SaveToImage(basefilename, dimg);

                      no_input = false;
                      if (abortSequence || !currentRange.stepTo(baseRange, SFAK))
                        {
                          sequence = false;
                          no_input = true;
                          if (video)
                            vout.close();
                        }
                    }
                    klicks = 0;
                    neue_rechnung = true;
                    break;

                    case 'A':
                    case 'a':
                      currentRange.squarePixel();

                      no_input = false;
                      klicks = 0;
                      neue_rechnung = true;
                      break;

                    case 'S':
                    case 's':
                      if (!video)
                        SaveToImage(basefilename, dimg);
                      else
                        SaveToVideo(vout, dimg);

                      break;

                    case '3':
                      Save3d(basefilename, vimg, dimg);
                      break;

                    }
                }
            }

          if (klicks >= 2)
            {
              currentRange.zoomIn(x1, y1, xe, ye);
              no_input = false;
              neue_rechnung = true;
            }
        }
    }
  CloseAlpha();
  if (bench)
    printf("\n");
  return OK;
}

int iterapfel(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE x2, y2;
  DOUBLE a, b;
  a = ap;
  b = bp;
  fx = a; // startwert = Wert nach 1 Iteration
  fy = b;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2 + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int iterapfelfalsch(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE x2, y2;
  DOUBLE a, b;
  a = ap;
  b = bp;
  fx = a; // startwert = Wert nach 1 Iteration
  fy = b;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fx = x2 - y2 + a;
      fy = fx * fy * 2 + b;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int iter3_2(DOUBLE a, DOUBLE b, int maxcount)
{
  int z = 0;
  DOUBLE fx = a; // startwert = Wert nach 1 Iteration
  DOUBLE fy = b;
  DOUBLE r = 0;
  while ((z < maxcount) && (r < 16))
    {
      r = sqrt(fx * fx + fy * fy);
      DOUBLE fi = atan2(fy, fx);
      if (fi < 0) fi = fi + M_PI;

      r = r * sqrt(r);
      fi = fi * 1.5;

      fy = r * cos(fi) + a;
      fx = r * sin(fi) + b;
      z++;
    }
  return z;
}

int iterexp(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE a, b;
  a = ap;
  b = bp;
  fx = 0;
  fy = 0;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 16))
    {
      DOUBLE ex = exp(fx);
      fx = ex * cos(fy) - fx - a;
      fy = ex * sin(fy) - fy - b;
      fd = fx * fx + fy * fy;
      z++;
    }
  return z;
}

int iterapfelcc(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE x2, y2;
  DOUBLE a, b;
  a = ap;
  b = bp;
  fx = a; // Startwert = Wert nach erstem Schritt
  fy = b;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 16))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = - fx * fy * 2 + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int itership(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE x2, y2;
  DOUBLE a, b;
  a = ap;
  b = bp;
  fx = a; // Startwert = Wert nach erstem Schritt
  fy = b;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 16))
    {
      if (fx < 0) fx = -fx;
      if (fy < 0) fy = -fy;
      x2 = fx * fx;
      y2 = fy * fy;
      fy = - fx * fy * 2 + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }
  return z;
}

int iterapfel4(DOUBLE ap, DOUBLE bp, int maxcount)
{
  int z;
  DOUBLE a, b;
  DOUBLE fd;
  DOUBLE fx, fy;
  DOUBLE x2, y2;
  a = ap;
  b = bp;
  fx = a;
  fy = b;
  z = 0;
  fd = 0;
  while ((z < maxcount) && (fd < 40))
    {
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2;
      fx = x2 - y2;
      x2 = fx * fx;
      y2 = fy * fy;
      fy = fx * fy * 2 + b;
      fx = x2 - y2 + a;
      fd = x2 + y2;
      z++;
    }
  return z;
}


int itercollatz(DOUBLE a, DOUBLE b, int maxcount)
{
  int ct = 0;
  Komplex z(a, b);
  DOUBLE fd = 0;
  while ((ct < maxcount) && (fd < 440))
    {
      // z = (1.0 + z * 4 - (1 + z * 2) * Komplex::cos(z * M_PI)) * 0.25;
      z = (2.0 + z * 7 - (2.0 + z * 5) * Komplex::cos(z * M_PI)) * 0.25;
      fd = z.abs2();
      ct++;
    }
  return ct;
}

int iterC2(DOUBLE crp, DOUBLE cip, int maxcount)
{
  DOUBLE ar = 1, ai = 0;
  DOUBLE ar2, ai2;
  DOUBLE cr = crp, ci = cip;
  DOUBLE c2 = cr * cr + ci * ci;

  int z = 0;
  DOUBLE a2 = 0;
  while ((z < maxcount) && (a2 < 4))
    {
      a2 = ar * ar + ai * ai;
      DOUBLE n = a2 * c2;
      ar2 = (a2 * (ar * cr + ai * ci) + c2 * ar) / n;
      ai2 = (a2 * (-ar * ci + ai * cr) - c2 * ai) / n;
      ar = ar2;
      ai = ai2;
      z++;
    }
  return z;
}

int iterCS(DOUBLE a, DOUBLE b, int maxcount)
{
  int z = 0;
  DOUBLE fd = 0;
  DOUBLE x = 0;
  DOUBLE y = 0;
  while ((z < maxcount) && (fd < 40))
    {
      DOUBLE pi_r = M_PI * x;
      DOUBLE pi_i = M_PI * y;
      DOUBLE cosz_r = cos(pi_r) * cosh(pi_i);
      DOUBLE cosz_i = - sin(pi_r) * sinh(pi_i);
      DOUBLE fak_r = 2 + 5 * x;
      DOUBLE fak_i = 5 * y;
      DOUBLE nx = 2 + 7 * x - fak_r * cosz_r + fak_i * cosz_i + a;
      DOUBLE ny = 7 * y - fak_r * cosz_i - fak_i * cosz_r + b;
      x = nx;
      y = ny;
      fd = x * x + y * y;
      z++;
    }
  return z;
}

void frame_(int x1, int y1, int x2, int y2)
{
  for (int x = x1; x <= x2; x++)
    {
      SetAlphaCursor(x, y1);
      PutChar(R_OU);
      SetAlphaCursor(x, y2);
      PutChar(R_OU);
    }
  for (int y = y1; y <= y2; y++)
    {
      SetAlphaCursor(x1, y);
      PutChar(R_LR);
      SetAlphaCursor(x2, y);
      PutChar(R_LR);
    }
  SetAlphaCursor(x2, y2);
  PutChar(R_RU);
  SetAlphaCursor(x1, y2);
  PutChar(R_LU);
  SetAlphaCursor(x2, y1);
  PutChar(R_RO);
  SetAlphaCursor(x1, y1);
  PutChar(R_LO);
}

void itersel(const Fractal*& fractal)
{
  vector<string> men;
  int select;
  int i = 0;
  while (fractals[i] != NULL)
    {
      men.push_back(fractals[i]->name);
      i++;
    }
  PushAlpha();
  select = Menu(men, 5, 5, 24, 18);
  if (select >= 0)
    {
      fractal = fractals[select];
    }
  PopAlpha();
}

void SelectColor(int& ColorMode)
{
  vector<string> men;
  vector<int> mid;
  int select;

  men.push_back("~Grau");
  mid.push_back(CT_GREY + CT_ALT);
  men.push_back("Grau linear");
  mid.push_back(CT_GREY + CT_NORM);
  men.push_back("~Blau-Gelb");
  mid.push_back(CT_BLUE_YELLOW + CT_ALT);
  men.push_back("Blau-Gelb ~linear");
  mid.push_back(CT_BLUE_YELLOW + CT_NORM + CT_ALT);
  men.push_back("~Rot-Gelb");
  mid.push_back(CT_RED_YELLOW + CT_ALT);
  men.push_back("Rot-Gelb ~linear");
  mid.push_back(CT_RED_YELLOW + CT_NORM);
  men.push_back("~Spektrum");
  mid.push_back(CT_RAINBOW);
  men.push_back("Spektrum linear");
  mid.push_back(CT_RAINBOW + CT_ALT + CT_NORM);
  men.push_back("~Schwarz-Weiß");
  mid.push_back(CT_BW);

  PushAlpha();
  select = Menu(men, mid, 5, 5, 24, 18);
  if (select >= 0)
    {
      ColorMode = select;
      setcolor(ColorMode);
    }
  PopAlpha();
}

void selectDepth(int& maxcount)
{
  vector<string> men;
  vector<int> value;
  int select;
  int p = 64;
  while (p < MAXCOUNT_MAX)
    {
      value.push_back(p);
      men.push_back(to_string(p));
      p += p;
    }
  PushAlpha();
  select = Menu(men, 7, 3, 24, 18);
  if (select >= 0)
    {
      maxcount = value[select];
    }
  PopAlpha();
}

void reset(int& maxcount, iterfunc*& iter, int& ColorMode)
{
  maxcount = MAXCOUNT_DEFAULT;
  iter = &iterapfel;
  /* Modifikator = 2.0;*/
  ColorMode = CT_BLUE_YELLOW + CT_ALT;
  if (display)
    setcolor(ColorMode);
}

void setcolor(int ColorMode)
{
  vector<int> red(maxval + 1);
  vector<int> green(maxval + 1);
  vector<int> blue(maxval + 1);

  int select = ColorMode % 16;
  bool alternierend = (ColorMode & CT_ALT) > 0;
  switch (select)
    {
    case CT_GREY:
      for (int z = 0; z <= maxval; z++)
        {
          red[z] = green[z] = blue[z] = (maxval - z) * 255 / maxval;
        }
      break;

    case CT_BLUE_YELLOW:
      for (int z = 0; z <= maxval; z++)
        {
#if 1
          double fx = z * 3.14159265 * 2 / (maxval + 1);
          int i1 = (int)((sin(fx) + 1) * 255 / 2);
          int i2 = (int)((cos(fx) + 1) * 255 / 2);
#else
          int i1 = z * 255 / maxval;
          int i2 = 255 - i1;
#endif
          red[z] = green[z] = i1;
          blue[z] =  i2;
        }
      break;

    case CT_RED_YELLOW:
      for (int z = 0; z <= maxval; z++)
        {
          double fx = z * 3.14159265 * 2 / (maxval + 1);
          int i1 = (int)((-cos(fx) + 1) * 255 / 2);
          blue[z] = 0;
          green[z] = i1;
          red[z] =  255;
        }
      break;

    case CT_RAINBOW:
      for (int z = 0; z <= maxval; z++)
        {
          double fx = z * 3.14159265 * 2  / (maxval + 1);
          int b = (int)((cos(fx) + 0.5) * 255 / 1.5);
          int g = (int)((cos(fx - M_PI * 2 / 3) + 0.5) * 255 / 1.5);
          int r = (int)((cos(fx + M_PI * 2 / 3) + 0.5) * 255 / 1.5);
          red[z] = r > 0 ? r : 0;
          green[z] = g > 0 ? g : 0;
          blue[z] = b > 0 ? b : 0;
        }
      break;

    case CT_BW:
      for (int z = 0; z <= maxval; z += 2)
        {
          red[z] = green[z] = blue[z] = 255;
          red[z + 1] = green[z + 1] = blue[z + 1] = 0;
        }
      break;
    }
  if (alternierend)
    {
      for (int z = 0; z <= maxval; z += 2)
        {
          red[z] = HELLDUNKELMODIFIER(red[z]);
          green[z] = HELLDUNKELMODIFIER(green[z]);
          blue[z] = HELLDUNKELMODIFIER(blue[z]);
        }
    }
  for (int z = 0; z <= maxval; z++)
    SetGrayColor(z, red[z], green[z], blue[z]);
}

void makeColorImage(const Image& ptr, ColorImage& co)
{
  co.create(ptr.xsize, ptr.ysize, 255);

  for (int y = 0; y < ptr.ysize; y++)
    for (int x = 0; x < ptr.xsize; x++)
      {
        int gw = ptr.getPixel(x, y);
        unsigned char r, g, b;
        GetGrayColor(ptr, gw, r, g, b); // color table lookup
        co.setPixel(x, y, ColorValue(r, g, b));
      }
}

void SaveToImage(const string& bfn, const Image& ptr)
{
  string fn = findFilename(bfn + ".jpg");

  if (fn.empty())
    {
      if (display)
        {
          PrintStatus("Kann Bild " + fn + " nicht speichern!");
        }
    }
  else
    {
      if (display)
        {
          PrintStatus("Bild " + fn + " wird gespeichert.");
        }

      ColorImage co;
      makeColorImage(ptr, co);

      co.write(fn);
      /*
        ostringstream so;
        so << tostring(a0 + 0.5 * da) << " " << tostring(b0 + 0.5 * db) << " " << tostring(da);
        system(("exiftool -overwrite_original -q -comment='" + so.str() + "' '" + fn + "'").c_str());
      */
      if (display)
        PrintStatus();
    }
}

void SaveToVideo(VideoFile& vout, const Image& ptr)
{
  if (display)
    {
      PrintStatus("Bild wird gespeichert.");
    }

  ColorImage co;
  makeColorImage(ptr, co);

  vout.write(co);

  if (display)
    PrintStatus();
}

void writeVector(ostream& os, double x, double y, double z)
{
  os << "<" << x << ", " << y << ", " << z << ">";
}

void writeColor(ostream& os, ColorValue cv)
{
  os << "color ";
  writeVector(os,
              1.0 * cv.red / 256,
              1.0 * cv.green / 256,
              1.0 * cv.blue / 256);
  os << endl;
}

void writePixel(ostream& os, const Image& height, double x, double y)
{
  writeVector(os, x, GetVal(height, x, y) / 3.0, y);
}

void writeTriangle(ostream& os, const Image& height, const ColorImage& co,
                   int x1, int y1, int x2, int y2, int x3, int y3)
{
  ColorValue cv = (co.getPixel(x1, y1) + co.getPixel(x2, y2) + co.getPixel(x3, y3)) / 3;
  writeColor(os, cv);
  os << "triangle ";
  writePixel(os, height, x1, y1);
  os << ", ";
  writePixel(os, height, x2, y2);
  os << ", ";
  writePixel(os, height, x3, y3);
  os << endl;
}

void Save3d(const string& bfn, const Image& height, const Image& ptr)
{
  string fn = findFilename(bfn + ".cv");

  if (fn.empty())
    {
      if (display)
        {
          PrintStatus("Kann 3d-Datei " + fn + " nicht speichern!");
        }
    }
  else
    {
      if (display)
        {
          PrintStatus("3d-Datei " + fn + " wird gespeichert.");
        }

      ColorImage co;
      makeColorImage(ptr, co);

      ofstream os(fn.c_str());
      for (int y = 0; y < ptr.ysize - 1; y++)
        for (int x = 0; x < ptr.xsize - 1; x++)
          {
            writeTriangle(os, height, co, x, y, x + 1, y, x + 1, y + 1);
            writeTriangle(os, height, co, x + 1, y + 1, x, y + 1, x, y);
          }
    }
  if (display)
    PrintStatus();
}

int gc()
{
  int c = GetKey();
  return c;
}
/*
string toString(int e)
{
  bool sign = e < 0;
  if (sign)
    e = -e;
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
  //cout << v << " " << sizeof(v) << " " << sizeof(long long int) << endl;
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
*/
void ParameterMessage()
{
  printf("\nvis2 (c)OWeh\n\n");
  printf("vis [-b|-B] commandstring\n");
}

void PrintA(string s)
/* Ausschriften mit Attribut */
{
  if (!display)
    return;
  bool high = false;
  for (unsigned int ct = 0; ct < s.length(); ++ct)
    {
      char c = s[ct];
      if (c == '~')
        {
          SetAttribute(-1, -1, -1, 1);
          high = true;
        }
      else
        {
          PutChar(c);
          if (high)
            SetAttribute(-1, -1, -1, 0);
          high = false;
        }
    }
  if (high)
    SetAttribute(-1, -1, -1, 0);
}

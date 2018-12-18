# 1 "circledetection.ag"
#include <image.h>
# 2 "circledetection.ag"

# 3 "circledetection.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <iomanip>
/*AppGen:GlobalEnd*/
# 4 "circledetection.ag"

# 5 "circledetection.ag"
/*AppGen
# 6 "circledetection.ag"
 prog: Anwendung DFT auf Bilder
# 7 "circledetection.ag"
 %% Beschreibung Parameter
# 8 "circledetection.ag"
 % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 9 "circledetection.ag"
 opt: r, radius, double, radius, Fixed circle radius, 0
# 10 "circledetection.ag"
AppGen*/
# 11 "circledetection.ag"

# 12 "circledetection.ag"
LineSeg InputLine(const Image& img, const string& p)
# 13 "circledetection.ag"
{
# 14 "circledetection.ag"
  LineSeg res;
# 15 "circledetection.ag"
  Print("Eingabe " + p + "\n");
# 16 "circledetection.ag"
  IPoint p1 = SelPoint(img);
# 17 "circledetection.ag"
  Marker(1, p1, 3, 5, img);
# 18 "circledetection.ag"
  IPoint p2 = SelPoint(img);
# 19 "circledetection.ag"
  Marker(1, p2, 3, 5, img);
# 20 "circledetection.ag"
  Print("G - Gerade\n");
# 21 "circledetection.ag"
  Print("A - Strahl\n");
# 22 "circledetection.ag"
  Print("I - Inverser Strahl\n");
# 23 "circledetection.ag"
  Print("S - Strecke\n");
# 24 "circledetection.ag"
  char c = GetChar();
# 25 "circledetection.ag"

# 26 "circledetection.ag"
  switch (c)
# 27 "circledetection.ag"
    {
# 28 "circledetection.ag"
    case 'G':
# 29 "circledetection.ag"
    case 'g':
# 30 "circledetection.ag"
      res = LineSeg(p1, p2, LineSeg::line);
# 31 "circledetection.ag"
      break;
# 32 "circledetection.ag"
    case 'A':
# 33 "circledetection.ag"
    case 'a':
# 34 "circledetection.ag"
      res = LineSeg(p1, p2, LineSeg::ray);
# 35 "circledetection.ag"
      break;
# 36 "circledetection.ag"
    case 'I':
# 37 "circledetection.ag"
    case 'i':
# 38 "circledetection.ag"
      res = LineSeg(p1, p2, LineSeg::inv_ray);
# 39 "circledetection.ag"
      break;
# 40 "circledetection.ag"
    case 'S':
# 41 "circledetection.ag"
    case 's':
# 42 "circledetection.ag"
    default:
# 43 "circledetection.ag"
      res = LineSeg(p1, p2, LineSeg::segment);
# 44 "circledetection.ag"
      break;
# 45 "circledetection.ag"
    }
# 46 "circledetection.ag"

# 47 "circledetection.ag"
  Marker(1, p1, 0, 5, img);
# 48 "circledetection.ag"
  Marker(1, p2, 0, 5, img);
# 49 "circledetection.ag"
  return res;
# 50 "circledetection.ag"
}
# 51 "circledetection.ag"

# 52 "circledetection.ag"
bool InputCircle(const Image& img, double rad, Circle& c)
# 53 "circledetection.ag"
{
# 54 "circledetection.ag"
  c = Circle();
# 55 "circledetection.ag"
  Print("Eingabe Mittelpunkt \n");
# 56 "circledetection.ag"
  int rc;
# 57 "circledetection.ag"

# 58 "circledetection.ag"
  IPoint p1 = SelPoint(img, rc);
# 59 "circledetection.ag"
  if (rc >= 0)
# 60 "circledetection.ag"
    {
# 61 "circledetection.ag"
      Marker(1, p1, 3, 5, img);
# 62 "circledetection.ag"
      if (rad == 0)
# 63 "circledetection.ag"
        {
# 64 "circledetection.ag"
          Print("Eingabe Randpunkt \n");
# 65 "circledetection.ag"

# 66 "circledetection.ag"
          IPoint p2 = SelPoint(img, rc);
# 67 "circledetection.ag"
          if (rc >= 0)
# 68 "circledetection.ag"
            {
# 69 "circledetection.ag"
              c  = Circle(p1, (p2 - p1).r());
# 70 "circledetection.ag"
              Marker(1, p1, 0, 5, img);
# 71 "circledetection.ag"
            }
# 72 "circledetection.ag"
        }
# 73 "circledetection.ag"
      else
# 74 "circledetection.ag"
        {
# 75 "circledetection.ag"
          c = Circle(p1, rad);
# 76 "circledetection.ag"
          Marker(1, p1, 0, 5, img);
# 77 "circledetection.ag"
        }
# 78 "circledetection.ag"
    }
# 79 "circledetection.ag"

# 80 "circledetection.ag"
  return (rc >= 0);
# 81 "circledetection.ag"
}
# 82 "circledetection.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - Anwendung DFT auf Bilder" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] " << endl;
  cout << "Options:" << endl;
  cout << "-r <d>" << "  " << "--radius=<d>" << "   ";
  cout << "Fixed circle radius (default: 0)" << endl ; 
  exit(1);
}

void error(const string &msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

double ptod(const char *para)
{
  char *end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in floating point value: ") + para);
  return res;
}

int main(int argc, char **argv)
{
  double radius = 0;
  static struct option ag_long_options[] =
  {
    {"radius", required_argument, 0, 'r' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":r:", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'r':
          radius = ptod(optarg);
          break;

        default:
          error("error in options");
        }
    }
/*AppGen:MainEnd*/
# 86 "circledetection.ag"
  Image img;
# 87 "circledetection.ag"
  img.create(1024, 1024, 255);
# 88 "circledetection.ag"
  Image mrk;
# 89 "circledetection.ag"
  mrk.create(1024, 1024, 8);
# 90 "circledetection.ag"
  clearImg(img);
# 91 "circledetection.ag"
  clearImg(mrk);
# 92 "circledetection.ag"
  Show(OVERLAY, img, mrk);
# 93 "circledetection.ag"

# 94 "circledetection.ag"
  vector<Circle> circles;
# 95 "circledetection.ag"
  Circle c;
# 96 "circledetection.ag"
  while (InputCircle(mrk, radius, c))
# 97 "circledetection.ag"
    {
# 98 "circledetection.ag"
      circles.push_back(c);
# 99 "circledetection.ag"
      draw(c, mrk, 1);
# 100 "circledetection.ag"
    }
# 101 "circledetection.ag"

# 102 "circledetection.ag"
  vector<Point> points;
# 103 "circledetection.ag"
  WindowWalker w(mrk);
# 104 "circledetection.ag"
  for (w.init(); !w.ready(); w.next())
# 105 "circledetection.ag"
    {
# 106 "circledetection.ag"
      double r = drand48();
# 107 "circledetection.ag"
      if (r < 0.1)
# 108 "circledetection.ag"
        if (mrk.getPixel(w) > 0)
# 109 "circledetection.ag"
          points.push_back(w);
# 110 "circledetection.ag"
    }
# 111 "circledetection.ag"

# 112 "circledetection.ag"
  cout << "selected: " << points.size() << " Points selected" << endl;
# 113 "circledetection.ag"

# 114 "circledetection.ag"
  for (Point p : points)
# 115 "circledetection.ag"
    {
# 116 "circledetection.ag"
      mrk.setPixel(IPoint(p), 3);
# 117 "circledetection.ag"
      img.setPixel(IPoint(p), 255);
# 118 "circledetection.ag"
    }
# 119 "circledetection.ag"
  FourierTrafo2D fft(img.xsize, img.ysize);
# 120 "circledetection.ag"
  while (GetChar() != 13) /* wait*/;
# 121 "circledetection.ag"

# 122 "circledetection.ag"
  fft.setInput(img);
# 123 "circledetection.ag"
  ImageD spectrumR, spectrumI;
# 124 "circledetection.ag"
  spectrumR.create(img);
# 125 "circledetection.ag"
  spectrumI.create(img);
# 126 "circledetection.ag"

# 127 "circledetection.ag"
  int xCenter = img.xsize / 2;
# 128 "circledetection.ag"
  int yCenter = img.ysize / 2;
# 129 "circledetection.ag"
  IPoint center(xCenter, yCenter);
# 130 "circledetection.ag"
  fft.getResult(spectrumR, spectrumI);
# 131 "circledetection.ag"
  WindowWalker s(spectrumR);
# 132 "circledetection.ag"
  double max = 0;
# 133 "circledetection.ag"
  for (s.init(); !s.ready(); s.next())
# 134 "circledetection.ag"
    {
# 135 "circledetection.ag"
      if (s != center)
# 136 "circledetection.ag"
        {
# 137 "circledetection.ag"
          double r = spectrumR.getPixel(s);
# 138 "circledetection.ag"
          double i = spectrumI.getPixel(s);
# 139 "circledetection.ag"
          double value = sqrt(r * r + i * i);
# 140 "circledetection.ag"
          if (value > max)
# 141 "circledetection.ag"
            max = value;
# 142 "circledetection.ag"
        }
# 143 "circledetection.ag"
    }
# 144 "circledetection.ag"
  cout << "max: " << max << endl;
# 145 "circledetection.ag"
  for (s.init(); !s.ready(); s.next())
# 146 "circledetection.ag"
    {
# 147 "circledetection.ag"
      double r = spectrumR.getPixel(s);
# 148 "circledetection.ag"
      double i = spectrumI.getPixel(s);
# 149 "circledetection.ag"
      double value = sqrt(r * r + i * i) / max;
# 150 "circledetection.ag"
      img.setPixelLimited(s, value * 255);
# 151 "circledetection.ag"
    }
# 152 "circledetection.ag"

# 153 "circledetection.ag"
  while (GetChar() != 13) /* wait*/;
# 154 "circledetection.ag"

# 155 "circledetection.ag"
  return OK;
# 156 "circledetection.ag"
}

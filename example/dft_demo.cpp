# 1 "dft_demo.ag"
#include <image.h>
# 2 "dft_demo.ag"

# 3 "dft_demo.ag"
#include "xdisplay.h"
# 4 "dft_demo.ag"

# 5 "dft_demo.ag"
/*AppGen
# 6 "dft_demo.ag"
  %%  Beschreibung des Programmes:
# 7 "dft_demo.ag"
  prog: spectra and manipulations
# 8 "dft_demo.ag"
  %% Beschreibung Parameter
# 9 "dft_demo.ag"
  % symbolischerName, Art, Typ, Variablenname, Erklärung, Default-Wert
# 10 "dft_demo.ag"
  para: file, optional, string, imgFile, image file, test.jpg
# 11 "dft_demo.ag"
  %% Beschreibung der Optionen
# 12 "dft_demo.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 13 "dft_demo.ag"
  opt: m, mode, char, operation, mode of manipulation, 'n'
# 14 "dft_demo.ag"
  opt2:  n - no operation
# 15 "dft_demo.ag"
  opt2:  c - conjugate complex
# 16 "dft_demo.ag"
  opt2:  p - power spectra
# 17 "dft_demo.ag"
  opt2:  m - magnitude
# 18 "dft_demo.ag"
  opt2:  s - square
# 19 "dft_demo.ag"
  opt2:  w - whitening
# 20 "dft_demo.ag"
  opt: o, output, string, outFile, output file, ""
# 21 "dft_demo.ag"
  opt: b, batch, void, batch, batch mode, false
# 22 "dft_demo.ag"
  opt: n, null, void, nobias, ignore bias, false
# 23 "dft_demo.ag"
  opt: l, log, void, logSpectra, logarithmic visualization, false
# 24 "dft_demo.ag"
  opt: w, wait, Void, wait, wait in batch mode,  false
# 25 "dft_demo.ag"
  opt: v, verbose, Void, verbose, be verbose,  false
# 26 "dft_demo.ag"
  opt: d, display, Void, display,  display of images,  false
# 27 "dft_demo.ag"
  opt: 6, xdiplay, void, xdisplay, display as matrix, false
# 28 "dft_demo.ag"
  opt: X, xdxsize, int, xdxsize, width of display, -1
# 29 "dft_demo.ag"
  opt: Y, xdysize, int, xdysize, height of display, -1
# 30 "dft_demo.ag"
  opt: t, notitle, void, noTitle, no titles, false
# 31 "dft_demo.ag"
  opt: h, help,   usage,   , this help
# 32 "dft_demo.ag"
  AppGen*/
# 33 "dft_demo.ag"

# 34 "dft_demo.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <iomanip>
#include <string>
#include <string>
bool wait = false;
bool verbose = false;
bool display = false;
/*AppGen:GlobalEnd*/
# 35 "dft_demo.ag"

# 36 "dft_demo.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - spectra and manipulations" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] [file] " << endl;
  cout << "  file - image file (optional, default:\"test.jpg\")" << endl;
  cout << "Options:" << endl;
  cout << "-m <c>" << "  " << "--mode=<c>   " << "   ";
  cout << "mode of manipulation (default: 'n')" << endl ; 
  cout << "                        " << "n - no operation" << endl; 
  cout << "                        " << "c - conjugate complex" << endl; 
  cout << "                        " << "p - power spectra" << endl; 
  cout << "                        " << "m - magnitude" << endl; 
  cout << "                        " << "s - square" << endl; 
  cout << "                        " << "w - whitening" << endl; 
  cout << "-o <s>" << "  " << "--output=<s> " << "   ";
  cout << "output file (default: \"\")" << endl ; 
  cout << "-b    " << "  " << "--batch      " << "   ";
  cout << "batch mode (default: false)" << endl ; 
  cout << "-n    " << "  " << "--null       " << "   ";
  cout << "ignore bias (default: false)" << endl ; 
  cout << "-l    " << "  " << "--log        " << "   ";
  cout << "logarithmic visualization (default: false)" << endl ; 
  cout << "-w    " << "  " << "--wait       " << "   ";
  cout << "wait in batch mode (default: false)" << endl ; 
  cout << "-v    " << "  " << "--verbose    " << "   ";
  cout << "be verbose (default: false)" << endl ; 
  cout << "-d    " << "  " << "--display    " << "   ";
  cout << "display of images (default: false)" << endl ; 
  cout << "-6    " << "  " << "--xdiplay    " << "   ";
  cout << "display as matrix (default: false)" << endl ; 
  cout << "-X <i>" << "  " << "--xdxsize=<i>" << "   ";
  cout << "width of display (default: -1)" << endl ; 
  cout << "-Y <i>" << "  " << "--xdysize=<i>" << "   ";
  cout << "height of display (default: -1)" << endl ; 
  cout << "-t    " << "  " << "--notitle    " << "   ";
  cout << "no titles (default: false)" << endl ; 
  cout << "-h    " << "  " << "--help       " << "   ";
  cout << "this help" << endl ; 
  exit(1);
}

void error(const string &msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

long int ptol(const char *para)
{
  char *end;
  long int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in integer value: ") + para);
  return res;
}

int main(int argc, char **argv)
{
  char operation = 'n';
  string outFile = "";
  bool batch = false;
  bool nobias = false;
  bool logSpectra = false;
  bool xdisplay = false;
  int xdxsize = -1;
  int xdysize = -1;
  bool noTitle = false;
  
  string imgFile = "test.jpg";
  static struct option ag_long_options[] =
  {
    {"mode", required_argument, 0, 'm' },
    {"output", required_argument, 0, 'o' },
    {"batch", no_argument, 0, 'b' },
    {"null", no_argument, 0, 'n' },
    {"log", no_argument, 0, 'l' },
    {"wait", no_argument, 0, 'w' },
    {"verbose", no_argument, 0, 'v' },
    {"display", no_argument, 0, 'd' },
    {"xdiplay", no_argument, 0, '6' },
    {"xdxsize", required_argument, 0, 'X' },
    {"xdysize", required_argument, 0, 'Y' },
    {"notitle", no_argument, 0, 't' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":m:o:bnlwvd6X:Y:th", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'm':
          operation = optarg[0];
          break;

        case 'o':
          outFile = optarg;
          break;

        case 'b':
          batch = true;
          break;

        case 'n':
          nobias = true;
          break;

        case 'l':
          logSpectra = true;
          break;

        case 'w':
          wait = true;
          break;

        case 'v':
          verbose = true;
          break;

        case 'd':
          display = true;
          break;

        case '6':
          xdisplay = true;
          break;

        case 'X':
          xdxsize = ptol(optarg);
          break;

        case 'Y':
          xdysize = ptol(optarg);
          break;

        case 't':
          noTitle = true;
          break;

        case 'h':
          usage();
          break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    imgFile = argv[optind++];

/*AppGen:MainEnd*/
# 40 "dft_demo.ag"

# 41 "dft_demo.ag"
  if (outFile.empty())
# 42 "dft_demo.ag"
    {
# 43 "dft_demo.ag"
      // wenn keine Ausgabe, dann ansehen
# 44 "dft_demo.ag"
      if (!xdisplay)
# 45 "dft_demo.ag"
        display = true;
# 46 "dft_demo.ag"
      wait = true;
# 47 "dft_demo.ag"
    }
# 48 "dft_demo.ag"

# 49 "dft_demo.ag"
  if (xdisplay)
# 50 "dft_demo.ag"
    wait = true;
# 51 "dft_demo.ag"

# 52 "dft_demo.ag"
  int xs, ys, mv, ch;
# 53 "dft_demo.ag"
  InfImgFile(imgFile, xs, ys, mv, ch);
# 54 "dft_demo.ag"

# 55 "dft_demo.ag"
  Image sourceImage;
# 56 "dft_demo.ag"
  sourceImage.read(imgFile);
# 57 "dft_demo.ag"
  if (verbose)
# 58 "dft_demo.ag"
    cout << "read " << imgFile << endl;
# 59 "dft_demo.ag"

# 60 "dft_demo.ag"
  // .. und anzeigen
# 61 "dft_demo.ag"
  if (display)
# 62 "dft_demo.ag"
    Show(ON, sourceImage, "original image");
# 63 "dft_demo.ag"

# 64 "dft_demo.ag"
  int xdx = 3 * xs;
# 65 "dft_demo.ag"
  int xdy = 2 * ys;
# 66 "dft_demo.ag"

# 67 "dft_demo.ag"
  // sizes of display given?
# 68 "dft_demo.ag"
  if (xdxsize > 0)
# 69 "dft_demo.ag"
    {
# 70 "dft_demo.ag"
      xdx = xdxsize;
# 71 "dft_demo.ag"
      if (xdysize > 0)
# 72 "dft_demo.ag"
        xdy = xdysize;
# 73 "dft_demo.ag"
      else
# 74 "dft_demo.ag"
        xdy = 2 * xdx / 3;
# 75 "dft_demo.ag"
    }
# 76 "dft_demo.ag"
  else if (xdysize > 0)
# 77 "dft_demo.ag"
    {
# 78 "dft_demo.ag"
      xdy = xdysize;
# 79 "dft_demo.ag"
      xdx = 3 * xdy / 2;
# 80 "dft_demo.ag"
    }
# 81 "dft_demo.ag"
  //  cout << xdx  << "x" << xdy << endl;
# 82 "dft_demo.ag"
  XDisplay xd(xdx, xdy, 6);
# 83 "dft_demo.ag"
  if (xdisplay)
# 84 "dft_demo.ag"
    {
# 85 "dft_demo.ag"
      xd.show();
# 86 "dft_demo.ag"
      xd.show(0, sourceImage);
# 87 "dft_demo.ag"
      if (!noTitle)
# 88 "dft_demo.ag"
        xd.setTitle(0, "original");
# 89 "dft_demo.ag"
    }
# 90 "dft_demo.ag"

# 91 "dft_demo.ag"
  // Bild mittelpunkt = Frequenz Null des Spektrums
# 92 "dft_demo.ag"
  int xCenter = xs / 2;
# 93 "dft_demo.ag"
  int yCenter = ys / 2;
# 94 "dft_demo.ag"
  Point center(xCenter, yCenter);
# 95 "dft_demo.ag"

# 96 "dft_demo.ag"
  FourierTrafo2D fft;
# 97 "dft_demo.ag"
  fft.setInput(sourceImage);
# 98 "dft_demo.ag"

# 99 "dft_demo.ag"
  FourierTrafo2D ffti(ys, xs, false);
# 100 "dft_demo.ag"

# 101 "dft_demo.ag"
  // Zielbild für FT
# 102 "dft_demo.ag"
  ImageD spectra_real;
# 103 "dft_demo.ag"
  spectra_real.create(xs, ys);
# 104 "dft_demo.ag"
  ImageD spectra_imag;
# 105 "dft_demo.ag"
  spectra_imag.create(xs, ys);
# 106 "dft_demo.ag"

# 107 "dft_demo.ag"
  ImageD spectra_m_real;
# 108 "dft_demo.ag"
  spectra_m_real.create(xs, ys);
# 109 "dft_demo.ag"
  ImageD spectra_m_imag;
# 110 "dft_demo.ag"
  spectra_m_imag.create(xs, ys);
# 111 "dft_demo.ag"

# 112 "dft_demo.ag"
  Image result;
# 113 "dft_demo.ag"
  result.create(xs, ys, mv);
# 114 "dft_demo.ag"

# 115 "dft_demo.ag"
  ImageD dresult;
# 116 "dft_demo.ag"
  dresult.create(xs, ys);
# 117 "dft_demo.ag"

# 118 "dft_demo.ag"
  if (display)
# 119 "dft_demo.ag"
    Show(GRAY, result, "result");
# 120 "dft_demo.ag"

# 121 "dft_demo.ag"
  double fg = -1;
# 122 "dft_demo.ag"
  bool newfilter = true;
# 123 "dft_demo.ag"

# 124 "dft_demo.ag"
  bool sZoom = false; // zoom of spectra
# 125 "dft_demo.ag"
  bool ready = false;
# 126 "dft_demo.ag"
  while (!ready)
# 127 "dft_demo.ag"
    {
# 128 "dft_demo.ag"
      if (batch) // nur ein Durchlauf
# 129 "dft_demo.ag"
        ready = true;
# 130 "dft_demo.ag"

# 131 "dft_demo.ag"
      fft.getResult(spectra_real, spectra_imag);
# 132 "dft_demo.ag"

# 133 "dft_demo.ag"
      if (verbose)
# 134 "dft_demo.ag"
        cout << "spectrum calculated" << endl;
# 135 "dft_demo.ag"

# 136 "dft_demo.ag"
      // Gleichanteil loeschen ?
# 137 "dft_demo.ag"
      if (nobias)
# 138 "dft_demo.ag"
        {
# 139 "dft_demo.ag"
          spectra_real.setPixel(xCenter, yCenter, 0);
# 140 "dft_demo.ag"
          spectra_imag.setPixel(xCenter, yCenter, 0);
# 141 "dft_demo.ag"
        }
# 142 "dft_demo.ag"

# 143 "dft_demo.ag"
      if (xdisplay)
# 144 "dft_demo.ag"
        {
# 145 "dft_demo.ag"
          xd.show(1, spectra_real, logSpectra);
# 146 "dft_demo.ag"
          if (sZoom)
# 147 "dft_demo.ag"
            xd.setZoom(1, 4);
# 148 "dft_demo.ag"
          if (!noTitle)
# 149 "dft_demo.ag"
            xd.setTitle(1, "spectrum (real)");
# 150 "dft_demo.ag"
          xd.show(2, spectra_imag, logSpectra);
# 151 "dft_demo.ag"
          if (sZoom)
# 152 "dft_demo.ag"
            xd.setZoom(2, 4);
# 153 "dft_demo.ag"
          if (!noTitle)
# 154 "dft_demo.ag"
            xd.setTitle(2, "spectrum (imag)");
# 155 "dft_demo.ag"
        }
# 156 "dft_demo.ag"

# 157 "dft_demo.ag"
      WindowWalker all(spectra_real);
# 158 "dft_demo.ag"

# 159 "dft_demo.ag"
      string s;
# 160 "dft_demo.ag"
      // manipulate spectra
# 161 "dft_demo.ag"
      switch (operation)
# 162 "dft_demo.ag"
        {
# 163 "dft_demo.ag"
        case 'c':
# 164 "dft_demo.ag"
          s = "conjugated";
# 165 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 166 "dft_demo.ag"
            {
# 167 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 168 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 169 "dft_demo.ag"
              spectra_m_real.setPixel(all, r);
# 170 "dft_demo.ag"
              spectra_m_imag.setPixel(all, -i);
# 171 "dft_demo.ag"
            }
# 172 "dft_demo.ag"
          break;
# 173 "dft_demo.ag"
        case 'n':
# 174 "dft_demo.ag"
          s = "noop";
# 175 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 176 "dft_demo.ag"
            {
# 177 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 178 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 179 "dft_demo.ag"
              spectra_m_real.setPixel(all, r);
# 180 "dft_demo.ag"
              spectra_m_imag.setPixel(all, i);
# 181 "dft_demo.ag"
            }
# 182 "dft_demo.ag"
          break;
# 183 "dft_demo.ag"
        case 's':
# 184 "dft_demo.ag"
          s = "square";
# 185 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 186 "dft_demo.ag"
            {
# 187 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 188 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 189 "dft_demo.ag"
              spectra_m_real.setPixel(all, r * r - i * i);
# 190 "dft_demo.ag"
              spectra_m_imag.setPixel(all, r * i + i * r);
# 191 "dft_demo.ag"
            }
# 192 "dft_demo.ag"
          break;
# 193 "dft_demo.ag"

# 194 "dft_demo.ag"
        case 'p':
# 195 "dft_demo.ag"
          s = "power";
# 196 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 197 "dft_demo.ag"
            {
# 198 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 199 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 200 "dft_demo.ag"
              spectra_m_real.setPixel(all, r * r + i * i);
# 201 "dft_demo.ag"
              spectra_m_imag.setPixel(all, 0);
# 202 "dft_demo.ag"
            }
# 203 "dft_demo.ag"
          break;
# 204 "dft_demo.ag"

# 205 "dft_demo.ag"
        case 'h':
# 206 "dft_demo.ag"
        {
# 207 "dft_demo.ag"
          s = "highpass";
# 208 "dft_demo.ag"
          if (newfilter || fg < 0)
# 209 "dft_demo.ag"
            fg = InputD("cut-off frequency of highpass: ");
# 210 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 211 "dft_demo.ag"
            {
# 212 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 213 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 214 "dft_demo.ag"
              Point dd = Point(all) - center;
# 215 "dft_demo.ag"
              if (dd.length() < fg)
# 216 "dft_demo.ag"
                {
# 217 "dft_demo.ag"
                  r = 0;
# 218 "dft_demo.ag"
                  i = 0;
# 219 "dft_demo.ag"
                }
# 220 "dft_demo.ag"
              spectra_m_real.setPixel(all, r);
# 221 "dft_demo.ag"
              spectra_m_imag.setPixel(all, i);
# 222 "dft_demo.ag"

# 223 "dft_demo.ag"
            }
# 224 "dft_demo.ag"
        }
# 225 "dft_demo.ag"
        break;
# 226 "dft_demo.ag"
        case 'l':
# 227 "dft_demo.ag"
        {
# 228 "dft_demo.ag"
          s = "lowpass";
# 229 "dft_demo.ag"
          if (newfilter || fg < 0)
# 230 "dft_demo.ag"
            fg = InputD("cut-off frequency of lowpass: ");
# 231 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 232 "dft_demo.ag"
            {
# 233 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 234 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 235 "dft_demo.ag"
              Point dd = Point(all) - center;
# 236 "dft_demo.ag"
              if (dd.length() > fg)
# 237 "dft_demo.ag"
                {
# 238 "dft_demo.ag"
                  r = 0;
# 239 "dft_demo.ag"
                  i = 0;
# 240 "dft_demo.ag"
                }
# 241 "dft_demo.ag"
              spectra_m_real.setPixel(all, r);
# 242 "dft_demo.ag"
              spectra_m_imag.setPixel(all, i);
# 243 "dft_demo.ag"

# 244 "dft_demo.ag"
            }
# 245 "dft_demo.ag"
        }
# 246 "dft_demo.ag"
        break;
# 247 "dft_demo.ag"
        case 'm':
# 248 "dft_demo.ag"
          s = "magnitude+phase";
# 249 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 250 "dft_demo.ag"
            {
# 251 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 252 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 253 "dft_demo.ag"
              spectra_m_real.setPixel(all, sqrt(r * r + i * i));
# 254 "dft_demo.ag"
              spectra_m_imag.setPixel(all, atan2(i, r));
# 255 "dft_demo.ag"
            }
# 256 "dft_demo.ag"
          break;
# 257 "dft_demo.ag"

# 258 "dft_demo.ag"
        case 'w':
# 259 "dft_demo.ag"
        {
# 260 "dft_demo.ag"
          s = "whitened";
# 261 "dft_demo.ag"
          for (all.init(); !all.ready(); all.next())
# 262 "dft_demo.ag"
            {
# 263 "dft_demo.ag"
              double r = spectra_real.getPixel(all);
# 264 "dft_demo.ag"
              double i = spectra_imag.getPixel(all);
# 265 "dft_demo.ag"
              double a = sqrt(r * r + i * i + 1);
# 266 "dft_demo.ag"
              spectra_m_real.setPixel(all, r / a);
# 267 "dft_demo.ag"
              spectra_m_imag.setPixel(all, i / a);
# 268 "dft_demo.ag"
            }
# 269 "dft_demo.ag"
        }
# 270 "dft_demo.ag"
        break;
# 271 "dft_demo.ag"

# 272 "dft_demo.ag"
        default:
# 273 "dft_demo.ag"
          cerr << (string("mode ") + operation + " not implemented") << endl;
# 274 "dft_demo.ag"
        }
# 275 "dft_demo.ag"

# 276 "dft_demo.ag"
      if (verbose)
# 277 "dft_demo.ag"
        cout << s << endl;
# 278 "dft_demo.ag"

# 279 "dft_demo.ag"
      if (xdisplay)
# 280 "dft_demo.ag"
        {
# 281 "dft_demo.ag"
          xd.show(3, spectra_m_real, logSpectra);
# 282 "dft_demo.ag"
          if (sZoom)
# 283 "dft_demo.ag"
            xd.setZoom(3, 4);
# 284 "dft_demo.ag"
          if (!noTitle)
# 285 "dft_demo.ag"
            xd.setTitle(3, "spectrum (real) - " + s);
# 286 "dft_demo.ag"
          xd.show(4, spectra_m_imag, logSpectra);
# 287 "dft_demo.ag"
          if (sZoom)
# 288 "dft_demo.ag"
            xd.setZoom(4, 4);
# 289 "dft_demo.ag"
          if (!noTitle)
# 290 "dft_demo.ag"
            xd.setTitle(4, "spectrum (imag) - " + s);
# 291 "dft_demo.ag"
        }
# 292 "dft_demo.ag"

# 293 "dft_demo.ag"
      ffti.setInput(spectra_m_real,
# 294 "dft_demo.ag"
                    spectra_m_imag);
# 295 "dft_demo.ag"
      ffti.getResult(result, ADAPTIVE);
# 296 "dft_demo.ag"
      ffti.getResult(dresult);
# 297 "dft_demo.ag"

# 298 "dft_demo.ag"
      if (verbose)
# 299 "dft_demo.ag"
        cout << "transformed back" << endl;
# 300 "dft_demo.ag"

# 301 "dft_demo.ag"
      if (xdisplay)
# 302 "dft_demo.ag"
        {
# 303 "dft_demo.ag"
          xd.show(5, result);
# 304 "dft_demo.ag"
          if (!noTitle)
# 305 "dft_demo.ag"
            xd.setTitle(5, "result");
# 306 "dft_demo.ag"
        }
# 307 "dft_demo.ag"

# 308 "dft_demo.ag"
      if (!batch)
# 309 "dft_demo.ag"
        {
# 310 "dft_demo.ag"
          ClearAlpha();
# 311 "dft_demo.ag"

# 312 "dft_demo.ag"
          Print("\n");
# 313 "dft_demo.ag"
          Print("operation = " + s + "\n");
# 314 "dft_demo.ag"
          Print(string("mode = ") + (logSpectra ? "logarithmic" : "linear") + "\n");
# 315 "dft_demo.ag"
          Print(string(nobias ? "without bias" : "with bias") + "\n");
# 316 "dft_demo.ag"

# 317 "dft_demo.ag"
          Print("\n");
# 318 "dft_demo.ag"
          Print("n - no operation\n");
# 319 "dft_demo.ag"
          Print("c - conjugate complex\n");
# 320 "dft_demo.ag"
          Print("p - power spectrum\n");
# 321 "dft_demo.ag"
          Print("m - magnitude spectrum\n");
# 322 "dft_demo.ag"
          Print("h - highpass\n");
# 323 "dft_demo.ag"
          Print("l - lowpass\n");
# 324 "dft_demo.ag"
          Print("w - whitening image\n");
# 325 "dft_demo.ag"

# 326 "dft_demo.ag"
          // Print("s - square spectrum\n");
# 327 "dft_demo.ag"
          Print("-\n");
# 328 "dft_demo.ag"
          Print("L - switch linear <-> logarithmic\n");
# 329 "dft_demo.ag"

# 330 "dft_demo.ag"
          newfilter = false;
# 331 "dft_demo.ag"
          bool keyValid = false;
# 332 "dft_demo.ag"
          while (!keyValid)
# 333 "dft_demo.ag"
            {
# 334 "dft_demo.ag"
              int c = GetChar();
# 335 "dft_demo.ag"
              // Printf("%d\n",c);
# 336 "dft_demo.ag"
              keyValid = true;
# 337 "dft_demo.ag"
              switch (c)
# 338 "dft_demo.ag"
                {
# 339 "dft_demo.ag"
                case 'p':
# 340 "dft_demo.ag"
                case 's':
# 341 "dft_demo.ag"
                case 'm':
# 342 "dft_demo.ag"
                case 'w':
# 343 "dft_demo.ag"
                case 'n':
# 344 "dft_demo.ag"
                case 'c':
# 345 "dft_demo.ag"
                case 'h':
# 346 "dft_demo.ag"
                case 'l':
# 347 "dft_demo.ag"
                  newfilter = true;
# 348 "dft_demo.ag"
                  operation = c;
# 349 "dft_demo.ag"
                  break;
# 350 "dft_demo.ag"
                case 'L':
# 351 "dft_demo.ag"
                  logSpectra = ! logSpectra;
# 352 "dft_demo.ag"
                  break;
# 353 "dft_demo.ag"
                case 'b':
# 354 "dft_demo.ag"
                  nobias = ! nobias;
# 355 "dft_demo.ag"
                  break;
# 356 "dft_demo.ag"
                case 'z':
# 357 "dft_demo.ag"
                  sZoom = ! sZoom;
# 358 "dft_demo.ag"
                  break;
# 359 "dft_demo.ag"
                case 27:
# 360 "dft_demo.ag"
                case 'q':
# 361 "dft_demo.ag"
                case 'x':
# 362 "dft_demo.ag"
                  ready = true;
# 363 "dft_demo.ag"
                  break;
# 364 "dft_demo.ag"
                default:
# 365 "dft_demo.ag"
                  keyValid = false;
# 366 "dft_demo.ag"
                }
# 367 "dft_demo.ag"
            }
# 368 "dft_demo.ag"
        }
# 369 "dft_demo.ag"
    }
# 370 "dft_demo.ag"

# 371 "dft_demo.ag"
  // warten auf Tastendruck
# 372 "dft_demo.ag"
  if (batch & wait)
# 373 "dft_demo.ag"
    GetChar();
# 374 "dft_demo.ag"

# 375 "dft_demo.ag"
  if (!outFile.empty())
# 376 "dft_demo.ag"
    {
# 377 "dft_demo.ag"
      result.write(outFile);
# 378 "dft_demo.ag"
      if (verbose)
# 379 "dft_demo.ag"
        cout << "wrote output file " << outFile << endl;
# 380 "dft_demo.ag"
    }
# 381 "dft_demo.ag"
  return 0;
# 382 "dft_demo.ag"
}

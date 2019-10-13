# 1 "fcalib.ag"
#include <unistd.h>
# 2 "fcalib.ag"
#include <image.h>
# 3 "fcalib.ag"
#include <geo.h>
# 4 "fcalib.ag"
#include <Distortion.h>
# 5 "fcalib.ag"
#include <Distortion1.h>
# 6 "fcalib.ag"
#include <picio.h>
# 7 "fcalib.ag"
#include <lseg.h>
# 8 "fcalib.ag"
#include <visual/screen.h>
# 9 "fcalib.ag"
#include <visual/visual.h>
# 10 "fcalib.ag"
#include <Camera.h>
# 11 "fcalib.ag"
#include <filter.h>
# 12 "fcalib.ag"
#include <selfcalib.h>
# 13 "fcalib.ag"

# 14 "fcalib.ag"
#include <string>
# 15 "fcalib.ag"
#include <vector>
# 16 "fcalib.ag"
#include <fstream>
# 17 "fcalib.ag"

# 18 "fcalib.ag"
#include "filelist.h"
# 19 "fcalib.ag"

# 20 "fcalib.ag"
#include "pattern.inc"
# 21 "fcalib.ag"

# 22 "fcalib.ag"
#include "fc_references.h"
# 23 "fcalib.ag"
#include "exifsig.h"
# 24 "fcalib.ag"

# 25 "fcalib.ag"
#include "fcalib.h"
# 26 "fcalib.ag"

# 27 "fcalib.ag"
using namespace std;
# 28 "fcalib.ag"
using namespace ice;
# 29 "fcalib.ag"

# 30 "fcalib.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <iomanip>
#include <string>
#include <string>
#include "filelist.h"
/*AppGen:GlobalEnd*/
# 31 "fcalib.ag"

# 32 "fcalib.ag"
const int exif_file = 1;
# 33 "fcalib.ag"
const int exif_ignore = 2;
# 34 "fcalib.ag"
const int exif_user = 3;
# 35 "fcalib.ag"

# 36 "fcalib.ag"
int exifmode = exif_file;
# 37 "fcalib.ag"

# 38 "fcalib.ag"
int verboseSwitch = 0;
# 39 "fcalib.ag"
int displaySwitch = 0;
# 40 "fcalib.ag"

# 41 "fcalib.ag"
void NormalizeDistortion(double& x0, double& y0, double& d2, double& d4, int dimx, int dimy)
# 42 "fcalib.ag"
{
# 43 "fcalib.ag"
  double refsize = max(dimx, dimy) / 2.0;
# 44 "fcalib.ag"
  double refsize2 = refsize * refsize;
# 45 "fcalib.ag"
  double refsize4 = refsize2 * refsize2;
# 46 "fcalib.ag"

# 47 "fcalib.ag"
  x0 = x0 / dimx;
# 48 "fcalib.ag"
  y0 = y0 / dimy;
# 49 "fcalib.ag"
  d2 = d2 * refsize2;
# 50 "fcalib.ag"
  d4 = d4 * refsize4;
# 51 "fcalib.ag"
}
# 52 "fcalib.ag"

# 53 "fcalib.ag"
void Print(const Distortion1& distortion, int dimx, int dimy)
# 54 "fcalib.ag"
{
# 55 "fcalib.ag"
  double x0 = distortion.X0();
# 56 "fcalib.ag"
  double y0 = distortion.Y0();
# 57 "fcalib.ag"
  double d2 = distortion.D2();
# 58 "fcalib.ag"
  double d4 = distortion.D4();
# 59 "fcalib.ag"
  cout << "Verzeichnung              ";
# 60 "fcalib.ag"
  cout << "x0: " << x0 << "  y0: " << y0;
# 61 "fcalib.ag"
  cout << "  d2: " << d2 << "  d4: " << d4 << endl;
# 62 "fcalib.ag"

# 63 "fcalib.ag"
  NormalizeDistortion(x0, y0, d2, d4, dimx, dimy);
# 64 "fcalib.ag"

# 65 "fcalib.ag"
  cout << "Verzeichnung normalisiert " ;
# 66 "fcalib.ag"
  cout << "x0: " << x0 << "  y0: " << y0;
# 67 "fcalib.ag"
  cout << "  d2: " << d2 << "  d4: " << d4 << endl << endl;
# 68 "fcalib.ag"
}
# 69 "fcalib.ag"

# 70 "fcalib.ag"
void Print(const Matrix& m, int dimx, int dimy)
# 71 "fcalib.ag"
{
# 72 "fcalib.ag"
  cout << "Bildgröße:     " << dimx << " x " << dimy << endl;
# 73 "fcalib.ag"
  double f = m[0][0];
# 74 "fcalib.ag"
  cout << "Brennweite  f: " << f << endl;
# 75 "fcalib.ag"
  cout << "Anisotropie a: " << m[1][1] / m[0][0] << endl;
# 76 "fcalib.ag"
  cout << "Scherung    s: " << m[0][1] / m[0][0] << endl;
# 77 "fcalib.ag"
  cout << "Hauptpunkt u0: " << m[0][2] << endl;
# 78 "fcalib.ag"
  cout << "           v0: " << m[1][2] << endl << endl;
# 79 "fcalib.ag"

# 80 "fcalib.ag"
  cout << "Brennweite (KB äquivalent): " << f * 36 / dimx << " mm" <<  endl;
# 81 "fcalib.ag"
  cout << "Bildwinkel:" << endl;
# 82 "fcalib.ag"
  cout << Degree(atan2(dimx / 2, f) * 2) << "° (x)" << endl;
# 83 "fcalib.ag"
  cout << Degree(atan2(dimy / 2, f) * 2) << "° (y)" << endl;
# 84 "fcalib.ag"
  cout << Degree(atan2(sqrt(dimx * dimx + dimy * dimy) / 2, f) * 2) << "° (diag)" << endl;
# 85 "fcalib.ag"
}
# 86 "fcalib.ag"

# 87 "fcalib.ag"
/*AppGen
# 88 "fcalib.ag"
 prog: calibration with 2 d pattern
# 89 "fcalib.ag"
 %% Beschreibung Parameter
# 90 "fcalib.ag"
 % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 91 "fcalib.ag"
 para: file, required, filelist, imgfile, source file[s],
# 92 "fcalib.ag"
 %
# 93 "fcalib.ag"
 opt: s, smear, void, smearImage, preprocessing with box filter (3 x 3), false
# 94 "fcalib.ag"
 opt: S, smearsize, int, smearSize, preprocessing with box filter (n x n), 0
# 95 "fcalib.ag"
 opt2: if size == 0  and smear==false no box filter
# 96 "fcalib.ag"
 opt: l, seglevel, int, segmentationLevel, Segmentation level, 55
# 97 "fcalib.ag"
 opt: L, segsize, int, segmentationSize, Segmentation size, 15
# 98 "fcalib.ag"

# 99 "fcalib.ag"
 opt: m, neededmarkers, int, markersNeeded, minimal number of markers, 400
# 100 "fcalib.ag"

# 101 "fcalib.ag"
 opt: d, display, void, display, display images while processing, false
# 102 "fcalib.ag"
 opt: D, displayselected, int, displaySelected, display selected images, 0
# 103 "fcalib.ag"
 opt2: Bitmaske:
# 104 "fcalib.ag"
 opt2:   1 - original
# 105 "fcalib.ag"
 opt2:   2 - segmented
# 106 "fcalib.ag"
 opt2:   4 - detected polygon and markers
# 107 "fcalib.ag"
 opt: o, output, string, outputSelection, data to output, ""
# 108 "fcalib.ag"
 opt2:   c - intrinsic camera parameters
# 109 "fcalib.ag"
 opt2:   d - distortion parameters
# 110 "fcalib.ag"
 opt2:   D - distortion parameters to database
# 111 "fcalib.ag"
 opt: p, profiles, string, profileFile, file to store profile, "dist.txt"
# 112 "fcalib.ag"
 opt: v, verbose, void, verbose, be verbose, false
# 113 "fcalib.ag"
 opt: V, verboseselected, int, verboseSelected, be verbose in special stage, 0
# 114 "fcalib.ag"
 opt: h, help, usage, help, help
# 115 "fcalib.ag"
AppGen*/
# 116 "fcalib.ag"

# 117 "fcalib.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - calibration with 2 d pattern" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] file " << endl;
  cout << "  file - source file[s]" << endl;
  cout << "Options:" << endl;
  cout << "-s    " << "  " << "--smear     " << "   ";
  cout << "preprocessing with box filter (3 x 3) (default: false)" << endl ;
  cout << "-S <i>" << "  " << "--smearsize=<i>" << "   ";
  cout << endl << "                       ";
  cout << "preprocessing with box filter (n x n) (default: 0)" << endl ;
  cout << "                       " << "if size == 0  and smear==false no box filter" << endl;
  cout << "-l <i>" << "  " << "--seglevel=<i>" << "   ";
  cout << endl << "                       ";
  cout << "Segmentation level (default: 55)" << endl ;
  cout << "-L <i>" << "  " << "--segsize=<i>" << "   ";
  cout << endl << "                       ";
  cout << "Segmentation size (default: 15)" << endl ;
  cout << "-m <i>" << "  " << "--neededmarkers=<i>" << "   ";
  cout << endl << "                       ";
  cout << "minimal number of markers (default: 400)" << endl ;
  cout << "-d    " << "  " << "--display   " << "   ";
  cout << "display images while processing (default: false)" << endl ;
  cout << "-D <i>" << "  " << "--displayselected=<i>" << "   ";
  cout << endl << "                       ";
  cout << "display selected images (default: 0)" << endl ;
  cout << "                       " << "Bitmaske:" << endl;
  cout << "                       " << "1 - original" << endl;
  cout << "                       " << "2 - segmented" << endl;
  cout << "                       " << "4 - detected polygon and markers" << endl;
  cout << "-o <s>" << "  " << "--output=<s>" << "   ";
  cout << "data to output (default: \"\")" << endl ;
  cout << "                       " << "c - intrinsic camera parameters" << endl;
  cout << "                       " << "d - distortion parameters" << endl;
  cout << "                       " << "D - distortion parameters to database" << endl;
  cout << "-p <s>" << "  " << "--profiles=<s>" << "   ";
  cout << endl << "                       ";
  cout << "file to store profile (default: \"dist.txt\")" << endl ;
  cout << "-v    " << "  " << "--verbose   " << "   ";
  cout << "be verbose (default: false)" << endl ;
  cout << "-V <i>" << "  " << "--verboseselected=<i>" << "   ";
  cout << endl << "                       ";
  cout << "be verbose in special stage (default: 0)" << endl ;
  cout << "-h    " << "  " << "--help      " << "   ";
  cout << "help" << endl ;
  exit(1);
}

void error(const string& msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

long int ptol(const char* para)
{
  char* end;
  long int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in integer value: ") + para);
  return res;
}

int main(int argc, char** argv)
{
  bool smearImage = false;
  int smearSize = 0;
  int segmentationLevel = 55;
  int segmentationSize = 15;
  int markersNeeded = 400;
  bool display = false;
  int displaySelected = 0;
  string outputSelection = "";
  string profileFile = "dist.txt";
  bool verbose = false;
  int verboseSelected = 0;

  std::vector<std::string> imgfile;
  static struct option ag_long_options[] =
  {
    {"smear", no_argument, 0, 's' },
    {"smearsize", required_argument, 0, 'S' },
    {"seglevel", required_argument, 0, 'l' },
    {"segsize", required_argument, 0, 'L' },
    {"neededmarkers", required_argument, 0, 'm' },
    {"display", no_argument, 0, 'd' },
    {"displayselected", required_argument, 0, 'D' },
    {"output", required_argument, 0, 'o' },
    {"profiles", required_argument, 0, 'p' },
    {"verbose", no_argument, 0, 'v' },
    {"verboseselected", required_argument, 0, 'V' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":sS:l:L:m:dD:o:p:vV:h", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 's':
          smearImage = true;
          break;

        case 'S':
          smearSize = ptol(optarg);
          break;

        case 'l':
          segmentationLevel = ptol(optarg);
          break;

        case 'L':
          segmentationSize = ptol(optarg);
          break;

        case 'm':
          markersNeeded = ptol(optarg);
          break;

        case 'd':
          display = true;
          break;

        case 'D':
          displaySelected = ptol(optarg);
          break;

        case 'o':
          outputSelection = optarg;
          break;

        case 'p':
          profileFile = optarg;
          break;

        case 'v':
          verbose = true;
          break;

        case 'V':
          verboseSelected = ptol(optarg);
          break;

        case 'h':
          usage();
          break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    FileList(argc, argv, optind, imgfile, false);
  else error("Parameter file needed");

  /*AppGen:MainEnd*/
# 121 "fcalib.ag"
  Image img;
# 122 "fcalib.ag"
  Image mrk;
# 123 "fcalib.ag"
  Image segmented_img;
# 124 "fcalib.ag"

# 125 "fcalib.ag"
  try
# 126 "fcalib.ag"
    {
# 127 "fcalib.ag"
      // handle combined settings in verbose + verboseSelected
# 128 "fcalib.ag"
      if (verboseSelected > 0)
# 129 "fcalib.ag"
        {
# 130 "fcalib.ag"
          verbose = true;
# 131 "fcalib.ag"
          verboseSwitch = verboseSelected;
# 132 "fcalib.ag"
        }
# 133 "fcalib.ag"
      else if (verbose)
# 134 "fcalib.ag"
        {
# 135 "fcalib.ag"
          verboseSwitch = verboseSelected = v_all;
# 136 "fcalib.ag"
        }
# 137 "fcalib.ag"

# 138 "fcalib.ag"
      if (displaySelected > 0)
# 139 "fcalib.ag"
        {
# 140 "fcalib.ag"
          display = true;
# 141 "fcalib.ag"
          displaySwitch = displaySelected;
# 142 "fcalib.ag"
        }
# 143 "fcalib.ag"
      else if (display)
# 144 "fcalib.ag"
        {
# 145 "fcalib.ag"
          displaySwitch = displaySelected = 7;
# 146 "fcalib.ag"
        }
# 147 "fcalib.ag"

# 148 "fcalib.ag"
      int outputSelected = 0;
# 149 "fcalib.ag"
      if (outputSelection.empty())
# 150 "fcalib.ag"
        outputSelected = 3;
# 151 "fcalib.ag"
      else
# 152 "fcalib.ag"
        {
# 153 "fcalib.ag"
          for (unsigned int i = 0; i < outputSelection.size(); i++)
# 154 "fcalib.ag"
            {
# 155 "fcalib.ag"
              char c = outputSelection[i];
# 156 "fcalib.ag"
              switch (c)
# 157 "fcalib.ag"
                {
# 158 "fcalib.ag"
                case 'c':
                  outputSelected |= output_intrinsic;
                  break;
# 159 "fcalib.ag"
                case 'd':
                  outputSelected |= output_distortion;
                  break;
# 160 "fcalib.ag"
                case 'D':
                  outputSelected |= output_distdb;
                  break;
# 161 "fcalib.ag"
                }
# 162 "fcalib.ag"
            }
# 163 "fcalib.ag"
        }
# 164 "fcalib.ag"

# 165 "fcalib.ag"
      vector<vector<Point> > pointList;
# 166 "fcalib.ag"
      vector<vector<Point> > referenceList;
# 167 "fcalib.ag"
      vector<Trafo> trafoList;
# 168 "fcalib.ag"
      vector<Distortion1> distList;
# 169 "fcalib.ag"

# 170 "fcalib.ag"
      int images = 0;
# 171 "fcalib.ag"
      int output = 0;
# 172 "fcalib.ag"

# 173 "fcalib.ag"
      string exif; // EXIF-basierte Kennung
# 174 "fcalib.ag"
      string suffix; // Modifier für EXIF-Kennung
# 175 "fcalib.ag"

# 176 "fcalib.ag"
      double focallength = 0.0;
# 177 "fcalib.ag"

# 178 "fcalib.ag"
      if (smearImage && smearSize == 0)
# 179 "fcalib.ag"
        smearSize = 3;
# 180 "fcalib.ag"

# 181 "fcalib.ag"
      if (output == 0)
# 182 "fcalib.ag"
        output = 3;
# 183 "fcalib.ag"

# 184 "fcalib.ag"
      vector<string> file;
# 185 "fcalib.ag"
      FileList(argc, argv, optind, file, false);
# 186 "fcalib.ag"

# 187 "fcalib.ag"
      int dimx = -1;
# 188 "fcalib.ag"
      int dimy = -1;
# 189 "fcalib.ag"

# 190 "fcalib.ag"

# 191 "fcalib.ag"
      images = 0;
# 192 "fcalib.ag"
      while (!file.empty())
# 193 "fcalib.ag"
        {
# 194 "fcalib.ag"
          string fn = file.back();
# 195 "fcalib.ag"
          file.pop_back();
# 196 "fcalib.ag"

# 197 "fcalib.ag"
          if (verboseSwitch > 0)
# 198 "fcalib.ag"
            cout << "Reading " << fn << endl;
# 199 "fcalib.ag"

# 200 "fcalib.ag"
          int sx, sy, mv, ch;
# 201 "fcalib.ag"

# 202 "fcalib.ag"
          InfImgFile(fn, sx, sy, mv, ch);
# 203 "fcalib.ag"

# 204 "fcalib.ag"
          string fileexif;
# 205 "fcalib.ag"
          double filefocallength;
# 206 "fcalib.ag"
          if (exifmode == exif_file)
# 207 "fcalib.ag"
            {
# 208 "fcalib.ag"
              if (!getExifSig(fn, suffix, fileexif, filefocallength))
# 209 "fcalib.ag"
                {
# 210 "fcalib.ag"
                  throw "EXIF data not available";
# 211 "fcalib.ag"
                }
# 212 "fcalib.ag"

# 213 "fcalib.ag"
              if (exif.empty()) // erste Datei
# 214 "fcalib.ag"
                {
# 215 "fcalib.ag"
                  exif = fileexif;
# 216 "fcalib.ag"
                  focallength = filefocallength;
# 217 "fcalib.ag"
                }
# 218 "fcalib.ag"

# 219 "fcalib.ag"
              if (fileexif != exif || filefocallength != focallength)
# 220 "fcalib.ag"
                {
# 221 "fcalib.ag"
                  throw "exif data differ";
# 222 "fcalib.ag"
                }
# 223 "fcalib.ag"
            }
# 224 "fcalib.ag"

# 225 "fcalib.ag"
          if (dimx < 0)
# 226 "fcalib.ag"
            {
# 227 "fcalib.ag"
              // erstes Bild
# 228 "fcalib.ag"
              dimx = sx;
# 229 "fcalib.ag"
              dimy = sy;
# 230 "fcalib.ag"

# 231 "fcalib.ag"
              img = NewImg(dimx, dimy, 255);
# 232 "fcalib.ag"
              mrk = NewImg(dimx, dimy, 7);
# 233 "fcalib.ag"
              segmented_img = NewImg(dimx, dimy, 4);
# 234 "fcalib.ag"
              if (displaySwitch & 1)
# 235 "fcalib.ag"
                {
# 236 "fcalib.ag"
                  Show(OVERLAY, img, mrk);
# 237 "fcalib.ag"
                }
# 238 "fcalib.ag"
              if (displaySwitch & 2)
# 239 "fcalib.ag"
                {
# 240 "fcalib.ag"
                  Show(OVERLAY, img, segmented_img);
# 241 "fcalib.ag"
                }
# 242 "fcalib.ag"

# 243 "fcalib.ag"
            }
# 244 "fcalib.ag"

# 245 "fcalib.ag"
          if (dimx != sx || dimy != sy)
# 246 "fcalib.ag"
            throw "image sizes differ";
# 247 "fcalib.ag"

# 248 "fcalib.ag"
          // Load image
# 249 "fcalib.ag"
          ReadImg(fn, img);
# 250 "fcalib.ag"
          if (smearSize > 0)
# 251 "fcalib.ag"
            smearImg(img, img, smearSize);
# 252 "fcalib.ag"
          // Segmentation
# 253 "fcalib.ag"
          LocalSeg(img, segmented_img, segmentationSize, segmentationLevel);
# 254 "fcalib.ag"

# 255 "fcalib.ag"
          // new pointlists, homography and distortion
# 256 "fcalib.ag"
          pointList.push_back(vector<Point>());
# 257 "fcalib.ag"
          referenceList.push_back(vector<Point>());
# 258 "fcalib.ag"
          trafoList.push_back(Trafo());
# 259 "fcalib.ag"
          distList.push_back(Distortion1());
# 260 "fcalib.ag"

# 261 "fcalib.ag"
          if (getReferencePoints(img, mrk, segmented_img,
# 262 "fcalib.ag"
                                 pointList[images], referenceList[images],
# 263 "fcalib.ag"
                                 trafoList[images], distList[images]) &&
# 264 "fcalib.ag"
              (int)referenceList[images].size() >= markersNeeded)
# 265 "fcalib.ag"
            {
# 266 "fcalib.ag"
#if 0
# 267 "fcalib.ag"
              Image rest = NewImg(img);
# 268 "fcalib.ag"
              Show(ON, rest);
# 269 "fcalib.ag"
              distList[images].RectImg(img, rest, INTERPOL);
# 270 "fcalib.ag"
              GetChar();
# 271 "fcalib.ag"
#endif
# 272 "fcalib.ag"

# 273 "fcalib.ag"
              images++;
# 274 "fcalib.ag"
              if (verboseSwitch > 0)
# 275 "fcalib.ag"
                cout << "OK" << endl;
# 276 "fcalib.ag"
            }
# 277 "fcalib.ag"
          else
# 278 "fcalib.ag"
            {
# 279 "fcalib.ag"
              pointList.pop_back();// Liste verwerfen
# 280 "fcalib.ag"
              referenceList.pop_back();// Liste verwerfen
# 281 "fcalib.ag"
              trafoList.pop_back();// Liste verwerfen
# 282 "fcalib.ag"
              distList.pop_back();// Liste verwerfen
# 283 "fcalib.ag"
              if (!(verboseSwitch & v_step))
# 284 "fcalib.ag"
                cout << fn << " nicht nutzbar!" << endl;
# 285 "fcalib.ag"

# 286 "fcalib.ag"
            }
# 287 "fcalib.ag"
        }
# 288 "fcalib.ag"
      optind++;
# 289 "fcalib.ag"

# 290 "fcalib.ag"
      Distortion1 distortion;
# 291 "fcalib.ag"
      Matrix m(3, 3);
# 292 "fcalib.ag"

# 293 "fcalib.ag"
      if (images > 0)
# 294 "fcalib.ag"
        {
# 295 "fcalib.ag"
          // Berechnungen
# 296 "fcalib.ag"

# 297 "fcalib.ag"
          if ((outputSelected & output_distortion) || (outputSelected & output_distdb))
# 298 "fcalib.ag"
            {
# 299 "fcalib.ag"
              // cout << pointList.size() << " " << referenceList.size() << endl;
# 300 "fcalib.ag"
              // for (int i=0;i<pointList.size();i++)
# 301 "fcalib.ag"
              //   cout << pointList[i].size() << " " << referenceList[i].size() << endl;
# 302 "fcalib.ag"
              distortion.Calc(pointList, referenceList);
# 303 "fcalib.ag"
            }
# 304 "fcalib.ag"

# 305 "fcalib.ag"
          if (outputSelected & output_intrinsic)
# 306 "fcalib.ag"
            {
# 307 "fcalib.ag"
              if (images < 3)
# 308 "fcalib.ag"
                throw "not enough images to calibrate intrinsic parameters";
# 309 "fcalib.ag"
              else
# 310 "fcalib.ag"
                m = calib_intrinsic_zhang(trafoList);
# 311 "fcalib.ag"
            }
# 312 "fcalib.ag"

# 313 "fcalib.ag"
          // Ausgaben
# 314 "fcalib.ag"
          if (verboseSwitch > 0)
# 315 "fcalib.ag"
            cout << "--------------------------" << endl;
# 316 "fcalib.ag"

# 317 "fcalib.ag"
          if (outputSelected & output_intrinsic)
# 318 "fcalib.ag"
            {
# 319 "fcalib.ag"
              Print(m, dimx, dimy);
# 320 "fcalib.ag"
            }
# 321 "fcalib.ag"

# 322 "fcalib.ag"
          if (outputSelected & output_distortion)
# 323 "fcalib.ag"
            {
# 324 "fcalib.ag"
              Print(distortion, dimx, dimy);
# 325 "fcalib.ag"
            }
# 326 "fcalib.ag"

# 327 "fcalib.ag"
          if (outputSelected & output_distdb)
# 328 "fcalib.ag"
            {
# 329 "fcalib.ag"
              if (exifmode == exif_ignore)
# 330 "fcalib.ag"
                throw "no exif data - no entry in database";
# 331 "fcalib.ag"

# 332 "fcalib.ag"
              double x0 = distortion.X0();
# 333 "fcalib.ag"
              double y0 = distortion.Y0();
# 334 "fcalib.ag"
              double d2 = distortion.D2();
# 335 "fcalib.ag"
              double d4 = distortion.D4();
# 336 "fcalib.ag"

# 337 "fcalib.ag"
              NormalizeDistortion(x0, y0, d2, d4, dimx, dimy);
# 338 "fcalib.ag"

# 339 "fcalib.ag"
              string signature = exif;
# 340 "fcalib.ag"

# 341 "fcalib.ag"
              ofstream ddb(profileFile, ios::app);
# 342 "fcalib.ag"
              ddb.precision(12);
# 343 "fcalib.ag"
              ddb << signature << " "  << focallength << " ";
# 344 "fcalib.ag"
              ddb << x0 << " " << y0 << " ";
# 345 "fcalib.ag"
              ddb << d2 << " " << d4 << endl;
# 346 "fcalib.ag"
            }
# 347 "fcalib.ag"
        }
# 348 "fcalib.ag"
      if (displaySwitch > 0)
# 349 "fcalib.ag"
        {
# 350 "fcalib.ag"
          cout << "input enter to continue" << endl;
# 351 "fcalib.ag"
          getchar();
# 352 "fcalib.ag"
        }
# 353 "fcalib.ag"
      return OK;
# 354 "fcalib.ag"
    }
# 355 "fcalib.ag"
  catch (const char* msg)
# 356 "fcalib.ag"
    {
# 357 "fcalib.ag"
      cerr << "Exeption: " << msg << endl;
# 358 "fcalib.ag"
    }
# 359 "fcalib.ag"
  if (displaySwitch > 0)
# 360 "fcalib.ag"
    {
# 361 "fcalib.ag"
      cout << "input enter to continue" << endl;
# 362 "fcalib.ag"
      getchar();
# 363 "fcalib.ag"
    }
# 364 "fcalib.ag"
  return 1;
# 365 "fcalib.ag"
}

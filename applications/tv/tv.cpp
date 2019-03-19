# 1 "tv.ag"
#ifndef WIN32
# 2 "tv.ag"
#include <sys/time.h>
# 3 "tv.ag"
#include <unistd.h>
# 4 "tv.ag"
#else
# 5 "tv.ag"
#include <time.h>
# 6 "tv.ag"
#include <sys/timeb.h>
# 7 "tv.ag"
#endif
# 8 "tv.ag"

# 9 "tv.ag"
#include <iostream>
# 10 "tv.ag"
#include <stdio.h>
# 11 "tv.ag"
#include <stdlib.h>
# 12 "tv.ag"
#include <string.h>
# 13 "tv.ag"
#include <string>
# 14 "tv.ag"
#include <iomanip>
# 15 "tv.ag"
#include <math.h>
# 16 "tv.ag"
#ifndef WIN32
# 17 "tv.ag"
#include <getopt.h>
# 18 "tv.ag"
#else
# 19 "tv.ag"
#include "getopt.h"
# 20 "tv.ag"
#endif
# 21 "tv.ag"
#include <GL/glut.h>
# 22 "tv.ag"

# 23 "tv.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
#include <iomanip>
#include <string>
#include <string>
int window_width = 1280;
int window_height = 1024;
int cammode = 2;
double cameraDistance = 30;
bool verbose = false;
bool benchmarking = false;
int cdebug = 0;
/*AppGen:GlobalEnd*/
# 24 "tv.ag"

# 25 "tv.ag"
using namespace std;
# 26 "tv.ag"

# 27 "tv.ag"
#include "tv.h"
# 28 "tv.ag"
#include "trafo.h"
# 29 "tv.ag"
#include "Triangle.h"
# 30 "tv.ag"
#include "numbase.h"
# 31 "tv.ag"

# 32 "tv.ag"
vector<Triangle> world;
# 33 "tv.ag"

# 34 "tv.ag"
float VirtualTime = 0.0;
# 35 "tv.ag"
float RealTime = 0.0;
# 36 "tv.ag"
float frametime = 0.05;
# 37 "tv.ag"

# 38 "tv.ag"
float rspeed = 40;
# 39 "tv.ag"

# 40 "tv.ag"
float move_step = 10.0;
# 41 "tv.ag"
float angle_step = 2.0;
# 42 "tv.ag"

# 43 "tv.ag"
enum tm_stat {tm_halt, tm_slow, tm_normal, tm_fast};
# 44 "tv.ag"
enum tm_stat timestat = tm_normal;
# 45 "tv.ag"

# 46 "tv.ag"
// Kamera-Parameter (Mode 1,3)
# 47 "tv.ag"
float camdistance;
# 48 "tv.ag"
float camangle = 40;
# 49 "tv.ag"
float leftrightrotate;
# 50 "tv.ag"
float updownrotate;
# 51 "tv.ag"

# 52 "tv.ag"
int visible = false;
# 53 "tv.ag"

# 54 "tv.ag"
float window_ratio;
# 55 "tv.ag"

# 56 "tv.ag"
int light_number;
# 57 "tv.ag"

# 58 "tv.ag"
GLfloat globalambient[] = {0.3, 0.3, 0.3, 1.0};
# 59 "tv.ag"

# 60 "tv.ag"
/* The specifications for 2 light sources */
# 61 "tv.ag"
GLfloat pos0[] = {1.0, 1.0, 1.0, 0.0}; /* w = 0 == infinite distance */
# 62 "tv.ag"
GLfloat dif0[] = {0.4, 0.4, 0.4, 1.0}; // diffuse and specular
# 63 "tv.ag"

# 64 "tv.ag"
GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0}; // ambient light for all light sources
# 65 "tv.ag"

# 66 "tv.ag"
GLfloat pos1[] = { -1.0, .0, 5.0, 0.0}; /* w = 0 == infinite distance */
# 67 "tv.ag"
GLfloat dif1[] = {0.4, 0.4, 0.4, 1.0}; // diffuse and specular
# 68 "tv.ag"

# 69 "tv.ag"
GLfloat null[] = { 0.0, 0.0, 0.0, 1.0};
# 70 "tv.ag"
GLfloat eins[] = { 1.0, 1.0, 1.0, 1.0};
# 71 "tv.ag"

# 72 "tv.ag"
/*AppGen
# 73 "tv.ag"
%%  Beschreibung des Programmes:
# 74 "tv.ag"
  prog: Programm zur Darstellung von 3D-Welten
# 75 "tv.ag"

# 76 "tv.ag"
  %% Beschreibung Parameter
# 77 "tv.ag"
  % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 78 "tv.ag"
  para: welt, optional, string, fname, Weltbeschreibung, welt.cv
# 79 "tv.ag"

# 80 "tv.ag"
  %% Beschreibung der Optionen
# 81 "tv.ag"
  % kurz-Option, lang-Option, Typ, Variablenname, Erklärung, Default-Wert
# 82 "tv.ag"

# 83 "tv.ag"
  opt: f, worldfile, string, fname_opt, Dateiname Weltbeschreibung, ""
# 84 "tv.ag"
  opt2:   Dies ist eine alternative Form, die auch die Angabe von
# 85 "tv.ag"
  opt2:   -f - erlaubt, was die Standard-Eingabe als Quelle festlegt
# 86 "tv.ag"

# 87 "tv.ag"
  opt: X, xrotation, void, xrotation, Rotation um 180° um X-Achse, false
# 88 "tv.ag"
  opt: Y, yrotation, void, yrotation, Rotation um 180° um Y-Achse, false
# 89 "tv.ag"
  opt: Z, zrotation, void, zrotation, Rotation um 180° um Z-Achse, false
# 90 "tv.ag"

# 91 "tv.ag"
  opt: C, center, void, centerModel, Modellmittelpunkt in Ursprung schieben, false
# 92 "tv.ag"
  opt: x, xsize,  Int,     window_width,    Bildgröße X,  1280
# 93 "tv.ag"
  opt: y, ysize,  Int,     window_height,    Bildgröße Y,  1024
# 94 "tv.ag"

# 95 "tv.ag"
  opt: c, camera, Int, cammode, Kamera-Modus,2
# 96 "tv.ag"
  opt2:   1 - Kamera sieht auf den Ursprung
# 97 "tv.ag"
  opt2:   2 - Kamera frei beweglich im Raum
# 98 "tv.ag"
  opt2:   3 - Kamera rotiert um den Ursprung
# 99 "tv.ag"

# 100 "tv.ag"
  opt: D, cameradistance, Double, cameraDistance, Abstand der Kamera, 30
# 101 "tv.ag"
  opt: a, anglestep, double, angledelta, Winkel-Schrittweite, 2
# 102 "tv.ag"

# 103 "tv.ag"
  opt: l, lights, int, std_lights, Zahl der Standard-Lichtquellen, 2
# 104 "tv.ag"

# 105 "tv.ag"
  opt: v, verbose,  Void,   verbose,  Ausführliche Meldungen,  false
# 106 "tv.ag"
  opt: b, benchmark, Void, benchmarking, Framerate anzeigen, false
# 107 "tv.ag"
  opt: d, debug, Int, cdebug, Debug-Modus, 0
# 108 "tv.ag"
  opt: h, help,   usage,   ,    Hilfe
# 109 "tv.ag"

# 110 "tv.ag"
%% "l:st:"
# 111 "tv.ag"
  AppGen*/
# 112 "tv.ag"

# 113 "tv.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - Programm zur Darstellung von 3D-Welten" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] [welt] " << endl;
  cout << "  welt - Weltbeschreibung (optional, default:\"welt.cv\")" << endl;
  cout << "Options:" << endl;
  cout << "-f <s>" << "  " << "--worldfile=<s>" << "   ";
cout << endl << "                       "; 
  cout << "Dateiname Weltbeschreibung (default: \"\")" << endl ; 
  cout << "                       " << "Dies ist eine alternative Form, die auch die Angabe von" << endl; 
  cout << "                       " << "-f - erlaubt, was die Standard-Eingabe als Quelle festlegt" << endl; 
  cout << "-X    " << "  " << "--xrotation " << "   ";
  cout << "Rotation um 180° um X-Achse (default: false)" << endl ; 
  cout << "-Y    " << "  " << "--yrotation " << "   ";
  cout << "Rotation um 180° um Y-Achse (default: false)" << endl ; 
  cout << "-Z    " << "  " << "--zrotation " << "   ";
  cout << "Rotation um 180° um Z-Achse (default: false)" << endl ; 
  cout << "-C    " << "  " << "--center    " << "   ";
  cout << "Modellmittelpunkt in Ursprung schieben (default: false)" << endl ; 
  cout << "-x <i>" << "  " << "--xsize=<i> " << "   ";
  cout << "Bildgröße X (default: 1280)" << endl ; 
  cout << "-y <i>" << "  " << "--ysize=<i> " << "   ";
  cout << "Bildgröße Y (default: 1024)" << endl ; 
  cout << "-c <i>" << "  " << "--camera=<i>" << "   ";
  cout << "Kamera-Modus (default: 2)" << endl ; 
  cout << "                       " << "1 - Kamera sieht auf den Ursprung" << endl; 
  cout << "                       " << "2 - Kamera frei beweglich im Raum" << endl; 
  cout << "                       " << "3 - Kamera rotiert um den Ursprung" << endl; 
  cout << "-D <d>" << "  " << "--cameradistance=<d>" << "   ";
cout << endl << "                       "; 
  cout << "Abstand der Kamera (default: 30)" << endl ; 
  cout << "-a <d>" << "  " << "--anglestep=<d>" << "   ";
cout << endl << "                       "; 
  cout << "Winkel-Schrittweite (default: 2)" << endl ; 
  cout << "-l <i>" << "  " << "--lights=<i>" << "   ";
  cout << "Zahl der Standard-Lichtquellen (default: 2)" << endl ; 
  cout << "-v    " << "  " << "--verbose   " << "   ";
  cout << "Ausführliche Meldungen (default: false)" << endl ; 
  cout << "-b    " << "  " << "--benchmark " << "   ";
  cout << "Framerate anzeigen (default: false)" << endl ; 
  cout << "-d <i>" << "  " << "--debug=<i> " << "   ";
  cout << "Debug-Modus (default: 0)" << endl ; 
  cout << "-h    " << "  " << "--help      " << "   ";
  cout << "Hilfe" << endl ; 
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
  string fname_opt = "";
  bool xrotation = false;
  bool yrotation = false;
  bool zrotation = false;
  bool centerModel = false;
  double angledelta = 2;
  int std_lights = 2;
  
  string fname = "welt.cv";
  static struct option ag_long_options[] =
  {
    {"worldfile", required_argument, 0, 'f' },
    {"xrotation", no_argument, 0, 'X' },
    {"yrotation", no_argument, 0, 'Y' },
    {"zrotation", no_argument, 0, 'Z' },
    {"center", no_argument, 0, 'C' },
    {"xsize", required_argument, 0, 'x' },
    {"ysize", required_argument, 0, 'y' },
    {"camera", required_argument, 0, 'c' },
    {"cameradistance", required_argument, 0, 'D' },
    {"anglestep", required_argument, 0, 'a' },
    {"lights", required_argument, 0, 'l' },
    {"verbose", no_argument, 0, 'v' },
    {"benchmark", no_argument, 0, 'b' },
    {"debug", required_argument, 0, 'd' },
    {"help", no_argument, 0, 'h' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":f:XYZCx:y:c:D:a:l:vbd:h", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'f':
          fname_opt = optarg;
          break;

        case 'X':
          xrotation = true;
          break;

        case 'Y':
          yrotation = true;
          break;

        case 'Z':
          zrotation = true;
          break;

        case 'C':
          centerModel = true;
          break;

        case 'x':
          window_width = ptol(optarg);
          break;

        case 'y':
          window_height = ptol(optarg);
          break;

        case 'c':
          cammode = ptol(optarg);
          break;

        case 'D':
          cameraDistance = ptod(optarg);
          break;

        case 'a':
          angledelta = ptod(optarg);
          break;

        case 'l':
          std_lights = ptol(optarg);
          break;

        case 'v':
          verbose = true;
          break;

        case 'b':
          benchmarking = true;
          break;

        case 'd':
          cdebug = ptol(optarg);
          break;

        case 'h':
          usage();
          break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    fname = argv[optind++];

/*AppGen:MainEnd*/
# 117 "tv.ag"

# 118 "tv.ag"
  try
# 119 "tv.ag"
    {
# 120 "tv.ag"
      if (fname == "welt.cv") // default
# 121 "tv.ag"
        if (!fname_opt.empty()) // filename given as option
# 122 "tv.ag"
          fname = fname_opt;
# 123 "tv.ag"

# 124 "tv.ag"
      angle_step = angledelta;
# 125 "tv.ag"

# 126 "tv.ag"
      int dummy_argc = 1;
# 127 "tv.ag"
      char arg0[] = "tv";
# 128 "tv.ag"
      char* dummy_argv[] = {arg0, NULL};
# 129 "tv.ag"

# 130 "tv.ag"
      glutInit(&dummy_argc, dummy_argv);
# 131 "tv.ag"

# 132 "tv.ag"
      int mainmenu, anglemenu, speedmenu;
# 133 "tv.ag"

# 134 "tv.ag"
      // ein oder zwei Standard-lichtquellen
# 135 "tv.ag"
      // Szene-Beschreibung kann dies überschreiben
# 136 "tv.ag"
      if (std_lights > 2) std_lights = 2;
# 137 "tv.ag"
      if (std_lights < 1) std_lights = 1;
# 138 "tv.ag"

# 139 "tv.ag"
      // verhältnis seitenlängen der Fenster
# 140 "tv.ag"
      // (Pixel werden quadratisch angenommen)
# 141 "tv.ag"
      window_ratio = ((float)window_width) / ((float)window_height);
# 142 "tv.ag"

# 143 "tv.ag"
      int readFlags = 0;
# 144 "tv.ag"
      bool xs = false;
# 145 "tv.ag"
      bool ys = false;
# 146 "tv.ag"
      bool zs = false;
# 147 "tv.ag"
      if (xrotation)
# 148 "tv.ag"
        {
# 149 "tv.ag"
          ys = !ys;
# 150 "tv.ag"
          zs = !zs;
# 151 "tv.ag"
        }
# 152 "tv.ag"
      if (yrotation)
# 153 "tv.ag"
        {
# 154 "tv.ag"
          xs = !xs;
# 155 "tv.ag"
          zs = !zs;
# 156 "tv.ag"
        }
# 157 "tv.ag"
      if (zrotation)
# 158 "tv.ag"
        {
# 159 "tv.ag"
          xs = !xs;
# 160 "tv.ag"
          ys = !ys;
# 161 "tv.ag"
        }
# 162 "tv.ag"

# 163 "tv.ag"
      readFlags = (xs ? 1 : 0) | (ys ? 2 : 0) | (zs ? 4 : 0) | (centerModel ? 8 : 0);
# 164 "tv.ag"

# 165 "tv.ag"
      // Lesen des Szene-Files, Aufbau der Welt
# 166 "tv.ag"
      ReadWorld(world, fname, readFlags);
# 167 "tv.ag"

# 168 "tv.ag"
      if (light_number == 0) // weltbeschreibung enthält keine Lichtquellen
# 169 "tv.ag"
        {
# 170 "tv.ag"
          // Standard-Lichtquellen aktivieren
# 171 "tv.ag"
          light_number = std_lights;
# 172 "tv.ag"
        }
# 173 "tv.ag"

# 174 "tv.ag"
      /* Set things up and go */
# 175 "tv.ag"
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
# 176 "tv.ag"
      glutInitWindowSize(window_width, window_height);
# 177 "tv.ag"
      glutCreateWindow("TriangleViewer");
# 178 "tv.ag"
      // call back funktionen setzen
# 179 "tv.ag"
      glutDisplayFunc(Display);
# 180 "tv.ag"
      glutIdleFunc(HandleIdle);
# 181 "tv.ag"
      glutVisibilityFunc(HandleVisibility);
# 182 "tv.ag"
      glutKeyboardFunc(HandleKeyboard);
# 183 "tv.ag"
      glutSpecialFunc(HandleSpecialKeyboard);
# 184 "tv.ag"
      glutMouseFunc(HandleMouse);
# 185 "tv.ag"
      glutReshapeFunc(HandleReshape);
# 186 "tv.ag"

# 187 "tv.ag"
      //  glutMotionFunc(HandleMouseMotion);
# 188 "tv.ag"

# 189 "tv.ag"
      /* Set up some menus */
# 190 "tv.ag"

# 191 "tv.ag"
      anglemenu = glutCreateMenu(HandleAngleMenu);
# 192 "tv.ag"
      glutAddMenuEntry("Weit (w)", 1);
# 193 "tv.ag"
      glutAddMenuEntry("Normal", 2);
# 194 "tv.ag"
      glutAddMenuEntry("Tele (t)", 3);
# 195 "tv.ag"

# 196 "tv.ag"
      speedmenu = glutCreateMenu(HandleSpeedMenu);
# 197 "tv.ag"
      glutAddMenuEntry("Halt (p)", 1);
# 198 "tv.ag"
      glutAddMenuEntry("Langsam (s)", 2);
# 199 "tv.ag"
      glutAddMenuEntry("Normal (n)", 3);
# 200 "tv.ag"
      glutAddMenuEntry("Schnell (f)", 4);
# 201 "tv.ag"

# 202 "tv.ag"
      mainmenu = glutCreateMenu(HandleMainMenu);
# 203 "tv.ag"
      glutAddSubMenu("Blickwinkel", anglemenu);
# 204 "tv.ag"
      glutAddSubMenu("Geschwindigkeit", speedmenu);
# 205 "tv.ag"
      glutAddMenuEntry("Startposition (Home)", 5);
# 206 "tv.ag"
      glutAddMenuEntry("Quit", 100);
# 207 "tv.ag"

# 208 "tv.ag"
      glutSetMenu(mainmenu);
# 209 "tv.ag"
      glutAttachMenu(GLUT_RIGHT_BUTTON);
# 210 "tv.ag"

# 211 "tv.ag"
      CreateEnvironment();
# 212 "tv.ag"

# 213 "tv.ag"
      // Kamera-Parameter in Grundposition
# 214 "tv.ag"
      Home();
# 215 "tv.ag"

# 216 "tv.ag"
      glutMainLoop();
# 217 "tv.ag"
      return 0;
# 218 "tv.ag"
    }
# 219 "tv.ag"
  catch (const char* msg)
# 220 "tv.ag"
    {
# 221 "tv.ag"
      cerr << "Exception: " << msg << endl;
# 222 "tv.ag"
    }
# 223 "tv.ag"
  catch (const string& msg)
# 224 "tv.ag"
    {
# 225 "tv.ag"
      cerr << "Exception: " << msg << endl;
# 226 "tv.ag"
    }
# 227 "tv.ag"
}
# 228 "tv.ag"
/*
# 229 "tv.ag"
   Globale Einstellungen
# 230 "tv.ag"
*/
# 231 "tv.ag"

# 232 "tv.ag"
void CreateEnvironment(void)
# 233 "tv.ag"
{
# 234 "tv.ag"
  glEnable(GL_DEPTH_TEST);
# 235 "tv.ag"

# 236 "tv.ag"
  glEnable(GL_LINE_SMOOTH);
# 237 "tv.ag"
  glEnable(GL_POINT_SMOOTH);
# 238 "tv.ag"
  glEnable(GL_POLYGON_SMOOTH);
# 239 "tv.ag"

# 240 "tv.ag"
  glShadeModel(GL_SMOOTH);
# 241 "tv.ag"
  glDisable(GL_DITHER);         /* Assume RGBA capabilities */
# 242 "tv.ag"

# 243 "tv.ag"
  glLineWidth(1.0);
# 244 "tv.ag"
  glPointSize(1.0);
# 245 "tv.ag"
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
# 246 "tv.ag"

# 247 "tv.ag"
  glFrontFace(GL_CCW);
# 248 "tv.ag"
  glEnable(GL_DEPTH_TEST);
# 249 "tv.ag"
  glDisable(GL_CULL_FACE);
# 250 "tv.ag"

# 251 "tv.ag"
  // glEnable(GL_CULL_FACE); blendet eine Seite von Flächen aus
# 252 "tv.ag"
  // glCullFace(GL_BACK);
# 253 "tv.ag"

# 254 "tv.ag"
  glClearColor(0.0, 0.0, 0.0, 0.0);      /* Background colour */
# 255 "tv.ag"

# 256 "tv.ag"
  glEnable(GL_COLOR_MATERIAL);
# 257 "tv.ag"
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
# 258 "tv.ag"
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, null);
# 259 "tv.ag"
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, null);
# 260 "tv.ag"
}
# 261 "tv.ag"

# 262 "tv.ag"
/*
# 263 "tv.ag"
   This is the basic display callback routine
# 264 "tv.ag"
   It creates the geometry, lighting, and viewing position
# 265 "tv.ag"
*/
# 266 "tv.ag"

# 267 "tv.ag"
void Display(void)
# 268 "tv.ag"
{
# 269 "tv.ag"
//    cout << "display" << Time << endl;
# 270 "tv.ag"
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
# 271 "tv.ag"

# 272 "tv.ag"
  MakeCamera();
# 273 "tv.ag"
  MakeLighting();
# 274 "tv.ag"
  MakeGeometry();
# 275 "tv.ag"

# 276 "tv.ag"
  glutSwapBuffers();
# 277 "tv.ag"
}
# 278 "tv.ag"

# 279 "tv.ag"
/*
# 280 "tv.ag"
   Create the geometry
# 281 "tv.ag"
*/
# 282 "tv.ag"
void MakeGeometry(void)
# 283 "tv.ag"
{
# 284 "tv.ag"
//    cout << "geometrie" << endl;
# 285 "tv.ag"
  for (auto& t : world)
# 286 "tv.ag"
    t.draw();
# 287 "tv.ag"
}
# 288 "tv.ag"

# 289 "tv.ag"
/*
# 290 "tv.ag"
   Set up the lighting environment
# 291 "tv.ag"
*/
# 292 "tv.ag"
void MakeLighting(void)
# 293 "tv.ag"
{
# 294 "tv.ag"
  /* Set ambient globally, default ambient for light sources is 0 */
# 295 "tv.ag"
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalambient);
# 296 "tv.ag"

# 297 "tv.ag"
  if (light_number > 0)
# 298 "tv.ag"
    {
# 299 "tv.ag"
      glLightfv(GL_LIGHT0, GL_POSITION, pos0);
# 300 "tv.ag"
      glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
# 301 "tv.ag"
      glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
# 302 "tv.ag"
      glLightfv(GL_LIGHT0, GL_SPECULAR, dif0);
# 303 "tv.ag"
      glEnable(GL_LIGHT0);
# 304 "tv.ag"
    }
# 305 "tv.ag"

# 306 "tv.ag"
  if (light_number > 1)
# 307 "tv.ag"
    {
# 308 "tv.ag"
      glLightfv(GL_LIGHT1, GL_POSITION, pos1);
# 309 "tv.ag"
      glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
# 310 "tv.ag"
      glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
# 311 "tv.ag"
      glLightfv(GL_LIGHT1, GL_SPECULAR, dif1);
# 312 "tv.ag"
      glEnable(GL_LIGHT1);
# 313 "tv.ag"
    }
# 314 "tv.ag"

# 315 "tv.ag"
  glEnable(GL_LIGHTING);
# 316 "tv.ag"
}
# 317 "tv.ag"

# 318 "tv.ag"
/*
# 319 "tv.ag"
   Set up the camera
# 320 "tv.ag"
*/
# 321 "tv.ag"

# 322 "tv.ag"
void Home()
# 323 "tv.ag"
{
# 324 "tv.ag"
  move_step = cameraDistance / 20;
# 325 "tv.ag"
  switch (cammode)
# 326 "tv.ag"
    {
# 327 "tv.ag"
    case 1: // no break
# 328 "tv.ag"
    case 3:
# 329 "tv.ag"
      camdistance = cameraDistance;
# 330 "tv.ag"
      leftrightrotate = 80;
# 331 "tv.ag"
      updownrotate = 70;
# 332 "tv.ag"
      camangle = 40;
# 333 "tv.ag"
      break;
# 334 "tv.ag"

# 335 "tv.ag"
    case 2:
# 336 "tv.ag"
      glMatrixMode(GL_PROJECTION);
# 337 "tv.ag"
      glLoadIdentity();
# 338 "tv.ag"
      gluPerspective(camangle,     /* Field of view */
# 339 "tv.ag"
                     window_ratio, /* aspect ratio  */
# 340 "tv.ag"
                     0.1, 10000.0); /* near and far  */
# 341 "tv.ag"
      glMatrixMode(GL_MODELVIEW);
# 342 "tv.ag"
      glLoadIdentity();
# 343 "tv.ag"
      // initiale Position wie bei 1
# 344 "tv.ag"
      gluLookAt(cameraDistance * Cos(80) * Sin(70),
# 345 "tv.ag"
                cameraDistance * Cos(70),
# 346 "tv.ag"
                cameraDistance * Sin(80) * Sin(70),
# 347 "tv.ag"
                0.0, 0.0, 0.0,                                 /* Center   */
# 348 "tv.ag"
                0.0, 1.0, 0.0);                                /* Up       */
# 349 "tv.ag"
      break;
# 350 "tv.ag"
    }
# 351 "tv.ag"
}
# 352 "tv.ag"

# 353 "tv.ag"
void MakeCamera()
# 354 "tv.ag"
{
# 355 "tv.ag"
  /* Camera setup */
# 356 "tv.ag"
  glMatrixMode(GL_PROJECTION);
# 357 "tv.ag"
  glLoadIdentity();
# 358 "tv.ag"
  gluPerspective(camangle,     /* Field of view */
# 359 "tv.ag"
                 window_ratio, /* aspect ratio  */
# 360 "tv.ag"
                 0.1, 10000.0); /* near and far  */
# 361 "tv.ag"
  glMatrixMode(GL_MODELVIEW);
# 362 "tv.ag"

# 363 "tv.ag"
  if (cammode != 2)
# 364 "tv.ag"
    {
# 365 "tv.ag"
      glLoadIdentity();
# 366 "tv.ag"

# 367 "tv.ag"
      switch (cammode)
# 368 "tv.ag"
        {
# 369 "tv.ag"
        case 1:
# 370 "tv.ag"
        case 3:
# 371 "tv.ag"
          gluLookAt(camdistance * Cos(leftrightrotate)*Sin(updownrotate),
# 372 "tv.ag"
                    camdistance * Cos(updownrotate),
# 373 "tv.ag"
                    camdistance * Sin(leftrightrotate)*Sin(updownrotate),
# 374 "tv.ag"
                    0.0, 0.0, 0.0,                                 /* Center   */
# 375 "tv.ag"
                    0.0, 1.0, 0.0);                                /* Up       */
# 376 "tv.ag"
          break;
# 377 "tv.ag"
        }
# 378 "tv.ag"
    }
# 379 "tv.ag"
}
# 380 "tv.ag"

# 381 "tv.ag"
/*
# 382 "tv.ag"
  Tastaturbehandlung
# 383 "tv.ag"
*/
# 384 "tv.ag"
void HandleKeyboard(unsigned char key, int x, int y)
# 385 "tv.ag"
{
# 386 "tv.ag"
  int modifier = glutGetModifiers();
# 387 "tv.ag"
  switch (modifier)
# 388 "tv.ag"
    {
# 389 "tv.ag"
    case 0:
# 390 "tv.ag"
      switch (key)
# 391 "tv.ag"
        {
# 392 "tv.ag"
        case 27: /* ESC */
# 393 "tv.ag"
        case 'h':
# 394 "tv.ag"
        case 'H':
# 395 "tv.ag"
          Home();
# 396 "tv.ag"
          break;
# 397 "tv.ag"
        case 'Q':
# 398 "tv.ag"
        case 'q':
# 399 "tv.ag"
          exit(0);
# 400 "tv.ag"
          break;
# 401 "tv.ag"
        case 'P':
# 402 "tv.ag"
        case 'p':
# 403 "tv.ag"
          timestat = tm_halt;
# 404 "tv.ag"
          break;
# 405 "tv.ag"
        case 'S':
# 406 "tv.ag"
        case 's':
# 407 "tv.ag"
          timestat = tm_slow;
# 408 "tv.ag"
          break;
# 409 "tv.ag"
        case 'N':
# 410 "tv.ag"
        case 'n':
# 411 "tv.ag"
          timestat = tm_normal;
# 412 "tv.ag"
          break;
# 413 "tv.ag"
        case 'F':
# 414 "tv.ag"
        case 'f':
# 415 "tv.ag"
          timestat = tm_fast;
# 416 "tv.ag"
          break;
# 417 "tv.ag"
        case 'w':
# 418 "tv.ag"
        case 'W':
# 419 "tv.ag"
          camangle = 80;
# 420 "tv.ag"
          break;
# 421 "tv.ag"
        case 't':
# 422 "tv.ag"
        case 'T':
# 423 "tv.ag"
          camangle = 20;
# 424 "tv.ag"
          break;
# 425 "tv.ag"
        // Zoom
# 426 "tv.ag"
        case '-':
# 427 "tv.ag"
          if (camangle < 170) camangle++;
# 428 "tv.ag"
          break;
# 429 "tv.ag"
        case '+':
# 430 "tv.ag"
          if (camangle > 1) camangle--;
# 431 "tv.ag"
          break;
# 432 "tv.ag"

# 433 "tv.ag"
        case 'M':
# 434 "tv.ag"
          Trafo tr;
# 435 "tv.ag"
          tr.glGet();
# 436 "tv.ag"
          //  cout << tr.toString() << endl;
# 437 "tv.ag"
          //  cout << "------------" << endl;
# 438 "tv.ag"

# 439 "tv.ag"
          GLfloat m[16];
# 440 "tv.ag"
          glGetFloatv(GL_PROJECTION_MATRIX, m);
# 441 "tv.ag"
          cout << endl;
# 442 "tv.ag"
          for (int i = 0; i < 4; i++)
# 443 "tv.ag"
            {
# 444 "tv.ag"
              for (int j = 0; j < 4; j++)
# 445 "tv.ag"
                cout << setw(10) << setprecision(5) << m[j * 4 + i] << "\t";
# 446 "tv.ag"
              cout << endl;
# 447 "tv.ag"
            }
# 448 "tv.ag"

# 449 "tv.ag"
          break;
# 450 "tv.ag"
        }
# 451 "tv.ag"
      break;
# 452 "tv.ag"

# 453 "tv.ag"
    case GLUT_ACTIVE_CTRL:
# 454 "tv.ag"
      switch (key)
# 455 "tv.ag"
        {
# 456 "tv.ag"
        case '+':
# 457 "tv.ag"
          move_step *= 2;
# 458 "tv.ag"
          cout << "Step: " << move_step << endl;
# 459 "tv.ag"
          break;
# 460 "tv.ag"
        case '-':
# 461 "tv.ag"
          move_step *= 0.5;
# 462 "tv.ag"
          cout << "Step: " << move_step << endl;
# 463 "tv.ag"
          break;
# 464 "tv.ag"
        }
# 465 "tv.ag"
      break;
# 466 "tv.ag"
    }
# 467 "tv.ag"
}
# 468 "tv.ag"

# 469 "tv.ag"
/*
# 470 "tv.ag"
  Spezielle Tasten
# 471 "tv.ag"
*/
# 472 "tv.ag"
void CamControl1(int key, int modifier)
# 473 "tv.ag"
{
# 474 "tv.ag"
  switch (key)
# 475 "tv.ag"
    {
# 476 "tv.ag"
    case GLUT_KEY_LEFT:
# 477 "tv.ag"
      leftrightrotate += 3;
# 478 "tv.ag"
      break;
# 479 "tv.ag"
    case GLUT_KEY_RIGHT:
# 480 "tv.ag"
      leftrightrotate -= 3;
# 481 "tv.ag"
      break;
# 482 "tv.ag"
    case GLUT_KEY_UP:
# 483 "tv.ag"
      updownrotate -= 2;
# 484 "tv.ag"
      break;
# 485 "tv.ag"
    case GLUT_KEY_DOWN:
# 486 "tv.ag"
      updownrotate += 2;
# 487 "tv.ag"
      break;
# 488 "tv.ag"
    case GLUT_KEY_PAGE_UP:
# 489 "tv.ag"
      camdistance *= 0.8;
# 490 "tv.ag"
      break;
# 491 "tv.ag"
    case GLUT_KEY_PAGE_DOWN:
# 492 "tv.ag"
      camdistance *= 1.2;
# 493 "tv.ag"
      break;
# 494 "tv.ag"
    case GLUT_KEY_HOME:
# 495 "tv.ag"
      Home();
# 496 "tv.ag"
      break;
# 497 "tv.ag"
    }
# 498 "tv.ag"
  if (updownrotate > 179.9) updownrotate = 179.9;
# 499 "tv.ag"
  if (updownrotate < 0.1) updownrotate = 0.1;
# 500 "tv.ag"
}
# 501 "tv.ag"

# 502 "tv.ag"
void CamControl3(int key, int modifier)
# 503 "tv.ag"
{
# 504 "tv.ag"
  switch (key)
# 505 "tv.ag"
    {
# 506 "tv.ag"
    case GLUT_KEY_LEFT:
# 507 "tv.ag"
      rspeed -= 10;
# 508 "tv.ag"
      break;
# 509 "tv.ag"
    case GLUT_KEY_RIGHT:
# 510 "tv.ag"
      rspeed += 10;
# 511 "tv.ag"
      break;
# 512 "tv.ag"
    case GLUT_KEY_UP:
# 513 "tv.ag"
      updownrotate -= 2;
# 514 "tv.ag"
      break;
# 515 "tv.ag"
    case GLUT_KEY_DOWN:
# 516 "tv.ag"
      updownrotate += 2;
# 517 "tv.ag"
      break;
# 518 "tv.ag"
    case GLUT_KEY_PAGE_UP:
# 519 "tv.ag"
      camdistance *= 0.8;
# 520 "tv.ag"
      break;
# 521 "tv.ag"
    case GLUT_KEY_PAGE_DOWN:
# 522 "tv.ag"
      camdistance *= 1.2;
# 523 "tv.ag"
      break;
# 524 "tv.ag"
    case GLUT_KEY_HOME:
# 525 "tv.ag"
      Home();
# 526 "tv.ag"
      break;
# 527 "tv.ag"
    }
# 528 "tv.ag"
  if (updownrotate > 179.9) updownrotate = 179.9;
# 529 "tv.ag"
  if (updownrotate < 0.1) updownrotate = 0.1;
# 530 "tv.ag"
}
# 531 "tv.ag"

# 532 "tv.ag"
void Cam4RotLeft(float val)
# 533 "tv.ag"
{
# 534 "tv.ag"
  GLfloat m[16];
# 535 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 536 "tv.ag"
  glLoadIdentity();
# 537 "tv.ag"
  glRotated(val, 0, -1, 0);
# 538 "tv.ag"
  glMultMatrixf(m);
# 539 "tv.ag"
}
# 540 "tv.ag"

# 541 "tv.ag"
void Cam4RotUp(float val)
# 542 "tv.ag"
{
# 543 "tv.ag"
  GLfloat m[16];
# 544 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 545 "tv.ag"
  glLoadIdentity();
# 546 "tv.ag"
  glRotated(val, -1, 0, 0);
# 547 "tv.ag"
  glMultMatrixf(m);
# 548 "tv.ag"
}
# 549 "tv.ag"

# 550 "tv.ag"
void Cam4Roll(float val)
# 551 "tv.ag"
{
# 552 "tv.ag"
  GLfloat m[16];
# 553 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 554 "tv.ag"
  glLoadIdentity();
# 555 "tv.ag"
  glRotated(val, 0, 0, 1);
# 556 "tv.ag"
  glMultMatrixf(m);
# 557 "tv.ag"
}
# 558 "tv.ag"

# 559 "tv.ag"
void Cam4MoveUp(float val)
# 560 "tv.ag"
{
# 561 "tv.ag"
  GLfloat m[16];
# 562 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 563 "tv.ag"
  glLoadIdentity();
# 564 "tv.ag"
  glTranslated(0, -val, 0);
# 565 "tv.ag"
  glMultMatrixf(m);
# 566 "tv.ag"
}
# 567 "tv.ag"

# 568 "tv.ag"
void Cam4MoveLeft(float val)
# 569 "tv.ag"
{
# 570 "tv.ag"
  GLfloat m[16];
# 571 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 572 "tv.ag"
  glLoadIdentity();
# 573 "tv.ag"
  glTranslated(val, 0, 0);
# 574 "tv.ag"
  glMultMatrixf(m);
# 575 "tv.ag"
}
# 576 "tv.ag"

# 577 "tv.ag"
void Cam4MoveForward(float val)
# 578 "tv.ag"
{
# 579 "tv.ag"
  GLfloat m[16];
# 580 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 581 "tv.ag"
  glLoadIdentity();
# 582 "tv.ag"
  glTranslated(0, 0, val);
# 583 "tv.ag"
  glMultMatrixf(m);
# 584 "tv.ag"
}
# 585 "tv.ag"

# 586 "tv.ag"
void CamControl2(int key, int modifier)
# 587 "tv.ag"
{
# 588 "tv.ag"
  switch (modifier)
# 589 "tv.ag"
    {
# 590 "tv.ag"
    case 0:
# 591 "tv.ag"

# 592 "tv.ag"
      switch (key)
# 593 "tv.ag"
        {
# 594 "tv.ag"
        case GLUT_KEY_LEFT:
# 595 "tv.ag"
          Cam4RotLeft(angle_step);
# 596 "tv.ag"
          break;
# 597 "tv.ag"
        case GLUT_KEY_RIGHT:
# 598 "tv.ag"
          Cam4RotLeft(-angle_step);
# 599 "tv.ag"
          break;
# 600 "tv.ag"

# 601 "tv.ag"
        case GLUT_KEY_UP:
# 602 "tv.ag"
          Cam4RotUp(angle_step);
# 603 "tv.ag"
          break;
# 604 "tv.ag"
        case GLUT_KEY_DOWN:
# 605 "tv.ag"
          Cam4RotUp(-angle_step);
# 606 "tv.ag"
          break;
# 607 "tv.ag"

# 608 "tv.ag"
        case GLUT_KEY_PAGE_UP:
# 609 "tv.ag"
          Cam4MoveUp(move_step);
# 610 "tv.ag"
          break;
# 611 "tv.ag"
        case GLUT_KEY_PAGE_DOWN:
# 612 "tv.ag"
          Cam4MoveUp(-move_step);
# 613 "tv.ag"
          break;
# 614 "tv.ag"

# 615 "tv.ag"
        case GLUT_KEY_HOME:
# 616 "tv.ag"
          Home();
# 617 "tv.ag"
          break;
# 618 "tv.ag"
        }
# 619 "tv.ag"
      break;
# 620 "tv.ag"

# 621 "tv.ag"
    case GLUT_ACTIVE_CTRL:
# 622 "tv.ag"

# 623 "tv.ag"
      switch (key)
# 624 "tv.ag"
        {
# 625 "tv.ag"
        case GLUT_KEY_LEFT:
# 626 "tv.ag"
          Cam4MoveLeft(move_step);
# 627 "tv.ag"
          break;
# 628 "tv.ag"
        case GLUT_KEY_RIGHT:
# 629 "tv.ag"
          Cam4MoveLeft(-move_step);
# 630 "tv.ag"
          break;
# 631 "tv.ag"

# 632 "tv.ag"
        case GLUT_KEY_UP:
# 633 "tv.ag"
          Cam4MoveForward(move_step);
# 634 "tv.ag"
          break;
# 635 "tv.ag"
        case GLUT_KEY_DOWN:
# 636 "tv.ag"
          Cam4MoveForward(-move_step);
# 637 "tv.ag"
          break;
# 638 "tv.ag"

# 639 "tv.ag"
        case GLUT_KEY_PAGE_UP:
# 640 "tv.ag"
          Cam4MoveUp(move_step);
# 641 "tv.ag"
          break;
# 642 "tv.ag"
        case GLUT_KEY_PAGE_DOWN:
# 643 "tv.ag"
          Cam4MoveUp(-move_step);
# 644 "tv.ag"
          break;
# 645 "tv.ag"

# 646 "tv.ag"
        case '+':
# 647 "tv.ag"
          move_step *= 2;
# 648 "tv.ag"
          break;
# 649 "tv.ag"
        case '-':
# 650 "tv.ag"
          move_step *= 0.5;
# 651 "tv.ag"
          break;
# 652 "tv.ag"
        }
# 653 "tv.ag"
      break;
# 654 "tv.ag"

# 655 "tv.ag"
    case GLUT_ACTIVE_SHIFT:
# 656 "tv.ag"
      switch (key)
# 657 "tv.ag"
        {
# 658 "tv.ag"
        case GLUT_KEY_LEFT:
# 659 "tv.ag"
          Cam4Roll(2);
# 660 "tv.ag"
          break;
# 661 "tv.ag"
        case GLUT_KEY_RIGHT:
# 662 "tv.ag"
          Cam4Roll(-2);
# 663 "tv.ag"
          break;
# 664 "tv.ag"
        }
# 665 "tv.ag"

# 666 "tv.ag"
      break;
# 667 "tv.ag"
    }
# 668 "tv.ag"
}
# 669 "tv.ag"

# 670 "tv.ag"
void HandleSpecialKeyboard(int key, int x, int y)
# 671 "tv.ag"
{
# 672 "tv.ag"
  int modifier = glutGetModifiers();
# 673 "tv.ag"
  // Masken GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL, GLUT_ACTIVE_ALT
# 674 "tv.ag"
  switch (cammode)
# 675 "tv.ag"
    {
# 676 "tv.ag"
    case 1:
# 677 "tv.ag"
      CamControl1(key, modifier);
# 678 "tv.ag"
      break;
# 679 "tv.ag"
    case 2:
# 680 "tv.ag"
      CamControl2(key, modifier);
# 681 "tv.ag"
      break;
# 682 "tv.ag"
    case 3:
# 683 "tv.ag"
      CamControl3(key, modifier);
# 684 "tv.ag"
      break;
# 685 "tv.ag"
    }
# 686 "tv.ag"
}
# 687 "tv.ag"

# 688 "tv.ag"
/*
# 689 "tv.ag"
   Handle mouse events
# 690 "tv.ag"
*/
# 691 "tv.ag"
void HandleMouse(int button, int state, int x, int y)
# 692 "tv.ag"
{
# 693 "tv.ag"
  if (state == GLUT_DOWN)
# 694 "tv.ag"
    {
# 695 "tv.ag"
    }
# 696 "tv.ag"
}
# 697 "tv.ag"

# 698 "tv.ag"
void HandleReshape(int width, int height)
# 699 "tv.ag"
{
# 700 "tv.ag"
  window_width = width;
# 701 "tv.ag"
  window_height = height;
# 702 "tv.ag"
  window_ratio = ((float)window_width) / ((float)window_height);
# 703 "tv.ag"
  glViewport(0, 0, width, height);
# 704 "tv.ag"
}
# 705 "tv.ag"
/*
# 706 "tv.ag"
   Handle the main menu
# 707 "tv.ag"
*/
# 708 "tv.ag"
void HandleMainMenu(int whichone)
# 709 "tv.ag"
{
# 710 "tv.ag"
  switch (whichone)
# 711 "tv.ag"
    {
# 712 "tv.ag"
    case 1:
# 713 "tv.ag"
      timestat = tm_halt;
# 714 "tv.ag"
      break;
# 715 "tv.ag"
    case 2:
# 716 "tv.ag"
      timestat = tm_slow;
# 717 "tv.ag"
      break;
# 718 "tv.ag"
    case 3:
# 719 "tv.ag"
      timestat = tm_normal;
# 720 "tv.ag"
      break;
# 721 "tv.ag"
    case 5:
# 722 "tv.ag"
      Home();
# 723 "tv.ag"
      break;
# 724 "tv.ag"
    case 100:
# 725 "tv.ag"
      exit(0);
# 726 "tv.ag"
      break;
# 727 "tv.ag"
    }
# 728 "tv.ag"
}
# 729 "tv.ag"

# 730 "tv.ag"
/*
# 731 "tv.ag"
  BlickWinkelMenu
# 732 "tv.ag"
*/
# 733 "tv.ag"
void HandleAngleMenu(int whichone)
# 734 "tv.ag"
{
# 735 "tv.ag"
  switch (whichone)
# 736 "tv.ag"
    {
# 737 "tv.ag"
    case 1:
# 738 "tv.ag"
      camangle = 80;
# 739 "tv.ag"
      break;
# 740 "tv.ag"
    case 2:
# 741 "tv.ag"
      camangle = 40;
# 742 "tv.ag"
      break;
# 743 "tv.ag"
    case 3:
# 744 "tv.ag"
      camangle = 20;
# 745 "tv.ag"
      break;
# 746 "tv.ag"
    }
# 747 "tv.ag"
}
# 748 "tv.ag"

# 749 "tv.ag"
/*
# 750 "tv.ag"
  Geschwindigkeitsmenu
# 751 "tv.ag"
*/
# 752 "tv.ag"
void HandleSpeedMenu(int whichone)
# 753 "tv.ag"
{
# 754 "tv.ag"
  switch (whichone)
# 755 "tv.ag"
    {
# 756 "tv.ag"
    case 1:
# 757 "tv.ag"
      timestat = tm_halt;
# 758 "tv.ag"
      break;
# 759 "tv.ag"
    case 2:
# 760 "tv.ag"
      timestat = tm_slow;
# 761 "tv.ag"
      break;
# 762 "tv.ag"
    case 3:
# 763 "tv.ag"
      timestat = tm_normal;
# 764 "tv.ag"
      break;
# 765 "tv.ag"
    case 4:
# 766 "tv.ag"
      timestat = tm_fast;
# 767 "tv.ag"
      break;
# 768 "tv.ag"
    }
# 769 "tv.ag"
}
# 770 "tv.ag"

# 771 "tv.ag"
/*
# 772 "tv.ag"
   How to handle visibility
# 773 "tv.ag"
*/
# 774 "tv.ag"
void HandleVisibility(int gvisible)
# 775 "tv.ag"
{
# 776 "tv.ag"
//   cout << "visibility " << (visible==GLUT_VISIBLE) << endl;
# 777 "tv.ag"
  if (gvisible == GLUT_VISIBLE)
# 778 "tv.ag"
    visible = true;
# 779 "tv.ag"
  else
# 780 "tv.ag"
    visible = false;
# 781 "tv.ag"
}
# 782 "tv.ag"

# 783 "tv.ag"
void HandleIdle()
# 784 "tv.ag"
{
# 785 "tv.ag"
  double atime;
# 786 "tv.ag"
  static double lasttime = -1;
# 787 "tv.ag"

# 788 "tv.ag"
#ifndef WIN32
# 789 "tv.ag"
  struct timeval tt;
# 790 "tv.ag"
  struct timezone tz;
# 791 "tv.ag"

# 792 "tv.ag"
  gettimeofday(&tt, &tz);
# 793 "tv.ag"
  atime = tt.tv_sec + (double)tt.tv_usec / 1000000.0;
# 794 "tv.ag"
#else
# 795 "tv.ag"
  struct timeb tt;
# 796 "tv.ag"
  ftime(&tt);
# 797 "tv.ag"
  atime = tt.time + (double)tt.millitm / 1000;
# 798 "tv.ag"
#endif
# 799 "tv.ag"
  double dt;
# 800 "tv.ag"
  if (lasttime < 0)
# 801 "tv.ag"
    dt = 0;
# 802 "tv.ag"
  else
# 803 "tv.ag"
    dt = atime - lasttime;
# 804 "tv.ag"
  //  if (dt > 1)
# 805 "tv.ag"
  //    dt = 1.0;
# 806 "tv.ag"
  lasttime = atime;
# 807 "tv.ag"

# 808 "tv.ag"
  if (benchmarking)
# 809 "tv.ag"
    {
# 810 "tv.ag"
      RealTime += dt;
# 811 "tv.ag"

# 812 "tv.ag"
      if (RealTime < 10)
# 813 "tv.ag"
        frametime = dt;
# 814 "tv.ag"
      else
# 815 "tv.ag"
        frametime = frametime * 0.999 + 0.001 * dt;
# 816 "tv.ag"

# 817 "tv.ag"
      if (RealTime > 1)
# 818 "tv.ag"
        cout << 1.0 / frametime << " fps " << endl;
# 819 "tv.ag"
    }
# 820 "tv.ag"

# 821 "tv.ag"
  switch (timestat)
# 822 "tv.ag"
    {
# 823 "tv.ag"
    case tm_halt:
# 824 "tv.ag"
      break;
# 825 "tv.ag"
    case tm_slow:
# 826 "tv.ag"
      VirtualTime += 0.2 * dt;
# 827 "tv.ag"
      break;
# 828 "tv.ag"
    case tm_normal:
# 829 "tv.ag"
      VirtualTime += dt;
# 830 "tv.ag"
      break;
# 831 "tv.ag"
    case tm_fast:
# 832 "tv.ag"
      VirtualTime += 5 * dt;
# 833 "tv.ag"
      break;
# 834 "tv.ag"
    }
# 835 "tv.ag"

# 836 "tv.ag"
  if (cammode == 3)
# 837 "tv.ag"
    leftrightrotate += dt * rspeed;
# 838 "tv.ag"

# 839 "tv.ag"
  if (visible)
# 840 "tv.ag"
    {
# 841 "tv.ag"
      if (!benchmarking)
# 842 "tv.ag"
        usleep(20000);
# 843 "tv.ag"
      glutPostRedisplay();
# 844 "tv.ag"
    }
# 845 "tv.ag"
}
# 846 "tv.ag"

# 847 "tv.ag"
/*
# 848 "tv.ag"
  Draw text in the x-y plane
# 849 "tv.ag"
  The x,y,z coordinate is the bottom left corner (looking down -ve z axis)
# 850 "tv.ag"
*/
# 851 "tv.ag"

# 852 "tv.ag"
void DrawTextXY(float x, float y, float z, float scale, char* s)
# 853 "tv.ag"
{
# 854 "tv.ag"
  unsigned int i;
# 855 "tv.ag"

# 856 "tv.ag"
  glPushMatrix();
# 857 "tv.ag"
  glTranslated(x, y, z);
# 858 "tv.ag"
  glScaled(scale, scale, scale);
# 859 "tv.ag"
  for (i = 0; i < strlen(s); i++)
# 860 "tv.ag"
    glutStrokeCharacter(GLUT_STROKE_ROMAN, s[i]);
# 861 "tv.ag"
  glPopMatrix();
# 862 "tv.ag"
}
# 863 "tv.ag"

# 864 "tv.ag"
/*
# 865 "tv.ag"
   Display the program usage information
# 866 "tv.ag"
*/
# 867 "tv.ag"
void Usage(const string& cmd, bool tasten)
# 868 "tv.ag"
{
# 869 "tv.ag"
  fprintf(stderr, "Anwendung:    %s <optionen> [<filename>]\n", cmd.c_str());
# 870 "tv.ag"
  fprintf(stderr, "  -h Hilfe\n");
# 871 "tv.ag"
  fprintf(stderr, "  -f <filename> Welt (welt.cv)\n");
# 872 "tv.ag"
  fprintf(stderr, "  -c <n> Kameramodell\n");
# 873 "tv.ag"
  fprintf(stderr, "     1 Kamera kann um Objekt (Ursprung) bewegt werden\n");
# 874 "tv.ag"
  fprintf(stderr, "     2 Freie Kamerabewegung in der Welt\n");
# 875 "tv.ag"
  fprintf(stderr, "     3 Kamera bewegt sich um Objekt (Ursprung) herum\n");
# 876 "tv.ag"
  fprintf(stderr, "  -d Skript-Debugging\n");
# 877 "tv.ag"

# 878 "tv.ag"
  fprintf(stderr, "  -t <time> Zeitvorgabe (stoppt Bewegungen)\n");
# 879 "tv.ag"
  fprintf(stderr, "  -s langsamer Zeitablauf\n");
# 880 "tv.ag"
  fprintf(stderr, "  -b Benchmark-Modus (Ausgabe von Frame-Raten\n");
# 881 "tv.ag"

# 882 "tv.ag"
  fprintf(stderr, "  -x <width>  Breite des Bildfensters\n");
# 883 "tv.ag"
  fprintf(stderr, "  -y <height> Höhe des Bildfensters\n");
# 884 "tv.ag"

# 885 "tv.ag"
  fprintf(stderr, "  -p Nur Parsen\n");
# 886 "tv.ag"
  fprintf(stderr, "  -o Optimierung durchführen\n");
# 887 "tv.ag"
  fprintf(stderr, "  -m <nr> minimale Segmentanzahl runder Körper\n");
# 888 "tv.ag"
  if (tasten)
# 889 "tv.ag"
    {
# 890 "tv.ag"
      fprintf(stderr, "\nTasten:\n");
# 891 "tv.ag"
      fprintf(stderr, "               q - Beenden\n");
# 892 "tv.ag"
      fprintf(stderr, "               p - Pause\n");
# 893 "tv.ag"
      fprintf(stderr, "               s - langsam\n");
# 894 "tv.ag"
      fprintf(stderr, "               n - normal\n");
# 895 "tv.ag"
      fprintf(stderr, "               f - schnell\n");
# 896 "tv.ag"
      fprintf(stderr, "Pfeiltasten: Bewegung bzw. Drehung (Mode-Abhängig)\n");
# 897 "tv.ag"
    }
# 898 "tv.ag"
  exit(-1);
# 899 "tv.ag"
}
# 900 "tv.ag"

# 901 "tv.ag"
void Usage(const string& cmd, const char* error)
# 902 "tv.ag"
{
# 903 "tv.ag"
  fprintf(stderr, "\n!!! %s !!!\n\n", error);
# 904 "tv.ag"
  Usage(cmd, false);
# 905 "tv.ag"
}

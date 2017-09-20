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
GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};// ambient light for all light sources
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
  opt: C, center, void, centerModel, Modellmittelpunkt in Ursprung schieben, false
# 91 "tv.ag"
  opt: x, xsize,  Int,     window_width,    Bildgröße X,  1280
# 92 "tv.ag"
  opt: y, ysize,  Int,     window_height,    Bildgröße Y,  1024
# 93 "tv.ag"

# 94 "tv.ag"
  opt: c, camera, Int, cammode, Kamera-Modus,2
# 95 "tv.ag"
  opt2:   1 - Kamera sieht auf den Ursprung
# 96 "tv.ag"
  opt2:   2 - Kamera frei beweglich im Raum
# 97 "tv.ag"
  opt2:   3 - Kamera rotiert um den Ursprung
# 98 "tv.ag"

# 99 "tv.ag"
  opt: D, cameradistance, Double, cameraDistance, Abstand der Kamera, 30
# 100 "tv.ag"
  opt: a, anglestep, double, angledelta, Winkel-Schrittweite, 2
# 101 "tv.ag"

# 102 "tv.ag"
  opt: l, lights, int, std_lights, Zahl der Standard-Lichtquellen, 2
# 103 "tv.ag"

# 104 "tv.ag"
  opt: v, verbose,  Void,   verbose,  Ausführliche Meldungen,  false
# 105 "tv.ag"
  opt: b, benchmark, Void, benchmarking, Framerate anzeigen, false
# 106 "tv.ag"
  opt: d, debug, Int, cdebug, Debug-Modus, 0
# 107 "tv.ag"
  opt: h, help,   usage,   ,    Hilfe
# 108 "tv.ag"

# 109 "tv.ag"
%% "l:st:"
# 110 "tv.ag"
  AppGen*/
# 111 "tv.ag"

# 112 "tv.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - Programm zur Darstellung von 3D-Welten" << endl;
  cout << "Aufruf:" << endl;
  cout << ag_programName << " [<options>] [welt] " << endl;
  cout << "  welt - Weltbeschreibung (optional, default:\"welt.cv\")" << endl;
  cout << "Optionen:" << endl;
  cout << "  -f --worldfile       Dateiname Weltbeschreibung (default: \"\")" << endl;
  cout << "                       Dies ist eine alternative Form, die auch die Angabe von" << endl;
  cout << "                       -f - erlaubt, was die Standard-Eingabe als Quelle festlegt" << endl;
  cout << "  -X --xrotation       Rotation um 180° um X-Achse (default: false)" << endl;
  cout << "  -Y --yrotation       Rotation um 180° um Y-Achse (default: false)" << endl;
  cout << "  -Z --zrotation       Rotation um 180° um Z-Achse (default: false)" << endl;
  cout << "  -C --center          Modellmittelpunkt in Ursprung schieben (default: false)" << endl;
  cout << "  -x --xsize           Bildgröße X (default: 1280)" << endl;
  cout << "  -y --ysize           Bildgröße Y (default: 1024)" << endl;
  cout << "  -c --camera          Kamera-Modus (default: 2)" << endl;
  cout << "                       1 - Kamera sieht auf den Ursprung" << endl;
  cout << "                       2 - Kamera frei beweglich im Raum" << endl;
  cout << "                       3 - Kamera rotiert um den Ursprung" << endl;
  cout << "  -D --cameradistance  Abstand der Kamera (default: 30)" << endl;
  cout << "  -a --anglestep       Winkel-Schrittweite (default: 2)" << endl;
  cout << "  -l --lights          Zahl der Standard-Lichtquellen (default: 2)" << endl;
  cout << "  -v --verbose         Ausführliche Meldungen (default: false)" << endl;
  cout << "  -b --benchmark       Framerate anzeigen (default: false)" << endl;
  cout << "  -d --debug           Debug-Modus (default: 0)" << endl;
  cout << "  -h --help            Hilfe" << endl;
  exit(1);
}

void error(const string& msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char* para)
{
  char* end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char* para)
{
  char* end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char** argv)
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
          error("Unbekannte Option");
          break;
        case ':':
          error("erwarte Option-Parameter");
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
          window_width = ptoi(optarg);
          break;

        case 'y':
          window_height = ptoi(optarg);
          break;

        case 'c':
          cammode = ptoi(optarg);
          break;

        case 'D':
          cameraDistance = ptod(optarg);
          break;

        case 'a':
          angledelta = ptod(optarg);
          break;

        case 'l':
          std_lights = ptoi(optarg);
          break;

        case 'v':
          verbose = true;
          break;

        case 'b':
          benchmarking = true;
          break;

        case 'd':
          cdebug = ptoi(optarg);
          break;

        case 'h':
          usage();
          break;

        default:
          error("Unbekannter Fehler in Optionen");
        }
    }
  if (optind < argc)
    fname = argv[optind++];

  /*AppGen:MainEnd*/
# 116 "tv.ag"

# 117 "tv.ag"
  try
    {
# 118 "tv.ag"
      if (fname == "welt.cv") // default
# 119 "tv.ag"
        if (!fname_opt.empty()) // filename given as option
# 120 "tv.ag"
          fname = fname_opt;
# 121 "tv.ag"

# 122 "tv.ag"
      angle_step = angledelta;
# 123 "tv.ag"

# 124 "tv.ag"
      int dummy_argc = 1;
# 125 "tv.ag"
      char arg0[] = "tv";
# 126 "tv.ag"
      char* dummy_argv[] = {arg0, NULL};
# 127 "tv.ag"

# 128 "tv.ag"
      glutInit(&dummy_argc, dummy_argv);
# 129 "tv.ag"

# 130 "tv.ag"
      int mainmenu, anglemenu, speedmenu;
# 131 "tv.ag"

# 132 "tv.ag"
      // ein oder zwei Standard-lichtquellen
# 133 "tv.ag"
      // Szene-Beschreibung kann dies überschreiben
# 134 "tv.ag"
      if (std_lights > 2) std_lights = 2;
# 135 "tv.ag"
      if (std_lights < 1) std_lights = 1;
# 136 "tv.ag"

# 137 "tv.ag"
      // verhältnis seitenlängen der Fenster
# 138 "tv.ag"
      // (Pixel werden quadratisch angenommen)
# 139 "tv.ag"
      window_ratio = ((float)window_width) / ((float)window_height);
# 140 "tv.ag"

# 141 "tv.ag"
      int readFlags = 0;
# 142 "tv.ag"
      bool xs = false;
# 143 "tv.ag"
      bool ys = false;
# 144 "tv.ag"
      bool zs = false;
# 145 "tv.ag"
      if (xrotation)
# 146 "tv.ag"
        {
# 147 "tv.ag"
          ys = !ys;
          zs = !zs;
# 148 "tv.ag"
        }
# 149 "tv.ag"
      if (yrotation)
# 150 "tv.ag"
        {
# 151 "tv.ag"
          xs = !xs;
          zs = !zs;
# 152 "tv.ag"
        }
# 153 "tv.ag"
      if (zrotation)
# 154 "tv.ag"
        {
# 155 "tv.ag"
          xs = !xs;
          ys = !ys;
# 156 "tv.ag"
        }
# 157 "tv.ag"

# 158 "tv.ag"
      readFlags = (xs ? 1 : 0) | (ys ? 2 : 0) | (zs ? 4 : 0) | (centerModel ? 8 : 0);
# 159 "tv.ag"

# 160 "tv.ag"
      // Lesen des Szene-Files, Aufbau der Welt
# 161 "tv.ag"
      ReadWorld(world, fname, readFlags);
# 162 "tv.ag"

# 163 "tv.ag"
      if (light_number == 0) // weltbeschreibung enthält keine Lichtquellen
# 164 "tv.ag"
        {
# 165 "tv.ag"
          // Standard-Lichtquellen aktivieren
# 166 "tv.ag"
          light_number = std_lights;
# 167 "tv.ag"
        }
# 168 "tv.ag"

# 169 "tv.ag"
      /* Set things up and go */
# 170 "tv.ag"
      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
# 171 "tv.ag"
      glutInitWindowSize(window_width, window_height);
# 172 "tv.ag"
      glutCreateWindow("TriangleViewer");
# 173 "tv.ag"
      // call back funktionen setzen
# 174 "tv.ag"
      glutDisplayFunc(Display);
# 175 "tv.ag"
      glutIdleFunc(HandleIdle);
# 176 "tv.ag"
      glutVisibilityFunc(HandleVisibility);
# 177 "tv.ag"
      glutKeyboardFunc(HandleKeyboard);
# 178 "tv.ag"
      glutSpecialFunc(HandleSpecialKeyboard);
# 179 "tv.ag"
      glutMouseFunc(HandleMouse);
# 180 "tv.ag"
      glutReshapeFunc(HandleReshape);
# 181 "tv.ag"

# 182 "tv.ag"
      //  glutMotionFunc(HandleMouseMotion);
# 183 "tv.ag"

# 184 "tv.ag"
      /* Set up some menus */
# 185 "tv.ag"

# 186 "tv.ag"
      anglemenu = glutCreateMenu(HandleAngleMenu);
# 187 "tv.ag"
      glutAddMenuEntry("Weit (w)", 1);
# 188 "tv.ag"
      glutAddMenuEntry("Normal", 2);
# 189 "tv.ag"
      glutAddMenuEntry("Tele (t)", 3);
# 190 "tv.ag"

# 191 "tv.ag"
      speedmenu = glutCreateMenu(HandleSpeedMenu);
# 192 "tv.ag"
      glutAddMenuEntry("Halt (p)", 1);
# 193 "tv.ag"
      glutAddMenuEntry("Langsam (s)", 2);
# 194 "tv.ag"
      glutAddMenuEntry("Normal (n)", 3);
# 195 "tv.ag"
      glutAddMenuEntry("Schnell (f)", 4);
# 196 "tv.ag"

# 197 "tv.ag"
      mainmenu = glutCreateMenu(HandleMainMenu);
# 198 "tv.ag"
      glutAddSubMenu("Blickwinkel", anglemenu);
# 199 "tv.ag"
      glutAddSubMenu("Geschwindigkeit", speedmenu);
# 200 "tv.ag"
      glutAddMenuEntry("Startposition (Home)", 5);
# 201 "tv.ag"
      glutAddMenuEntry("Quit", 100);
# 202 "tv.ag"

# 203 "tv.ag"
      glutSetMenu(mainmenu);
# 204 "tv.ag"
      glutAttachMenu(GLUT_RIGHT_BUTTON);
# 205 "tv.ag"

# 206 "tv.ag"
      CreateEnvironment();
# 207 "tv.ag"

# 208 "tv.ag"
      // Kamera-Parameter in Grundposition
# 209 "tv.ag"
      Home();
# 210 "tv.ag"

# 211 "tv.ag"
      glutMainLoop();
# 212 "tv.ag"
      return 0;
# 213 "tv.ag"
    }
# 214 "tv.ag"
  catch (const char* msg)
# 215 "tv.ag"
    {
# 216 "tv.ag"
      cerr << "Exception: " << msg << endl;
# 217 "tv.ag"
    }
# 218 "tv.ag"
  catch (const string& msg)
# 219 "tv.ag"
    {
# 220 "tv.ag"
      cerr << "Exception: " << msg << endl;
# 221 "tv.ag"
    }
# 222 "tv.ag"
}
# 223 "tv.ag"
/*
# 224 "tv.ag"
   Globale Einstellungen
# 225 "tv.ag"
*/
# 226 "tv.ag"

# 227 "tv.ag"
void CreateEnvironment(void)
# 228 "tv.ag"
{
# 229 "tv.ag"
  glEnable(GL_DEPTH_TEST);
# 230 "tv.ag"

# 231 "tv.ag"
  glEnable(GL_LINE_SMOOTH);
# 232 "tv.ag"
  glEnable(GL_POINT_SMOOTH);
# 233 "tv.ag"
  glEnable(GL_POLYGON_SMOOTH);
# 234 "tv.ag"

# 235 "tv.ag"
  glShadeModel(GL_SMOOTH);
# 236 "tv.ag"
  glDisable(GL_DITHER);         /* Assume RGBA capabilities */
# 237 "tv.ag"

# 238 "tv.ag"
  glLineWidth(1.0);
# 239 "tv.ag"
  glPointSize(1.0);
# 240 "tv.ag"
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
# 241 "tv.ag"

# 242 "tv.ag"
  glFrontFace(GL_CCW);
# 243 "tv.ag"
  glEnable(GL_DEPTH_TEST);
# 244 "tv.ag"
  glDisable(GL_CULL_FACE);
# 245 "tv.ag"

# 246 "tv.ag"
  // glEnable(GL_CULL_FACE); blendet eine Seite von Flächen aus
# 247 "tv.ag"
  // glCullFace(GL_BACK);
# 248 "tv.ag"

# 249 "tv.ag"
  glClearColor(0.0, 0.0, 0.0, 0.0);      /* Background colour */
# 250 "tv.ag"

# 251 "tv.ag"
  glEnable(GL_COLOR_MATERIAL);
# 252 "tv.ag"
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
# 253 "tv.ag"
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, null);
# 254 "tv.ag"
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, null);
# 255 "tv.ag"
}
# 256 "tv.ag"

# 257 "tv.ag"
/*
# 258 "tv.ag"
   This is the basic display callback routine
# 259 "tv.ag"
   It creates the geometry, lighting, and viewing position
# 260 "tv.ag"
*/
# 261 "tv.ag"

# 262 "tv.ag"
void Display(void)
# 263 "tv.ag"
{
# 264 "tv.ag"
//    cout << "display" << Time << endl;
# 265 "tv.ag"
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
# 266 "tv.ag"

# 267 "tv.ag"
  MakeCamera();
# 268 "tv.ag"
  MakeLighting();
# 269 "tv.ag"
  MakeGeometry();
# 270 "tv.ag"

# 271 "tv.ag"
  glutSwapBuffers();
# 272 "tv.ag"
}
# 273 "tv.ag"

# 274 "tv.ag"
/*
# 275 "tv.ag"
   Create the geometry
# 276 "tv.ag"
*/
# 277 "tv.ag"
void MakeGeometry(void)
# 278 "tv.ag"
{
# 279 "tv.ag"
//    cout << "geometrie" << endl;
# 280 "tv.ag"
  for (auto& t : world)
# 281 "tv.ag"
    t.draw();
# 282 "tv.ag"
}
# 283 "tv.ag"

# 284 "tv.ag"
/*
# 285 "tv.ag"
   Set up the lighting environment
# 286 "tv.ag"
*/
# 287 "tv.ag"
void MakeLighting(void)
# 288 "tv.ag"
{
# 289 "tv.ag"
  /* Set ambient globally, default ambient for light sources is 0 */
# 290 "tv.ag"
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalambient);
# 291 "tv.ag"

# 292 "tv.ag"
  if (light_number > 0)
# 293 "tv.ag"
    {
# 294 "tv.ag"
      glLightfv(GL_LIGHT0, GL_POSITION, pos0);
# 295 "tv.ag"
      glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
# 296 "tv.ag"
      glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
# 297 "tv.ag"
      glLightfv(GL_LIGHT0, GL_SPECULAR, dif0);
# 298 "tv.ag"
      glEnable(GL_LIGHT0);
# 299 "tv.ag"
    }
# 300 "tv.ag"

# 301 "tv.ag"
  if (light_number > 1)
# 302 "tv.ag"
    {
# 303 "tv.ag"
      glLightfv(GL_LIGHT1, GL_POSITION, pos1);
# 304 "tv.ag"
      glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
# 305 "tv.ag"
      glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
# 306 "tv.ag"
      glLightfv(GL_LIGHT1, GL_SPECULAR, dif1);
# 307 "tv.ag"
      glEnable(GL_LIGHT1);
# 308 "tv.ag"
    }
# 309 "tv.ag"

# 310 "tv.ag"
  glEnable(GL_LIGHTING);
# 311 "tv.ag"
}
# 312 "tv.ag"

# 313 "tv.ag"
/*
# 314 "tv.ag"
   Set up the camera
# 315 "tv.ag"
*/
# 316 "tv.ag"

# 317 "tv.ag"
void Home()
# 318 "tv.ag"
{
# 319 "tv.ag"
  move_step = cameraDistance / 20;
# 320 "tv.ag"
  switch (cammode)
# 321 "tv.ag"
    {
# 322 "tv.ag"
    case 1: // no break
# 323 "tv.ag"
    case 3:
# 324 "tv.ag"
      camdistance = cameraDistance;
# 325 "tv.ag"
      leftrightrotate = 80;
# 326 "tv.ag"
      updownrotate = 70;
# 327 "tv.ag"
      camangle = 40;
# 328 "tv.ag"
      break;
# 329 "tv.ag"

# 330 "tv.ag"
    case 2:
# 331 "tv.ag"
      glMatrixMode(GL_PROJECTION);
# 332 "tv.ag"
      glLoadIdentity();
# 333 "tv.ag"
      gluPerspective(camangle,     /* Field of view */
# 334 "tv.ag"
      window_ratio, /* aspect ratio  */
# 335 "tv.ag"
      0.1, 10000.0); /* near and far  */
# 336 "tv.ag"
      glMatrixMode(GL_MODELVIEW);
# 337 "tv.ag"
      glLoadIdentity();
# 338 "tv.ag"
      // initiale Position wie bei 1
# 339 "tv.ag"
      gluLookAt(cameraDistance * Cos(80) * Sin(70),
# 340 "tv.ag"
      cameraDistance * Cos(70),
# 341 "tv.ag"
      cameraDistance * Sin(80) * Sin(70),
# 342 "tv.ag"
      0.0, 0.0, 0.0,                                 /* Center   */
# 343 "tv.ag"
      0.0, 1.0, 0.0);                                /* Up       */
# 344 "tv.ag"
      break;
# 345 "tv.ag"
    }
# 346 "tv.ag"
}
# 347 "tv.ag"

# 348 "tv.ag"
void MakeCamera()
# 349 "tv.ag"
{
# 350 "tv.ag"
  /* Camera setup */
# 351 "tv.ag"
  glMatrixMode(GL_PROJECTION);
# 352 "tv.ag"
  glLoadIdentity();
# 353 "tv.ag"
  gluPerspective(camangle,     /* Field of view */
# 354 "tv.ag"
  window_ratio, /* aspect ratio  */
# 355 "tv.ag"
  0.1, 10000.0); /* near and far  */
# 356 "tv.ag"
  glMatrixMode(GL_MODELVIEW);
# 357 "tv.ag"

# 358 "tv.ag"
  if (cammode != 2)
# 359 "tv.ag"
    {
# 360 "tv.ag"
      glLoadIdentity();
# 361 "tv.ag"

# 362 "tv.ag"
      switch (cammode)
# 363 "tv.ag"
        {
# 364 "tv.ag"
        case 1:
# 365 "tv.ag"
        case 3:
# 366 "tv.ag"
          gluLookAt(camdistance * Cos(leftrightrotate)*Sin(updownrotate),
# 367 "tv.ag"
          camdistance * Cos(updownrotate),
# 368 "tv.ag"
          camdistance * Sin(leftrightrotate)*Sin(updownrotate),
# 369 "tv.ag"
          0.0, 0.0, 0.0,                                 /* Center   */
# 370 "tv.ag"
          0.0, 1.0, 0.0);                                /* Up       */
# 371 "tv.ag"
          break;
# 372 "tv.ag"
        }
# 373 "tv.ag"
    }
# 374 "tv.ag"
}
# 375 "tv.ag"

# 376 "tv.ag"
/*
# 377 "tv.ag"
  Tastaturbehandlung
# 378 "tv.ag"
*/
# 379 "tv.ag"
void HandleKeyboard(unsigned char key, int x, int y)
# 380 "tv.ag"
{
# 381 "tv.ag"
  int modifier = glutGetModifiers();
# 382 "tv.ag"
  switch (modifier)
# 383 "tv.ag"
    {
# 384 "tv.ag"
    case 0:
# 385 "tv.ag"
      switch (key)
# 386 "tv.ag"
        {
# 387 "tv.ag"
        case 27: /* ESC */
# 388 "tv.ag"
        case 'h':
# 389 "tv.ag"
        case 'H':
# 390 "tv.ag"
          Home();
# 391 "tv.ag"
          break;
# 392 "tv.ag"
        case 'Q':
# 393 "tv.ag"
        case 'q':
# 394 "tv.ag"
          exit(0);
# 395 "tv.ag"
          break;
# 396 "tv.ag"
        case 'P':
# 397 "tv.ag"
        case 'p':
# 398 "tv.ag"
          timestat = tm_halt;
# 399 "tv.ag"
          break;
# 400 "tv.ag"
        case 'S':
# 401 "tv.ag"
        case 's':
# 402 "tv.ag"
          timestat = tm_slow;
# 403 "tv.ag"
          break;
# 404 "tv.ag"
        case 'N':
# 405 "tv.ag"
        case 'n':
# 406 "tv.ag"
          timestat = tm_normal;
# 407 "tv.ag"
          break;
# 408 "tv.ag"
        case 'F':
# 409 "tv.ag"
        case 'f':
# 410 "tv.ag"
          timestat = tm_fast;
# 411 "tv.ag"
          break;
# 412 "tv.ag"
        case 'w':
# 413 "tv.ag"
        case 'W':
# 414 "tv.ag"
          camangle = 80;
# 415 "tv.ag"
          break;
# 416 "tv.ag"
        case 't':
# 417 "tv.ag"
        case 'T':
# 418 "tv.ag"
          camangle = 20;
# 419 "tv.ag"
          break;
# 420 "tv.ag"
          // Zoom
# 421 "tv.ag"
        case '-':
# 422 "tv.ag"
          if (camangle < 170) camangle++;
# 423 "tv.ag"
          break;
# 424 "tv.ag"
        case '+':
# 425 "tv.ag"
          if (camangle > 1) camangle--;
# 426 "tv.ag"
          break;
# 427 "tv.ag"

# 428 "tv.ag"
        case 'M':
# 429 "tv.ag"
          Trafo tr;
# 430 "tv.ag"
          tr.glGet();
# 431 "tv.ag"
          //  cout << tr.toString() << endl;
# 432 "tv.ag"
          //  cout << "------------" << endl;
# 433 "tv.ag"

# 434 "tv.ag"
          GLfloat m[16];
# 435 "tv.ag"
          glGetFloatv(GL_PROJECTION_MATRIX, m);
# 436 "tv.ag"
          cout << endl;
# 437 "tv.ag"
          for (int i = 0; i < 4; i++)
# 438 "tv.ag"
            {
# 439 "tv.ag"
              for (int j = 0; j < 4; j++)
# 440 "tv.ag"
                cout << setw(10) << setprecision(5) << m[j * 4 + i] << "\t";
# 441 "tv.ag"
              cout << endl;
# 442 "tv.ag"
            }
# 443 "tv.ag"

# 444 "tv.ag"
          break;
# 445 "tv.ag"
        }
# 446 "tv.ag"
      break;
# 447 "tv.ag"

# 448 "tv.ag"
    case GLUT_ACTIVE_CTRL:
# 449 "tv.ag"
      switch (key)
# 450 "tv.ag"
        {
# 451 "tv.ag"
        case '+':
# 452 "tv.ag"
          move_step *= 2;
# 453 "tv.ag"
          cout << "Step: " << move_step << endl;
# 454 "tv.ag"
          break;
# 455 "tv.ag"
        case '-':
# 456 "tv.ag"
          move_step *= 0.5;
# 457 "tv.ag"
          cout << "Step: " << move_step << endl;
# 458 "tv.ag"
          break;
# 459 "tv.ag"
        }
# 460 "tv.ag"
      break;
# 461 "tv.ag"
    }
# 462 "tv.ag"
}
# 463 "tv.ag"

# 464 "tv.ag"
/*
# 465 "tv.ag"
  Spezielle Tasten
# 466 "tv.ag"
*/
# 467 "tv.ag"
void CamControl1(int key, int modifier)
# 468 "tv.ag"
{
# 469 "tv.ag"
  switch (key)
# 470 "tv.ag"
    {
# 471 "tv.ag"
    case GLUT_KEY_LEFT:
# 472 "tv.ag"
      leftrightrotate += 3;
# 473 "tv.ag"
      break;
# 474 "tv.ag"
    case GLUT_KEY_RIGHT:
# 475 "tv.ag"
      leftrightrotate -= 3;
# 476 "tv.ag"
      break;
# 477 "tv.ag"
    case GLUT_KEY_UP:
# 478 "tv.ag"
      updownrotate -= 2;
# 479 "tv.ag"
      break;
# 480 "tv.ag"
    case GLUT_KEY_DOWN:
# 481 "tv.ag"
      updownrotate += 2;
# 482 "tv.ag"
      break;
# 483 "tv.ag"
    case GLUT_KEY_PAGE_UP:
# 484 "tv.ag"
      camdistance *= 0.8;
# 485 "tv.ag"
      break;
# 486 "tv.ag"
    case GLUT_KEY_PAGE_DOWN:
# 487 "tv.ag"
      camdistance *= 1.2;
# 488 "tv.ag"
      break;
# 489 "tv.ag"
    case GLUT_KEY_HOME:
# 490 "tv.ag"
      Home();
# 491 "tv.ag"
      break;
# 492 "tv.ag"
    }
# 493 "tv.ag"
  if (updownrotate > 179.9) updownrotate = 179.9;
# 494 "tv.ag"
  if (updownrotate < 0.1) updownrotate = 0.1;
# 495 "tv.ag"
}
# 496 "tv.ag"

# 497 "tv.ag"
void CamControl3(int key, int modifier)
# 498 "tv.ag"
{
# 499 "tv.ag"
  switch (key)
# 500 "tv.ag"
    {
# 501 "tv.ag"
    case GLUT_KEY_LEFT:
# 502 "tv.ag"
      rspeed -= 10;
# 503 "tv.ag"
      break;
# 504 "tv.ag"
    case GLUT_KEY_RIGHT:
# 505 "tv.ag"
      rspeed += 10;
# 506 "tv.ag"
      break;
# 507 "tv.ag"
    case GLUT_KEY_UP:
# 508 "tv.ag"
      updownrotate -= 2;
# 509 "tv.ag"
      break;
# 510 "tv.ag"
    case GLUT_KEY_DOWN:
# 511 "tv.ag"
      updownrotate += 2;
# 512 "tv.ag"
      break;
# 513 "tv.ag"
    case GLUT_KEY_PAGE_UP:
# 514 "tv.ag"
      camdistance *= 0.8;
# 515 "tv.ag"
      break;
# 516 "tv.ag"
    case GLUT_KEY_PAGE_DOWN:
# 517 "tv.ag"
      camdistance *= 1.2;
# 518 "tv.ag"
      break;
# 519 "tv.ag"
    case GLUT_KEY_HOME:
# 520 "tv.ag"
      Home();
# 521 "tv.ag"
      break;
# 522 "tv.ag"
    }
# 523 "tv.ag"
  if (updownrotate > 179.9) updownrotate = 179.9;
# 524 "tv.ag"
  if (updownrotate < 0.1) updownrotate = 0.1;
# 525 "tv.ag"
}
# 526 "tv.ag"

# 527 "tv.ag"
void Cam4RotLeft(float val)
# 528 "tv.ag"
{
# 529 "tv.ag"
  GLfloat m[16];
# 530 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 531 "tv.ag"
  glLoadIdentity();
# 532 "tv.ag"
  glRotated(val, 0, -1, 0);
# 533 "tv.ag"
  glMultMatrixf(m);
# 534 "tv.ag"
}
# 535 "tv.ag"

# 536 "tv.ag"
void Cam4RotUp(float val)
# 537 "tv.ag"
{
# 538 "tv.ag"
  GLfloat m[16];
# 539 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 540 "tv.ag"
  glLoadIdentity();
# 541 "tv.ag"
  glRotated(val, -1, 0, 0);
# 542 "tv.ag"
  glMultMatrixf(m);
# 543 "tv.ag"
}
# 544 "tv.ag"

# 545 "tv.ag"
void Cam4Roll(float val)
# 546 "tv.ag"
{
# 547 "tv.ag"
  GLfloat m[16];
# 548 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 549 "tv.ag"
  glLoadIdentity();
# 550 "tv.ag"
  glRotated(val, 0, 0, 1);
# 551 "tv.ag"
  glMultMatrixf(m);
# 552 "tv.ag"
}
# 553 "tv.ag"

# 554 "tv.ag"
void Cam4MoveUp(float val)
# 555 "tv.ag"
{
# 556 "tv.ag"
  GLfloat m[16];
# 557 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 558 "tv.ag"
  glLoadIdentity();
# 559 "tv.ag"
  glTranslated(0, -val, 0);
# 560 "tv.ag"
  glMultMatrixf(m);
# 561 "tv.ag"
}
# 562 "tv.ag"

# 563 "tv.ag"
void Cam4MoveLeft(float val)
# 564 "tv.ag"
{
# 565 "tv.ag"
  GLfloat m[16];
# 566 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 567 "tv.ag"
  glLoadIdentity();
# 568 "tv.ag"
  glTranslated(val, 0, 0);
# 569 "tv.ag"
  glMultMatrixf(m);
# 570 "tv.ag"
}
# 571 "tv.ag"

# 572 "tv.ag"
void Cam4MoveForward(float val)
# 573 "tv.ag"
{
# 574 "tv.ag"
  GLfloat m[16];
# 575 "tv.ag"
  glGetFloatv(GL_MODELVIEW_MATRIX, m);
# 576 "tv.ag"
  glLoadIdentity();
# 577 "tv.ag"
  glTranslated(0, 0, val);
# 578 "tv.ag"
  glMultMatrixf(m);
# 579 "tv.ag"
}
# 580 "tv.ag"

# 581 "tv.ag"
void CamControl2(int key, int modifier)
# 582 "tv.ag"
{
# 583 "tv.ag"
  switch (modifier)
# 584 "tv.ag"
    {
# 585 "tv.ag"
    case 0:
# 586 "tv.ag"

# 587 "tv.ag"
      switch (key)
# 588 "tv.ag"
        {
# 589 "tv.ag"
        case GLUT_KEY_LEFT:
# 590 "tv.ag"
          Cam4RotLeft(angle_step);
# 591 "tv.ag"
          break;
# 592 "tv.ag"
        case GLUT_KEY_RIGHT:
# 593 "tv.ag"
          Cam4RotLeft(-angle_step);
# 594 "tv.ag"
          break;
# 595 "tv.ag"

# 596 "tv.ag"
        case GLUT_KEY_UP:
# 597 "tv.ag"
          Cam4RotUp(angle_step);
# 598 "tv.ag"
          break;
# 599 "tv.ag"
        case GLUT_KEY_DOWN:
# 600 "tv.ag"
          Cam4RotUp(-angle_step);
# 601 "tv.ag"
          break;
# 602 "tv.ag"

# 603 "tv.ag"
        case GLUT_KEY_PAGE_UP:
# 604 "tv.ag"
          Cam4MoveUp(move_step);
# 605 "tv.ag"
          break;
# 606 "tv.ag"
        case GLUT_KEY_PAGE_DOWN:
# 607 "tv.ag"
          Cam4MoveUp(-move_step);
# 608 "tv.ag"
          break;
# 609 "tv.ag"

# 610 "tv.ag"
        case GLUT_KEY_HOME:
# 611 "tv.ag"
          Home();
# 612 "tv.ag"
          break;
# 613 "tv.ag"
        }
# 614 "tv.ag"
      break;
# 615 "tv.ag"

# 616 "tv.ag"
    case GLUT_ACTIVE_CTRL:
# 617 "tv.ag"

# 618 "tv.ag"
      switch (key)
# 619 "tv.ag"
        {
# 620 "tv.ag"
        case GLUT_KEY_LEFT:
# 621 "tv.ag"
          Cam4MoveLeft(move_step);
# 622 "tv.ag"
          break;
# 623 "tv.ag"
        case GLUT_KEY_RIGHT:
# 624 "tv.ag"
          Cam4MoveLeft(-move_step);
# 625 "tv.ag"
          break;
# 626 "tv.ag"

# 627 "tv.ag"
        case GLUT_KEY_UP:
# 628 "tv.ag"
          Cam4MoveForward(move_step);
# 629 "tv.ag"
          break;
# 630 "tv.ag"
        case GLUT_KEY_DOWN:
# 631 "tv.ag"
          Cam4MoveForward(-move_step);
# 632 "tv.ag"
          break;
# 633 "tv.ag"

# 634 "tv.ag"
        case GLUT_KEY_PAGE_UP:
# 635 "tv.ag"
          Cam4MoveUp(move_step);
# 636 "tv.ag"
          break;
# 637 "tv.ag"
        case GLUT_KEY_PAGE_DOWN:
# 638 "tv.ag"
          Cam4MoveUp(-move_step);
# 639 "tv.ag"
          break;
# 640 "tv.ag"

# 641 "tv.ag"
        case '+':
# 642 "tv.ag"
          move_step *= 2;
# 643 "tv.ag"
          break;
# 644 "tv.ag"
        case '-':
# 645 "tv.ag"
          move_step *= 0.5;
# 646 "tv.ag"
          break;
# 647 "tv.ag"
        }
# 648 "tv.ag"
      break;
# 649 "tv.ag"

# 650 "tv.ag"
    case GLUT_ACTIVE_SHIFT:
# 651 "tv.ag"
      switch (key)
# 652 "tv.ag"
        {
# 653 "tv.ag"
        case GLUT_KEY_LEFT:
# 654 "tv.ag"
          Cam4Roll(2);
# 655 "tv.ag"
          break;
# 656 "tv.ag"
        case GLUT_KEY_RIGHT:
# 657 "tv.ag"
          Cam4Roll(-2);
# 658 "tv.ag"
          break;
# 659 "tv.ag"
        }
# 660 "tv.ag"

# 661 "tv.ag"
      break;
# 662 "tv.ag"
    }
# 663 "tv.ag"
}
# 664 "tv.ag"

# 665 "tv.ag"
void HandleSpecialKeyboard(int key, int x, int y)
# 666 "tv.ag"
{
# 667 "tv.ag"
  int modifier = glutGetModifiers();
# 668 "tv.ag"
  // Masken GLUT_ACTIVE_SHIFT, GLUT_ACTIVE_CTRL, GLUT_ACTIVE_ALT
# 669 "tv.ag"
  switch (cammode)
# 670 "tv.ag"
    {
# 671 "tv.ag"
    case 1:
# 672 "tv.ag"
      CamControl1(key, modifier);
# 673 "tv.ag"
      break;
# 674 "tv.ag"
    case 2:
# 675 "tv.ag"
      CamControl2(key, modifier);
# 676 "tv.ag"
      break;
# 677 "tv.ag"
    case 3:
# 678 "tv.ag"
      CamControl3(key, modifier);
# 679 "tv.ag"
      break;
# 680 "tv.ag"
    }
# 681 "tv.ag"
}
# 682 "tv.ag"

# 683 "tv.ag"
/*
# 684 "tv.ag"
   Handle mouse events
# 685 "tv.ag"
*/
# 686 "tv.ag"
void HandleMouse(int button, int state, int x, int y)
# 687 "tv.ag"
{
# 688 "tv.ag"
  if (state == GLUT_DOWN)
# 689 "tv.ag"
    {
# 690 "tv.ag"
    }
# 691 "tv.ag"
}
# 692 "tv.ag"

# 693 "tv.ag"
void HandleReshape(int width, int height)
# 694 "tv.ag"
{
# 695 "tv.ag"
  window_width = width;
# 696 "tv.ag"
  window_height = height;
# 697 "tv.ag"
  window_ratio = ((float)window_width) / ((float)window_height);
# 698 "tv.ag"
  glViewport(0, 0, width, height);
# 699 "tv.ag"
}
# 700 "tv.ag"
/*
# 701 "tv.ag"
   Handle the main menu
# 702 "tv.ag"
*/
# 703 "tv.ag"
void HandleMainMenu(int whichone)
# 704 "tv.ag"
{
# 705 "tv.ag"
  switch (whichone)
# 706 "tv.ag"
    {
# 707 "tv.ag"
    case 1:
# 708 "tv.ag"
      timestat = tm_halt;
# 709 "tv.ag"
      break;
# 710 "tv.ag"
    case 2:
# 711 "tv.ag"
      timestat = tm_slow;
# 712 "tv.ag"
      break;
# 713 "tv.ag"
    case 3:
# 714 "tv.ag"
      timestat = tm_normal;
# 715 "tv.ag"
      break;
# 716 "tv.ag"
    case 5:
# 717 "tv.ag"
      Home();
# 718 "tv.ag"
      break;
# 719 "tv.ag"
    case 100:
# 720 "tv.ag"
      exit(0);
# 721 "tv.ag"
      break;
# 722 "tv.ag"
    }
# 723 "tv.ag"
}
# 724 "tv.ag"

# 725 "tv.ag"
/*
# 726 "tv.ag"
  BlickWinkelMenu
# 727 "tv.ag"
*/
# 728 "tv.ag"
void HandleAngleMenu(int whichone)
# 729 "tv.ag"
{
# 730 "tv.ag"
  switch (whichone)
# 731 "tv.ag"
    {
# 732 "tv.ag"
    case 1:
# 733 "tv.ag"
      camangle = 80;
# 734 "tv.ag"
      break;
# 735 "tv.ag"
    case 2:
# 736 "tv.ag"
      camangle = 40;
# 737 "tv.ag"
      break;
# 738 "tv.ag"
    case 3:
# 739 "tv.ag"
      camangle = 20;
# 740 "tv.ag"
      break;
# 741 "tv.ag"
    }
# 742 "tv.ag"
}
# 743 "tv.ag"

# 744 "tv.ag"
/*
# 745 "tv.ag"
  Geschwindigkeitsmenu
# 746 "tv.ag"
*/
# 747 "tv.ag"
void HandleSpeedMenu(int whichone)
# 748 "tv.ag"
{
# 749 "tv.ag"
  switch (whichone)
# 750 "tv.ag"
    {
# 751 "tv.ag"
    case 1:
# 752 "tv.ag"
      timestat = tm_halt;
# 753 "tv.ag"
      break;
# 754 "tv.ag"
    case 2:
# 755 "tv.ag"
      timestat = tm_slow;
# 756 "tv.ag"
      break;
# 757 "tv.ag"
    case 3:
# 758 "tv.ag"
      timestat = tm_normal;
# 759 "tv.ag"
      break;
# 760 "tv.ag"
    case 4:
# 761 "tv.ag"
      timestat = tm_fast;
# 762 "tv.ag"
      break;
# 763 "tv.ag"
    }
# 764 "tv.ag"
}
# 765 "tv.ag"

# 766 "tv.ag"
/*
# 767 "tv.ag"
   How to handle visibility
# 768 "tv.ag"
*/
# 769 "tv.ag"
void HandleVisibility(int gvisible)
# 770 "tv.ag"
{
# 771 "tv.ag"
//   cout << "visibility " << (visible==GLUT_VISIBLE) << endl;
# 772 "tv.ag"
  if (gvisible == GLUT_VISIBLE)
# 773 "tv.ag"
    visible = true;
# 774 "tv.ag"
  else
# 775 "tv.ag"
    visible = false;
# 776 "tv.ag"
}
# 777 "tv.ag"

# 778 "tv.ag"
void HandleIdle()
# 779 "tv.ag"
{
# 780 "tv.ag"
  double atime;
# 781 "tv.ag"
  static double lasttime = -1;
# 782 "tv.ag"

# 783 "tv.ag"
#ifndef WIN32
# 784 "tv.ag"
  struct timeval tt;
# 785 "tv.ag"
  struct timezone tz;
# 786 "tv.ag"

# 787 "tv.ag"
  gettimeofday(&tt, &tz);
# 788 "tv.ag"
  atime = tt.tv_sec + (double)tt.tv_usec / 1000000.0;
# 789 "tv.ag"
#else
# 790 "tv.ag"
  struct timeb tt;
# 791 "tv.ag"
  ftime(&tt);
# 792 "tv.ag"
  atime = tt.time + (double)tt.millitm / 1000;
# 793 "tv.ag"
#endif
# 794 "tv.ag"
  double dt;
# 795 "tv.ag"
  if (lasttime < 0)
# 796 "tv.ag"
    dt = 0;
# 797 "tv.ag"
  else
# 798 "tv.ag"
    dt = atime - lasttime;
# 799 "tv.ag"
  //  if (dt > 1)
# 800 "tv.ag"
  //    dt = 1.0;
# 801 "tv.ag"
  lasttime = atime;
# 802 "tv.ag"

# 803 "tv.ag"
  if (benchmarking)
# 804 "tv.ag"
    {
# 805 "tv.ag"
      RealTime += dt;
# 806 "tv.ag"

# 807 "tv.ag"
      if (RealTime < 10)
# 808 "tv.ag"
        frametime = dt;
# 809 "tv.ag"
      else
# 810 "tv.ag"
        frametime = frametime * 0.999 + 0.001 * dt;
# 811 "tv.ag"

# 812 "tv.ag"
      if (RealTime > 1)
# 813 "tv.ag"
        cout << 1.0 / frametime << " fps " << endl;
# 814 "tv.ag"
    }
# 815 "tv.ag"

# 816 "tv.ag"
  switch (timestat)
# 817 "tv.ag"
    {
# 818 "tv.ag"
    case tm_halt:
# 819 "tv.ag"
      break;
# 820 "tv.ag"
    case tm_slow:
# 821 "tv.ag"
      VirtualTime += 0.2 * dt;
# 822 "tv.ag"
      break;
# 823 "tv.ag"
    case tm_normal:
# 824 "tv.ag"
      VirtualTime += dt;
# 825 "tv.ag"
      break;
# 826 "tv.ag"
    case tm_fast:
# 827 "tv.ag"
      VirtualTime += 5 * dt;
# 828 "tv.ag"
      break;
# 829 "tv.ag"
    }
# 830 "tv.ag"

# 831 "tv.ag"
  if (cammode == 3)
# 832 "tv.ag"
    leftrightrotate += dt * rspeed;
# 833 "tv.ag"

# 834 "tv.ag"
  if (visible)
# 835 "tv.ag"
    {
# 836 "tv.ag"
      if (!benchmarking)
# 837 "tv.ag"
        usleep(20000);
# 838 "tv.ag"
      glutPostRedisplay();
# 839 "tv.ag"
    }
# 840 "tv.ag"
}
# 841 "tv.ag"

# 842 "tv.ag"
/*
# 843 "tv.ag"
  Draw text in the x-y plane
# 844 "tv.ag"
  The x,y,z coordinate is the bottom left corner (looking down -ve z axis)
# 845 "tv.ag"
*/
# 846 "tv.ag"

# 847 "tv.ag"
void DrawTextXY(float x, float y, float z, float scale, char* s)
# 848 "tv.ag"
{
# 849 "tv.ag"
  unsigned int i;
# 850 "tv.ag"

# 851 "tv.ag"
  glPushMatrix();
# 852 "tv.ag"
  glTranslated(x, y, z);
# 853 "tv.ag"
  glScaled(scale, scale, scale);
# 854 "tv.ag"
  for (i = 0; i < strlen(s); i++)
# 855 "tv.ag"
    glutStrokeCharacter(GLUT_STROKE_ROMAN, s[i]);
# 856 "tv.ag"
  glPopMatrix();
# 857 "tv.ag"
}
# 858 "tv.ag"

# 859 "tv.ag"
/*
# 860 "tv.ag"
   Display the program usage information
# 861 "tv.ag"
*/
# 862 "tv.ag"
void Usage(const string& cmd, bool tasten)
# 863 "tv.ag"
{
# 864 "tv.ag"
  fprintf(stderr, "Anwendung:    %s <optionen> [<filename>]\n", cmd.c_str());
# 865 "tv.ag"
  fprintf(stderr, "  -h Hilfe\n");
# 866 "tv.ag"
  fprintf(stderr, "  -f <filename> Welt (welt.cv)\n");
# 867 "tv.ag"
  fprintf(stderr, "  -c <n> Kameramodell\n");
# 868 "tv.ag"
  fprintf(stderr, "     1 Kamera kann um Objekt (Ursprung) bewegt werden\n");
# 869 "tv.ag"
  fprintf(stderr, "     2 Freie Kamerabewegung in der Welt\n");
# 870 "tv.ag"
  fprintf(stderr, "     3 Kamera bewegt sich um Objekt (Ursprung) herum\n");
# 871 "tv.ag"
  fprintf(stderr, "  -d Skript-Debugging\n");
# 872 "tv.ag"

# 873 "tv.ag"
  fprintf(stderr, "  -t <time> Zeitvorgabe (stoppt Bewegungen)\n");
# 874 "tv.ag"
  fprintf(stderr, "  -s langsamer Zeitablauf\n");
# 875 "tv.ag"
  fprintf(stderr, "  -b Benchmark-Modus (Ausgabe von Frame-Raten\n");
# 876 "tv.ag"

# 877 "tv.ag"
  fprintf(stderr, "  -x <width>  Breite des Bildfensters\n");
# 878 "tv.ag"
  fprintf(stderr, "  -y <height> Höhe des Bildfensters\n");
# 879 "tv.ag"

# 880 "tv.ag"
  fprintf(stderr, "  -p Nur Parsen\n");
# 881 "tv.ag"
  fprintf(stderr, "  -o Optimierung durchführen\n");
# 882 "tv.ag"
  fprintf(stderr, "  -m <nr> minimale Segmentanzahl runder Körper\n");
# 883 "tv.ag"
  if (tasten)
# 884 "tv.ag"
    {
# 885 "tv.ag"
      fprintf(stderr, "\nTasten:\n");
# 886 "tv.ag"
      fprintf(stderr, "               q - Beenden\n");
# 887 "tv.ag"
      fprintf(stderr, "               p - Pause\n");
# 888 "tv.ag"
      fprintf(stderr, "               s - langsam\n");
# 889 "tv.ag"
      fprintf(stderr, "               n - normal\n");
# 890 "tv.ag"
      fprintf(stderr, "               f - schnell\n");
# 891 "tv.ag"
      fprintf(stderr, "Pfeiltasten: Bewegung bzw. Drehung (Mode-Abhängig)\n");
# 892 "tv.ag"
    }
# 893 "tv.ag"
  exit(-1);
# 894 "tv.ag"
}
# 895 "tv.ag"

# 896 "tv.ag"
void Usage(const string& cmd, const char* error)
# 897 "tv.ag"
{
# 898 "tv.ag"
  fprintf(stderr, "\n!!! %s !!!\n\n", error);
# 899 "tv.ag"
  Usage(cmd, false);
# 900 "tv.ag"
}

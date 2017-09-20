#include <unistd.h>
#include <geo.h>
#include <Distortion.h>
#include <Distortion1.h>
#include <picio.h>
#include <lseg.h>
#include <visual/screen.h>
#include <visual/visual.h>
#include <Camera.h>
#include <filter.h>
#include <selfcalib.h>

#include <string>
#include <vector>
#include <fstream>

#include "filelist.h"

#include "pattern.inc"

#include "fc_references.h"
#include "exifsig.h"

#include "fcalib.h"

using namespace std;
using namespace ice;

int wait = 0;

int Verbose = 0;

bool display = false;

const int exif_file = 1;
const int exif_ignore = 2;
const int exif_user = 3;

int exifmode = exif_file;

void NormalizeDistortion(double &x0, double &y0, double &d2, double &d4, int dimx, int dimy)
{
  double refsize = Max(dimx, dimy) / 2.0;
  double refsize2 = refsize * refsize;
  double refsize4 = refsize2 * refsize2;

  x0 = x0 / dimx;
  y0 = y0 / dimy;
  d2 = d2 * refsize2;
  d4 = d4 * refsize4;
}

void Print(const Distortion1 &distortion, int dimx, int dimy)
{
  double x0 = distortion.X0();
  double y0 = distortion.Y0();
  double d2 = distortion.D2();
  double d4 = distortion.D4();
  cout << "Verzeichnung              ";
  cout << "x0: " << x0 << "  y0: " << y0;
  cout << "  d2: " << d2 << "  d4: " << d4 << endl;

  NormalizeDistortion(x0, y0, d2, d4, dimx, dimy);

  cout << "Verzeichnung normalisiert " ;
  cout << "x0: " << x0 << "  y0: " << y0;
  cout << "  d2: " << d2 << "  d4: " << d4 << endl << endl;
}

void Print(const Matrix &m, int dimx, int dimy)
{
  cout << "Bildgröße:     " << dimx << " x " << dimy << endl;
  double f = m[0][0];
  cout << "Brennweite  f: " << f << endl;
  cout << "Anisotropie a: " << m[1][1] / m[0][0] << endl;
  cout << "Scherung    s: " << m[0][1] / m[0][0] << endl;
  cout << "Hauptpunkt u0: " << m[0][2] << endl;
  cout << "           v0: " << m[1][2] << endl << endl;

  cout << "Brennweite (KB äquivalent): " << f * 36 / dimx << " mm" <<  endl;
  cout << "Bildwinkel:" << endl;
  cout << Degree(atan2(dimx / 2, f) * 2) << "° (x)" << endl;
  cout << Degree(atan2(dimy / 2, f) * 2) << "° (y)" << endl;
  cout << Degree(atan2(sqrt(dimx * dimx + dimy * dimy) / 2, f) * 2) << "° (diag)" << endl;
}

void usage(const string &pname)
{
  cout << "Kalibrierung mit 2D-Muster" << endl << endl;
  cout << "Aufruf" << "  " << pname << " [options] <filename> .." << endl << endl;
  //  cout << "Es sind mindestens 3 verwertbare Bilder des Kalibriermusters erforderlich." << endl << endl;
  cout << "Optionen:" << endl;

  cout << "-s      Mittelwert-Filterung der Bilddaten in 5 x 5-Umgebung" << endl;
  cout << "-S n    Mittelwert-Filterung der Bilddaten in n x n-Umgebung" << endl;

  cout << "-l n    Setze Segmentierungsschwelle auf n (default:55)" << endl;
  cout << "-L n    Setze Segmentierungs-Umgebungsgröße auf n (default:15)" << endl;
  cout << "-m n    Minimal notwendige Anzahl gefundener Marker im Bild (default:400)" << endl;

  cout << "-d      Anzeige der Bilder bei der Berechnung" << endl;

  cout << "-o c    Auswahl der Ausgabe:" << endl;
  cout << "         c='d'  Ausgabe der Verzeichnung" << endl;
  cout << "         c='i'  Ausgabe der internen Kamera-Parameter" << endl;
  cout << "         c='D'  Eintrag der Verzeichnung in die Datenbank (dist.txt)" << endl;
  cout << "        Diese Option kann mehrfach angewendet werden und wirkt kumulativ" << endl;

  cout << "-e      EXIF-Daten nicht verwenden" << endl;
  cout << "-E str  String als Kennung (statt EXIF-Daten) verwenden" << endl;
  cout << "-X mod  String mod als Zusatz für EXIF-Kennung verwenden" << endl;
  cout << "         =Differenzierung für Zubehör/Adapter, das nicht in EXIF-Daten erscheint" << endl;

  cout << "-v      viele Textausgaben" << endl;
  cout << "-V n    Textausgaben zu " << endl;
  cout << "         n = 1 - Schritte" << endl;
  cout << "         n = 2 - Optimierungsergebnis" << endl;
  cout << "         n = 4 - Transformationen" << endl;
  cout << "         n = 8 - Markersuche" << endl;
  cout << "         n =16 - Punktzuordnung" << endl;

  cout << "-w      Warten auf Tastendruck nach jedem Bild" << endl;
  cout << "-w      Warten auf Tastendruck nach jedem Iterationschritt in der Markersuche" << endl;

  cout << "-h      Ausgabe dieser Hilfe" << endl;
  exit(0);
}

void error(const string &pname, const string &msg)
{
  string msg1 = "!!! Fehler - " + msg + "  !!!";
  int lng = msg1.length();
  string rahmen(lng, '-');
  cout << rahmen << endl;
  cout << msg1 << endl;
  cout << rahmen << endl;
  usage(pname);
}

//========== Main =====================
int Main(int argc, char *argv[])
{
  vector<vector<Point> > pointList;
  vector<vector<Point> > referenceList;
  vector<Trafo> trafoList;
  vector<Distortion1> distList;
  int c;

  int seglevel = 55;
  int segsize = 15;
  int smear = 0;

  int minreferences = 400;

  int images = 0;
  int output = 0;
  string exif; // EXIF-basierte Kennung
  string suffix; // Modifier für EXIF-Kennung
  double focallength = 0.0;

  while ((c = getopt(argc, argv, "dhwWvV:eo:E:sS:l:L:X:m:")) >= 0)
    {
      switch (c)
        {
        case 's':
          smear = 5;
          break;
        case 'S':
          smear = atoi(optarg);
          break;
        case 'l':
          seglevel = atoi(optarg);
          break;
        case 'L':
          segsize = atoi(optarg);
          break;
        case 'm':
          minreferences = atoi(optarg);
          break;
        case 'd':
          display = true;
          break;
        case 'W':
          wait = 2;
          break;
        case 'w':
          wait = 1;
          break;
        case 'v':
          Verbose = v_step;
          break;
        case 'V':
          Verbose |= atoi(optarg);
          break;
        case 'e':
          exifmode = exif_ignore;
          break;
        case 'E':
          exifmode = exif_user;
          exif = optarg;
          for (unsigned int i = 0; i < exif.length(); i++)
            if (exif[i] == ' ')
              exif[i] = '_';
          break;
        case 'X':
          suffix = optarg;
          break;
        case 'o':
          switch (optarg[0])
            {
            case 'd':
              output |= output_distortion;
              break;
            case 'i':
              output |= output_intrinsic;
              break;
            case 'D':
              output |= output_distdb;
              break;
            default:
              error(argv[0], "Falsche Ausgabeauswahl");
              break;
            }
          break;
        case 'h':
          usage(argv[0]);
          break;
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind <= 0) // keine verbleibenden Parameter = Bildnamen
    usage(argv[0]);

  if (output == 0)
    output = 3;

  vector<string> file;
  FileList(argc, argv, optind, file, false);
  // cout << file.size() << " Dateien" << endl;
  // for (int i=0;i<file.size();++i)
  //    cout << file[i] << endl;
  int dimx = -1;
  int dimy = -1;

  Image img;
  Image mrk;
  Image segmented_img;

  images = 0;
  while (!file.empty())
    {
      string fn = file.back();
      file.pop_back();

      if (Verbose)
        cout << fn << endl;

      int sx, sy, mv, ch;

      InfImgFile(fn, sx, sy, mv, ch);

      string fileexif;
      double filefocallength;
      if (exifmode == exif_file)
	{
	  if (!getExifSig(fn, suffix, fileexif, filefocallength))
	    {
	      error(argv[0], "EXIF-Daten nicht vorhanden");
	    }

	  if (exif.empty()) // erste Datei
	    {
	      exif = fileexif;
	      focallength = filefocallength;
	    }

	  if (fileexif != exif || filefocallength != focallength)
	    {
	      error(argv[0], "Unterschiedliche EXIF-Daten");
	    }
	}

      if (dimx < 0)
	{
	  // erstes Bild
	  dimx = sx;
	  dimy = sy;

	  img = NewImg(dimx, dimy, 255);
	  mrk = NewImg(dimx, dimy, 7);
	  segmented_img = NewImg(dimx, dimy, 4);
	  if (display)
	    {
	      Show(OVERLAY, img, mrk);
	      Show(OVERLAY, img, segmented_img);
	    }

	}

      if (dimx != sx || dimy != sy)
	error(argv[0], "Bilder haben unterschiedliche Größen");

      // Load image
      ReadImg(fn, img);
      if (smear > 0)
	smearImg(img, img, smear);
      // Segmentation
      LocalSeg(img, segmented_img, segsize, seglevel);

      // new pointlists, homography and distortion
      pointList.push_back(vector<Point>());
      referenceList.push_back(vector<Point>());
      trafoList.push_back(Trafo());
      distList.push_back(Distortion1());

      if (GetReferencePoints(img, mrk, segmented_img,
			     pointList[images], referenceList[images],
			     trafoList[images], distList[images]) &&
	  (int)referenceList[images].size() >= minreferences)
	{
#if 0
	  Image rest = NewImg(img);
	  Show(ON, rest);
	  distList[images].RectImg(img, rest, INTERPOL);
	  GetChar();
#endif
<<<<<<< fcalib.cpp
	  images++;
	  if (Verbose & v_step)
	    cout << "OK" << endl;
	}
      else
	{
	  pointList.pop_back();// Liste verwerfen
	  referenceList.pop_back();// Liste verwerfen
	  trafoList.pop_back();// Liste verwerfen
	  distList.pop_back();// Liste verwerfen
	  if (!(Verbose & v_step))
	    cout << fn ;
	  cout << " nicht nutzbar!" << endl;

	}
      if (wait > 0)
	GetChar();
    }
  optind++;
=======
              images++;
              if (Verbose & v_step)
                cout << "OK" << endl;
            }
          else
            {
              pointList.pop_back();// Liste verwerfen
              referenceList.pop_back();// Liste verwerfen
              trafoList.pop_back();// Liste verwerfen
              distList.pop_back();// Liste verwerfen
              if ((Verbose & v_step))
                cout << fn ;
              cout << " nicht nutzbar!" << endl;
>>>>>>> 1.11


  Distortion1 distortion;
  Matrix m(3, 3);

  if (images > 0)
    {
      // Berechnungen

      if ((output & output_distortion) || (output & output_distdb))
        {
          // cout << pointList.size() << " " << referenceList.size() << endl;
          // for (int i=0;i<pointList.size();i++)
          //   cout << pointList[i].size() << " " << referenceList[i].size() << endl;
          distortion.Calc(pointList, referenceList);
        }

      if (output & output_intrinsic)
        {
          if (images < 3)
            error(argv[0], "nicht genug Bilder für Kamera-Parameter");
          else
            m = calib_intrinsic_zhang(trafoList);
        }

      // Ausgaben
      if (Verbose)
        cout << "--------------------------" << endl;

      if (output & output_intrinsic)
        {
          Print(m, dimx, dimy);
        }

      if (output & output_distortion)
        {
          Print(distortion, dimx, dimy);
        }

      if (output & output_distdb)
        {
          if (exifmode == exif_ignore)
            error(argv[0], "Keine EXIF-Daten - kein Eintrag in DB");

          double x0 = distortion.X0();
          double y0 = distortion.Y0();
          double d2 = distortion.D2();
          double d4 = distortion.D4();

          NormalizeDistortion(x0, y0, d2, d4, dimx, dimy);
          string signature = exif;
          ofstream ddb("dist.txt", ios::app);
          ddb.precision(12);
          ddb << signature << " "  << focallength << " ";
          ddb << x0 << " " << y0 << " ";
          ddb << d2 << " " << d4 << endl;
        }
    }
  return OK;
}

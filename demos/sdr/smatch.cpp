#include <string>
#include <iostream>
#include <image.h>
#include <fitfn.h>
#include <vector>
#include <getopt.h>

#include "smatch.h"

using namespace std;

string fn[MAXIMAGES];

ColorImage rgb[MAXIMAGES]; // Originale RGB-Bilder
Image img[MAXIMAGES]; // Originale Grauwert-Bilder

ColorImage rgb_m[MAXIMAGES]; // Ausgerichtete RGB-Bilder
Image mrk_m[MAXIMAGES]; // Markierungsbilder für "ungültige" Punkte

ColorImage dest;
int dxs, dys;

Image Iglobalshift;
Image Ilocalshift;

Trafo tr[MAXIMAGES];

bool ignoreglobal = false; // Für demozwecke
bool manual = false;
bool cross = false;

int images;
int refimg;
int summaxval;

int ilevel = 0;
char waitc = 's';
double beta = 0.1;

void Wait(int level)
{
  if (level > ilevel)
    {
      waitc = 's';
    }
  if (waitc != 'g')
    {
      int x, y;
      GetAlphaCursor(x, y);
      Print("Press key to continue");
      int i = GetChar();
      if (i == 'g') waitc = 'g';
      SetAlphaCursor(x, y);
      Print("\x010");
    }
}

void Trans(void)
{
  Image hmark1;

  int i;

  for (i = 0; i < images; i++)
    {
      Printf("Transforming %d\n", i);
      hmark1 = NewImg(img[i]->xsize, img[i]->ysize, 7);
      SetImg(hmark1, 7);

      // Markierungsbild zur Bestimmung gültiger Pixel
      mrk_m[i] = NewImg(dxs, dys, 7);
      rgb_m[i].create(dxs, dys, img[i]->maxval);
      Show(ON, rgb_m[i]);
      Transform(tr[i], hmark1, mrk_m[i], INTERPOL);
      Transform(tr[i], rgb[i], rgb_m[i], INTERPOL);
      Show(OFF, rgb_m[i]);
      FreeImg(hmark1);
    }
}

void Mittel()
{
  Image counter;

  ColorImage summe(dxs, dys, summaxval);

  Show(ON, summe);

  int i, x, y;

  counter = NewImg(dxs, dys, images);
  Show(ON, counter, "Counter");

  summe.Clear();
  ClearImg(counter);

  for (i = 0; i < images; i++)
    {
//    Printf("Summation %d\n",i);
      wloop(counter, x, y)
      {
        if (GetVal(mrk_m[i], x, y) == 7)
          {
            PutVal(summe.RedImage(), x, y,
                   GetVal(rgb_m[i].RedImage(), x, y) + GetVal(summe.RedImage(), x, y));
            PutVal(summe.GreenImage(), x, y,
                   GetVal(rgb_m[i].GreenImage(), x, y) + GetVal(summe.GreenImage(), x, y));
            PutVal(summe.BlueImage(), x, y,
                   GetVal(rgb_m[i].BlueImage(), x, y) + GetVal(summe.BlueImage(), x, y));
            PutVal(counter, x, y, GetVal(counter, x, y) + 1);
          }
      }
    }

  dest = ImageRGB(dxs, dys, summaxval / images);

//  Printf("Division durch Anzahl\n");
  wloop(counter, x, y)
  {
    if (GetVal(counter, x, y) > 0)
      {
        PutVal(dest.RedImage(), x, y,
               GetVal(summe.RedImage(), x, y) / GetVal(counter, x, y));
        PutVal(dest.GreenImage(), x, y,
               GetVal(summe.GreenImage(), x, y) / GetVal(counter, x, y));
        PutVal(dest.BlueImage(), x, y,
               GetVal(summe.BlueImage(), x, y) / GetVal(counter, x, y));
      }
    else
      {
        PutVal(dest.RedImage(), x, y, 0);
        PutVal(dest.GreenImage(), x, y, 0);
        PutVal(dest.BlueImage(), x, y, 0);
      }
  }
}

void WriteImages()
{
  int i;

  string::size_type pos;
  string newfilename;

  for (i = 0; i < images; i++)
    {
      newfilename = fn[i];
      pos = newfilename.find_last_of(".");
      if (pos == string::npos)
        newfilename += ".sm"; // kann nicht passieren, da ".jpg" vorhanden sein sollte
      else
        newfilename.insert(pos, ".sm");
      Printf("Writing %s\n", newfilename.c_str());
      rgb_m[i].Write(newfilename);
    }
}

int Load(const string& dir)
{
  int x, y, v;
  TextList Dir;
  TextListIter dptr;
#ifndef WIN32
  Directory(Dir, dir + "/*.JPG");
#endif
  Directory(Dir, dir + "/*.jpg");
  images = 0;
  summaxval = 0;
  for (dptr = Dir.begin(); dptr < Dir.end(); dptr++)
    {
      Print(*dptr);
      fn[images] = dir + "/" + *dptr;

      if ((fn[images].rfind("sm.jpg") == string::npos) &&
          (fn[images].rfind("sm.JPG") == string::npos))
        {
          InfImgFile(fn[images], x, y, v);
          Printf(" (%d*%d) ", x, y);
          summaxval += v;
          img[images] = NewImg(x, y, v);
//    mrk[images]=NewImg(x,y,v);

          IfFailed(ReadImg(fn[images], img[images]))
          {
            FreeImg(img[images]);
            Print(" -");
          }
          else
            {
              rgb[images].Read(fn[images]);
              images++;
              Print(" +");
            }
        }
      Print("\n");
    }
  refimg = images / 2;
  if (images < 2) return ERROR;
  return OK;
}

void usage(const string& progname)
{
  cout << progname << endl;
  cout << "Aufruf: " << endl;
  cout << progname << " <Optionen> <Verzeichnisname>" << endl << endl;
  cout << "Optionen:" << endl;
  cout << " -h       Hilfe" << endl;
  cout << " -m       Manuell" << endl;
  cout << " -g       Globale Verschiebung ignorieren" << endl;
  cout << " -b beta  Parameter Beta vorgeben" << endl;
  cout << " -i inter Interaktionlevel vorgeben (0-2) " << endl;
  cout << " -c       Darstellungsmode Farbe (default)" << endl;
  cout << " -s       Darstellungsmode nebeneinander" << endl;
  cout << " -k       Koordinatenachsen einzeichnen" << endl;
  cout << " -n step  Zahl der Iterationszyklen" << endl;
  exit(1);
}

#define COLOR 1
#define SIDE_BY_SIDE 2
int main(int argc, char* argv[])
{
  string Dir = ".";
  int i;
  int m, n;
  int displaymode = COLOR;
  int rc;
  int ct = 3;

  while ((rc = getopt(argc, argv, "mcsi:n:b:gkh")) >= 0)
    {
      switch (rc)
        {
        case 'm':
          manual = true;
          break;
        case 'g':
          ignoreglobal = true;
          break;
        case 'b':
          beta = atof(optarg);
          break;

        case 'i':
          ilevel = atol(optarg);
          if (ilevel > 2) waitc = 'g';
          break;
        case 'c':
          displaymode = COLOR;
          break;
        case 's':
          displaymode = SIDE_BY_SIDE;
          break;
        case 'k':
          cross = true;
          break;
        case 'n':
          ct = atol(optarg);
          break;

        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  if (optind < argc) // verbleibender Parameter ?
    Dir = argv[optind]; // muß Filename sein
  else
    {
      cerr << "Missing directory name" << endl;
      exit(1);
    }

  OpenAlpha("SDR demonstration");
  Alpha(ON);

  ClearAlpha();

  Display(ON);

  if (Load(Dir) != OK)
    {
      Print("Failed to load images\n");
      GetChar();
      return 1;
    }

  Iglobalshift = NewImg(GLOBALSHIFTSIZE, GLOBALSHIFTSIZE, 255);
  Ilocalshift = NewImg(LOCALSHIFTSIZE, LOCALSHIFTSIZE, 255);

  Image blau = NewImg(img[0]);
  SetImg(blau, blau->maxval);

  for (i = 0; i < images; i++)
    {
      Print(NumberString(i) + ":\n");
      if (i != refimg)
        {
          Image himg = NewImg(img[refimg], false);
          Image himg1 = NewImg(img[refimg], true);
          {
            int xm, ym;
            xm = Min(img[i]->xsize, img[refimg]->xsize) - 1;
            ym = Min(img[i]->ysize, img[refimg]->ysize) - 1;
            SetWindowImg(img[i], 0, 0, xm, ym);
            SetWindowImg(img[refimg], 0, 0, xm, ym);
            CopyImg(img[i], himg);
            SetWindowImg(img[i]);
            SetWindowImg(img[refimg]);
          }

          if (manual)
            {
              Show(ON, himg);
              Vector v1 = (Vector)SelVector(himg);
              Circle c(v1[0], v1[1], 5);
              c.Draw(himg, 0, 0);
              Show(ON, himg1);
              Vector v2 = (Vector)SelVector(himg1);
              Show(OFF, himg);
              Show(OFF, himg1);
              tr[i].Init();
              tr[i].Shift(v2 - v1);
            }

          switch (displaymode)
            {
            case COLOR:
//    CopyImg(himg1,blau);
              SetImg(blau, blau->maxval);
              Show(_RGB, himg1, himg, blau/*img[refimg]*/);
              Zoom(himg1, 0, 0, 0);
              break;
            case SIDE_BY_SIDE:
              Show(ON, himg1);
              Show(ON, himg);
              break;
            }

          int mode = DT_NEARLY_SHIFT;
          if (manual) mode |= DT_REFINE;

          DetectTrafo1(img[i], img[refimg], himg, blau, tr[i], beta, ct, mode);
          Show(OFF, himg);
          FreeImg(himg);
          FreeImg(himg1);

          for (n = 0; n < 3; n++)
            {
              for (m = 0; m < 3; m++) Print(NumberString(tr[i].Tmatrix()[n][m]) + "  ");
              Print("\n");
            }
          Wait(1);

        }
      else
        tr[i] = Trafo(2, 2);

    }

  double xmin = 0.0;
  double xmax = 0.0;
  double ymin = 0.0;
  double ymax = 0.0;

  for (i = 0; i < images; i++)
    {
      double xp, yp; // koordinaten der (transformierten) Ecke
      Transform(tr[i], 0, 0, xp, yp); // l.o.
      if (xp < xmin) xmin = xp;
      if (xp > xmax) xmax = xp;
      if (yp < ymin) ymin = yp;
      if (yp > ymax) ymax = yp;
      Transform(tr[i], img[i]->xsize, 0, xp, yp); // r.o.
      if (xp < xmin) xmin = xp;
      if (xp > xmax) xmax = xp;
      if (yp < ymin) ymin = yp;
      if (yp > ymax) ymax = yp;
      Transform(tr[i], img[i]->xsize, img[i]->ysize, xp, yp); // r.u.
      if (xp < xmin) xmin = xp;
      if (xp > xmax) xmax = xp;
      if (yp < ymin) ymin = yp;
      if (yp > ymax) ymax = yp;
      Transform(tr[i], 0, img[i]->ysize, xp, yp); // l.u.
      if (xp > xmax) xmin = xp;
      if (yp < ymin) ymin = yp;
    }

  for (i = 0; i < images; i++)
    {
      tr[i].Shift(-xmin, -ymin);
    }
  xmax -= xmin;
  ymax -= ymin;

  dxs = RoundInt(xmax);
  dys = RoundInt(ymax);

  Printf("Shift: %d,%d\n",
         RoundInt(-xmin), RoundInt(-ymin));
  Printf("Size of combined image: %d * %d\n", dxs, dys);
  Printf("Transforming images\n");
  Trans();

  Printf("Wiriting images\n");
  WriteImages();

  Printf("Calculating average image .. ");
  Mittel();
  Printf("Done");

  dest.Write(Dir + "/mittel.sm.jpg");

  Show(ON, dest);
  Zoom(dest.RedImage(), 0, 0, 0);

  GetChar();
  return OK;
}

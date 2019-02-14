#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <image.h>

/*
 * Planare Kalibrierung nach Zhang
 * 2007 - Suesse
 * 2012 - Ortmann
 */

void usage(const string &pn)
{
  cout << endl << pn << " - Kamerakalibierung nach Zhang" << endl << endl;
  cout << "Aufruf:" << endl;
  cout << pn << " <options>" << endl;
  cout << "Optionen:" << endl;
  cout << " -d dir  Bildverzeichnis (default: calib)" << endl;
  cout << " -e ext  Extension der Bilddateien (default: jpg)" << endl;
  cout << " -c      Darstellung des Kalibriermusters" << endl;
  cout << " -o file Ausgabe des Kalibriermusters in Datei file" << endl;
  cout << " -x xs     Größe des Bildes in X-Richtung" << endl;
  cout << " -y ys     Größe des Bildes in Y-Richtung" << endl;
  cout << " -s      Zeige Bilder" << endl;
  exit(1);
}

int main(int argc, char *argv[])
{
  vector<Image> bilder;
  vector<string> fn;

  int dimx, dimy, max_gw;

  string dirname = "calib";
  string ext = "jpg";

  int mode = 0; // standard: Kalibrierung

  int xsize = 1024;
  int ysize = 767;
  string ofn;

  bool show = false;

  int rc;
  while ((rc = getopt(argc, argv, "d:e:co:hx:y:s")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          xsize = atoi(optarg);
          break;
        case 'y':
          ysize = atoi(optarg);
          break;
        case 'd':
          dirname = optarg;
          break;
        case 'e':
          ext = optarg;
          break;
        case 'c':
          mode = 1;
          break;
        case 'o':
          mode = 2;
          ofn = optarg;
          break;
        case 's':
          show = true;
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  if (mode == 0)
    {
      Directory(fn, dirname + "/*." + ext);

      if (fn.size() == 0)
        {
          cerr << "Kein Bild gefunden" << endl;
          exit(1);
        }

      cout << fn[0] << endl;

      if (InfImgFile(dirname + "/" + fn[0], dimx, dimy, max_gw) != OK)
        {
          cerr << "Kann Bilddatei " << fn[0] << " nicht erkennen" << endl;
          exit(1);
        }
      // *****************************

      Printf("dimx %d dimy %d\n", dimx, dimy);

      for (unsigned int i = 0; i < fn.size(); i++)
        {
          bilder.push_back(Image(dimx, dimy, max_gw, fn[i]));
          Print(dirname + "/" + fn[i] + "\n");
          ReadImg(dirname + "/" + fn[i], bilder[i], IB_CROP);

          if (show)
            Show(ON, bilder[i]);

        }

      if (show)
        {
          GetChar();
          for (unsigned int i = 0; i < fn.size(); i++)
            Show(OFF, bilder[i]);
        }

      Camera camera;
      Image pic;
      pic = NewImg(dimx, dimy, max_gw);
      ClearImg(pic);
      Image mark;
      mark = NewImg(dimx, dimy, max_gw);
      ClearImg(mark);
      Show(OVERLAY, pic, mark);

      Calibrate(bilder, camera, true, pic, mark);

      cout << camera.toString("\n")   << endl;
    }
  else if (mode == 1)
    {
      Image img = DrawPattern(xsize, ysize);
      Show(ON, img);
      GetChar();
    }
  else // mode==2
    {
      Image img = DrawPattern(xsize, ysize);
      WriteImg(img, ofn);
//      Show(ON,img);
//      GetChar();
    }

  return OK;
}

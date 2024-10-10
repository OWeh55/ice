#include <image.h>
#include <fstream>

#include "image3d.h"
#include "display3d.h"
#include "depth.h"
#include "povexport.h"
#include "dequantize.h"
#include "graynormalize.h"

int zfak = 4;

string basedir = "/home/noo/3d/volume/mamma/";
const string mask = "img*.bmp";

class SeriesName
{
//private:
public:
  string basedir;
  string patient;
  string study;
  string series;
public:
  string Filename() const
  {
    return basedir + "/" + patient + "/" + study + "/" + series;
  }
};

SeriesName SelectSeries(const SeriesName& ser)
{
  SeriesName res = ser;
  res.series = "";

  string imgfile, imgdir;
  if (res.patient.empty())
    {
      SelFile(res.basedir + "/*", imgfile, imgdir, DIR_DIR, "Patient waehlen", 2, 4, 60, 20);
      res.patient = imgfile;
    }
  if (res.study.empty())
    {
      SelFile(res.basedir + "/" + res.patient + "/*", imgfile, imgdir, DIR_DIR, "Studie waehlen", 2, 4, 60, 20);
      res.study = imgfile;
    }

  SelFile(res.basedir + "/" + res.patient + "/" + res.study + "/*", imgfile, imgdir, DIR_DIR, "Serie waehlen", 2, 4, 60, 20);
  res.series = imgfile;
  return res;
}

SeriesName SelectSeries(const string& dir = basedir)
{
  SeriesName res;
  res.basedir = dir;
  res = SelectSeries(res);
  return res;
}

int main(int argc, char** argv)
{
  //  vector<string> fn;
  int zoom = 1;
  int level = 0;
  bool bin = false;
  bool nonormalize = false;

  try
    {
      int rc;
      while ((rc = getopt(argc, argv, "z:hZ:qyn")) >= 0)
        {
          switch (rc)
            {
            case 'z':
              zoom = atoi(optarg);
              break;
            case 'Z':
              zfak = atol(optarg);
              break;
            case 'n':
              nonormalize = true;
              break;
            }
        }

      if (optind < argc)
        basedir = argv[optind];

      Alpha(ON);
      ClearAlpha();

      Printf("\n                BMP-Viewer\n");
      Printf("Stellt BMP-Dateien dar, die aus DICOM-Datensatz extrahiert wurden\n");

      SeriesName ser1 = SelectSeries();
      Image3d<Image> primary(ser1.Filename() + "/" + mask, 1, 1, zfak);

      if (!nonormalize)
        NormalizeImages(primary);

      double xsd, ysd, zsd;
      primary.getSizeD(xsd, ysd, zsd);
      int xs = xsd;
      int ys = ysd;
      int zs = zsd;
      Printf("Groesse  %d %d %d\n", xs, ys, zs);
      Display3D display1(primary, ser1.Filename());

      SeriesName ser2 = SelectSeries(ser1);
      Image3d<Image> secondary(ser2.Filename() + "/" + mask, 1, 1, zfak);

      Display3D display2(secondary, ser2.Filename());

      if (!nonormalize)
        NormalizeImages(secondary);

      Image3d<Image> diff(primary);

      Display3D difference(diff, "Differenz " + ser2.Filename() + " - " + ser1.Filename());

      int xa = xs / 2, ya = ys / 2, za = zs / 2;

      bool update_diff = true;
      bool cont = true;
#if 0
      int dx = 0;
      int dy = 0;
#endif
      bool lines = true;

      while (cont)
        {
          if (update_diff)
            {
              Subtract(secondary, primary, diff, SMD_SHIFT);
              update_diff = false;
            }

          Printf("Position   xa: %d  ya: %d za: %d\n", xa, ya, za);
          Printf("Grauwerte  s1: %f  s2: %f dif: %f\n",
                 primary.GetValD(xa, ya, za),
                 secondary.GetValD(xa, ya, za),
                 diff.GetValD(xa, ya, za) - 128);

          Printf("Referenzpegel: %d\n", level);

          display1.Update(xa, ya, za);
          display2.Update(xa, ya, za);
          difference.Update(xa, ya, za);

          int c = 0;

          while (! display1.getMouse(xa, ya, za) &&
                 ! display2.getMouse(xa, ya, za) &&
                 ! difference.getMouse(xa, ya, za) &&
                 (c = GetKey()) == 0)
            /* nichts*/;

          //    cout << (int)c << " " ;
          //    cout.flush();
          switch (c)
            {
#if 0
            case 'r':
              dx--;
              update_diff = true;
              break;
            case 'l':
              dx++;
              update_diff = true;
              break;
            case 'o':
              dy++;
              update_diff = true;
              break;
            case 'u':
              dy--;
              update_diff = true;
              break;
#endif

            case '+':
              if (level < 255)
                level++;
              if (bin)
                display1.setMarkerLevel(level);
              break;
            case '-':
              if (level > 0)
                level--;
              if (bin)
                display1.setMarkerLevel(level);
              break;

            case 'z':
              if (za > 0) za--;
              break;
            case 'Z':
              if (za < zs - 1) za++;
              break;
            case 'x':
              if (xa > 0) xa--;
              break;
            case 'X':
              if (xa < xs - 1) xa++;
              break;
            case 'y':
              if (ya > 0) ya--;
              break;
            case 'Y':
              if (ya < ys - 1) ya++;
              break;

            case 'm':
              lines = !lines;
              display1.Cursor(lines);
              display2.Cursor(lines);
              difference.Cursor(lines);
              break;

            case 'l':
              level = RoundInt(primary.GetValD(xa, ya, za));
              if (bin)
                display1.setMarkerLevel(level);
              break;

            case 'b':
              bin = !bin;
              if (bin)
                display1.setMarkerLevel(level);
              else
                display1.setMarkerLevel(0);
              break;

            case 't':
            {
              // tiefenbild
              Image depth, depthvalid;

              DepthImage(primary, level, depth, depthvalid, 100);

              Show(OVERLAY, depth, depthvalid);

              Image depth2 = NewImg(depth, true);
              Dequantize(depth, 200, depth2);

              Show(OVERLAY, depth2, depthvalid);

              povExportDepth(depth2, depthvalid, "test.inc", 100);
              cvExportDepth(depth2, depthvalid, "test.cv", 100);

              system("cviewer -c 1 test.cv");
              //  GetChar();
            }
            break;

            case 'T':
            {
              // "Tiefenbild" Typ 2: Punkte eines Objektes werden gesucht
              // und in das tiefenbild eingetragen

              if (primary.GetValD(xa, ya, za) < level)
                {

                  Image3d<Image> binary(xs, ys, zs, 255);
                  Display3D displayb(binary);

                  for (int x = 0; x < xs; x++)
                    for (int y = 0; y < ys; y++)
                      for (int z = 0; z < zs; z++)
                        {
//              cout << x << " "<< y << " "<< z <<endl;
//              cout << xs << " "<< ys << " "<< zs <<endl;
                          int val = primary.GetValD(x, y, z);
                          if (val >= level)
                            binary.PutVal(x, y, z, 255);
                          else
                            binary.PutVal(x, y, z, 0);
                        }

                  displayb.Update(xa, ya, za);

                  Image depth = NewImg(xs, zs, ys * 100);
                  SetImg(depth, ys * 100);
                  Image depthvalid = NewImg(xs, zs, 4);
                  SetImg(depthvalid, 1);

                  Show(OVERLAY, depth, depthvalid);

                  vector<int> xx;
                  vector<int> yy;
                  vector<int> zz;

                  xx.push_back(xa);
                  yy.push_back(ya);
                  zz.push_back(za);

                  int xt, yt, zt;
                  while (!xx.empty())
                    {
                      xt = xx.back();
                      xx.pop_back();
                      yt = yy.back();
                      yy.pop_back();
                      zt = zz.back();
                      zz.pop_back();
                      if (xt >= 0 && xt < xs)
                        if (yt >= 0 && yt < ys)
                          if (zt >= 0 && zt < zs)
                            if (binary.GetVal(xt, yt, zt) == 0)
                              {
                                PutVal(depthvalid, xt, zt, 0);
                                if (yt * 100 < GetVal(depth, xt, zt))
                                  PutVal(depth, xt, zt, yt * 100);
                                binary.PutVal(xt, yt, zt, 255);

                                xx.push_back(xt - 1);
                                yy.push_back(yt);
                                zz.push_back(zt);
                                xx.push_back(xt + 1);
                                yy.push_back(yt);
                                zz.push_back(zt);
                                xx.push_back(xt);
                                yy.push_back(yt + 1);
                                zz.push_back(zt);
                                xx.push_back(xt);
                                yy.push_back(yt - 1);
                                zz.push_back(zt);
                                xx.push_back(xt);
                                yy.push_back(yt);
                                zz.push_back(zt + 1);
                                xx.push_back(xt);
                                yy.push_back(yt);
                                zz.push_back(zt - 1);

                              }
                    }

                  Image depth2 = NewImg(depth, true);
                  Show(OVERLAY, depth2, depthvalid);
                  Dequantize(depth, 200, depth2);

                  povExportDepth(depth2, depthvalid, "test.inc", 100);
                  cvExportDepth(depth2, depthvalid, "test.cv", 100);

                  system("cviewer -c 1 test.cv");
                }
              else
                Print("Punkt nicht im Objekt\n");
            }
            break;

            case 'Q':
            case 'q':
              cont = !Ask("Wirklich beenden?");
              break;
            }
        }
    }

  catch (const char* msg)
    {
      cerr << "Exception: " << msg << endl;
    }
  return 0;
}

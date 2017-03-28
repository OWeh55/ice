#include "unistd.h"
#include "image.h"

bool verbose = false;

void usage(const string& prog)
{
  cout << endl << prog << " Richtungsorientierte Filterung" << endl;
  cout << "Aufruf: " << prog << " [optionen] filename" << endl;
  cout << "Optionen:" << endl;
  cout << endl << " Eingabe:" << endl;
  cout << "  -i filename  zu filternde Datei (Bild oder Video)" << endl;
  cout << "                  kann alternativ auch als Parameter angegeben werden" << endl;
  cout << "  -g           Erzwinge Schwarz-Weiß-Bild" << endl;
  cout << "  -f           Eingabefile ist ein Video (Schalter bei Videos zwingend erforderlich)" << endl;
  cout << "  -n  nr       Verarbeite nur nr. Bild des Videos" << endl;

  cout << endl << " Verarbeitung:" << endl;
  cout << "  -d size      Größe des Richtungs-Detektor-Feldes" << endl;
  cout << "  -t c         Filtertyp c" << endl;
  cout << "               c = d  DoB-Filter" << endl;
  cout << "               c = s  Smear-Filter" << endl;
  cout << "               c = e  Edge-Filter" << endl;
  cout << "               c = a  Winkel aus Gradienten-Summe" << endl;
  cout << "               c = A  Winkel aus Struktur-Tensor" << endl;
  cout << "  -s size      Groesse der Filtermaske" << endl;
  cout << "  -l length    Laenge des Filters" << endl;
  cout << "  -w width     Breite des Filters" << endl;
  cout << "  -S size      Setzt alle Größenangaben entsprechend einer Filtermaske size" << endl;
  cout << "  -N           Normalisierung der Grauwerte/Intensitäten" << endl;
  cout << "  -H           Histogramm-Egalisierung" << endl;
  cout << "  -u           Ungerichtete Filter" << endl;

  cout << endl << " Ausgabe:" << endl;
  cout << "  -o filename  Dateiname für die Ausgabe (default:keine Ausgabe)" << endl;

  cout << endl << " Sonstiges:" << endl;
  cout << "  -b           Batchmode - kein Warten" << endl;
  cout << "  -v           Geschwätzig" << endl;
  exit(1);
}

void CalcDirectionImg2(const Image& src, Image& direction, int dsize, char typ)
{
  // Filtertyp-abhängige Winkelbestimmung
  if (typ == 'e' || typ == 'a')
    {
      //      cout << "CalcDirectionImg" << endl;
      CalcDirectionImg(src, direction, dsize);
    }
  else
    {
      //     cout << "CalcDirectionStructImg" << endl;
      CalcDirectionStructImg(src, direction, dsize);
    }
}

void FilterImg(const Image& src, const Image& direction, const Image& filtered,
               char typ,
               int fsize, int flen, int fwidth)
{
  switch (typ)
    {
    case 's':
      OrientedSmearImg(src, direction, filtered, fsize, flen, fwidth);
      break;
    case 'S':
      cout << flen << endl;
      SmearImg(src, filtered, int(flen) | 1);
      break;

    case 'd':
      OrientedDoBImg(src, direction, filtered, fsize, flen, fwidth);
      break;
    case 'D':
      DoBImg(src, filtered, fsize, fwidth);
      break;

    case 'e':
      OrientedEdgeImg(src, direction, filtered, fsize, flen);
      break;
    case 'A':
    case 'a':
      // Richtung ist resultat
      CopyImg(direction, filtered);
      break;
    default:
      cerr << "Falscher Filtertyp '" << typ << "'" << endl;
    }
}

void FilterImg(const ColorImage& src, const Image& direction, const ColorImage& filtered,
               char typ,
               int fsize, int flen, int fwidth)
{
  switch (typ)
    {
    case 's':
      OrientedSmearImg(src, direction, filtered, fsize, flen, fwidth);
      break;
    case 'S':
      cout << flen << endl;
      SmearImg(src, filtered, int(flen) | 1);
      break;
    case 'd':
      OrientedDoBImg(src, direction, filtered, fsize, flen, fwidth);
      break;
    case 'D':
      DoBImg(src, filtered, fsize, flen);
      break;
    case 'e':
      OrientedEdgeImg(src, direction, filtered, fsize, flen);
      break;
    case 'A':
    case 'a':
    {
      // Richtung ist resultat
      WindowWalker ww(direction);
      for (ww.init(); !ww.ready(); ww.next())
        {
          double angle = (double)direction.getPixel(ww) * 2 * M_PI / direction.maxval;
          filtered.setPixel(ww, ColorValue((cos(angle) + 1)*filtered.maxval / 2.0,
                                           (cos(angle + M_PI * 2 / 3) + 1)*filtered.maxval / 2.0,
                                           (cos(angle - M_PI * 2 / 3) + 1)*filtered.maxval / 2.0));
        }
    }
    break;
    default:
      cerr << "Falscher Filtertyp '" << typ << "'" << endl;
    }
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  string ofn = "";

  bool wait = true;
  bool gray = false;
  bool video = false;
  int framenumber = -1;

  bool hequal = false;
  bool gnorm = false;

  char typ = 's';

  int fsize = 15; // Größe der Filtermaske
  int flen = fsize - 1; // Länge bzw. Radius des Filters
  int fwidth = 1; // Breite des Filters (nicht bei Edge)

  int dsize = 15; // Groesse des Feldes für Richtungsdetektion
  bool def = true;

  bool undirected = false;

  int rc;

  while ((rc = getopt(argc, argv, "NHS:s:l:w:i:d:o:gvt:bfn:hu")) >= 0)
    {
      switch (rc)
        {
        case 's':
          fsize = atol(optarg);
          def = false;
          break;
        case 'S':
          fsize = atol(optarg);
          flen = fsize - 1;
          dsize = fsize;
          def = false;
          break;
        case 'l':
          flen = atol(optarg);
          def = false;
          break;
        case 'w':
          fwidth = atol(optarg);
          def = false;
          break;

        case 'd':
          dsize = atol(optarg);
          def = false;
          break;
        case 'f':
          video = true;
          break;
        case 'n':
          framenumber = atol(optarg);
          break;

        case 'i':
          fn = optarg;
          break;
        case 'o':
          ofn = optarg;
          break;

        case 'g':
          gray = true;
          break;
        case 't':
          typ = optarg[0];
          break;


        case 'H':
          hequal = true;
          break;
        case 'N':
          gnorm = true;
          break;

        case 'u':
          undirected = true;
          break;

        case 'b':
          wait = false;
          break;
        case 'v':
          verbose = true;
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  if (optind < argc)
    {
      fn = argv[optind];
    }

  if (undirected)
    {
      if (typ == 'd')
        typ = 'D';
      else if (typ == 's')
        typ = 'S';
      else
        {
          cerr << "Ungerichtete Filterung für Typ " << typ << " nicht möglich!" << endl;
          exit(1);
        }
    }

  if (def)
    {
      OpenAlpha("dfilter");
      ClearAlpha();
      Print("Bitte Filter-Groesse eingeben\n");
      fsize = Input("Groesse: ");
      flen = fsize - 1;
      dsize = fsize;
      ofn = fn + ".filtered.jpg";
    }

  int hsize = fsize / 2;

  if (!video)
    {
      int dimx, dimy, maxval, channels;
      InfImgFile(fn, dimx, dimy, maxval, channels);

      Window valid(hsize, hsize, dimx - hsize - 1, dimy - hsize - 1);

      if (verbose)
        {
          cout << "Bearbeite " << fn << ": " << dimx << "*" << dimy;
          cout << " " << maxval << " " << channels << endl;
        }

      if (!gray)
        gray = channels == 1;

      if (gray)
        {
          Image src, direction, filtered;

          src = ReadImg(fn);

          filtered = NewImg(src, false);
          SetImg(filtered, maxval);
          direction = NewImg(src->xsize, src->ysize, 90);

          Show(ON, src, "Original");
          Show(ON, filtered, "Gefiltered");

          double ta = TimeD(TM_WORLD);

          CalcDirectionImg2(src, direction, dsize, typ);

          if (verbose)
            cout << "Nach Richtungsbestimmung " << TimeD(TM_WORLD) - ta << endl;

          FilterImg(src, direction, filtered, typ, fsize, flen, fwidth);

          if (gnorm)
            {
              Image timg(filtered, valid);
              GrayNormalize(timg);
            }

          if (hequal)
            {
              Image timg(filtered, valid);
              HistogramEqual(timg);
            }


          if (verbose)
            cout << ((TimeD(TM_WORLD) - ta) * 1000) << " ms" << endl;

          if (!ofn.empty())
            WriteImg(filtered, ofn);

          if (wait)
            Enter("Press Enter to continue");
        }
      else
        {
          ColorImage src;
          src.read(fn);
          Image direction = NewImg(src.redImage()->xsize, src.redImage()->ysize, 90);
          ColorImage filtered;
          filtered.create(dimx, dimy, maxval);
          SetImg(filtered, maxval);

          Show(ON, src, "Original");
          Show(ON, filtered, "Gefiltered");

          /*
            Show(ON,filtered.R(),"Rot");
            Show(ON,filtered.G(),"Gruen");
            Show(ON,filtered.B(),"Blau");
          */

          //     Show(ON,direction,"Richtung");

          double ta = TimeD(TM_WORLD);

          CalcDirectionImg2(src.greenImage(), direction, dsize, typ);

          if (verbose)
            cout << "Nach Richtungsbestimmung " << TimeD(TM_WORLD) - ta << endl;

          FilterImg(src, direction, filtered, typ, fsize, flen, fwidth);

#if 0
          if (gnorm)
            {
              ColorImage timg(filtered, valid);
              Normalize(timg);
            }
#endif
          if (hequal)
            {
              ColorImage timg(filtered, valid);
              HistogramEqual(timg);
            }

          if (verbose)
            cout << ((TimeD(TM_WORLD) - ta) * 1000) << " ms" << endl;

          if (!ofn.empty())
            filtered.write(ofn);

          if (wait)
            Enter("Press Enter to continue");
        }
    }
  else
    {
#ifndef NOVIDEOIO
      // video
      VideoFile vin(fn);

      int dimx, dimy, maxval, frate;
      vin.getPara(dimx, dimy, maxval, frate);

      Window valid(hsize, hsize, dimx - hsize - 1, dimx - hsize - 1);

      ColorImage src;
      src.create(dimx, dimy, maxval);

      Image direction = NewImg(dimx, dimy, 90);
      ColorImage filtered;
      filtered.create(dimx, dimy, maxval);

      Show(ON, src, "Original");
      Show(ON, filtered, "Gefiltered");
      //     Show(ON,direction,"Richtung");

      if (framenumber >= 0)
        {
          // Einzelbild
          int fnr = 0;

          while (fnr < framenumber && vin.Read(src))
            {
              fnr++;
            }

          if (vin.Read(src))
            {

              CalcDirectionImg2(src.greenImage(), direction, dsize, typ);

              FilterImg(src, direction, filtered, typ, fsize, flen, fwidth);
#if 0
              if (gnorm)
                {
                  ColorImage timg(filtered, valid);
                  Normalize(timg);
                }
#endif
              if (hequal)
                {
                  ColorImage timg(filtered, valid);
                  HistogramEqual(timg);
                }

              if (!ofn.empty())
                filtered.write(ofn);


              if (wait)
                Enter("Press Enter to continue");
            }
          else
            cerr << "Frame " << framenumber << " nicht gefunden" << endl;
        }
      else
        {
          // Videosequenz
          VideoFile vout;

          if (!ofn.empty())
            vout.open(ofn, ios_base::out);

          while (vin.Read(src))
            {
              if (verbose)
                cout << "Bildnr: " << vin.FrameNumber() << endl;

              CalcDirectionImg2(src.greenImage(), direction, dsize, typ);

              FilterImg(src, direction, filtered, typ, fsize, flen, fwidth);

#if 0
              if (gnorm)
                {
                  ColorImage timg(filtered, valid);
                  Normalize(timg);
                }
#endif
              if (hequal)
                {
                  ColorImage timg(filtered, valid);
                  HistogramEqual(timg);
                }

              if (!ofn.empty())
                vout.Write(filtered);
            }
        }
#endif
    }

  return OK;
}

#include <image.h>

void Tiefpass(const Image& orig, Image& low, int fgx, int fgy)
{
// idealer Tiefpass durch Filterung im Frequenzbereich (Hartley)
  ImageD origd = NewImgD(orig, true);
  ImageD hart(origd);
  HartleyImgD(origd, hart);
  int xm = (orig.xsize + 1) / 2;
  int ym = (orig.ysize + 1) / 2;

  for (int y = 0; y < hart.ysize; y++)
    {
      int fy = abs(y - ym);

      for (int x = 0; x < hart.xsize; x++)
        {
          int fx = abs(x - xm);

          if ((fgx > 0 && fx > fgx) || (fgy > 0 && fy > fgy))
            hart.setPixel(x, y, 0);
        }
    }

  HartleyImgD(hart, origd);
  ConvImgDImg(origd, low, RAW);
}

int usage(const string& pn)
{
  cout << pn << endl;
  cout << "Usage: " << pn << " <options> filename" << endl;
  cout << "options:" << endl;
  cout << "-h     diese Hilfe" << endl;
  cout << "-f fg  Grenzfrequenz für vorgeschalteten Tiefpass" << endl;
  cout << "        fg=0 : Grenzfrequenz nach Abtasttheorem" << endl;
  cout << "-X fx  Grenzfrequenz für Tiefpass in X-Richtung" << endl;
  cout << "-Y fy  Grenzfrequenz für Tiefpass in Y-Richtung" << endl;
  cout << "-s sf  Subsampling-Faktor" << endl;
  cout << "-x xo  Offset in X-Richtung" << endl;
  cout << "-y yo  Offset in Y-Richtung" << endl;
  cout << "-i is  Integration bei Sampling (Kantenlänge)" << endl;
  cout << "-d dis Anzeige-Schalter (ODER-verknüpfte Werte, default=0)" << endl;
  cout << "        dis=1 Endergebnis" << endl;
  cout << "        dis=2 Nach Tiefpass" << endl;
  cout << "        dis=4 Original" << endl;
  cout << "-o fn  Ausgabe-Dateiname" << endl;
  cout << "-v     Ausführliche Ausgaben" << endl;

  exit(1);
}

int main(int argc, char** argv)
{
  int rc;
  int fgx = -1; // keine Filterung
  int fgy = -1;
  int sub = 1; // "kein" Subsampling
  int sx = 0; //  Shift
  int sy = 0;
  int display = 0; // keine Anzeige
  string filename;
  string outfilename = ""; // keine Dateiausgabe
  int is = 1;    // keine Integration
  bool verbose = false;
  while ((rc = getopt(argc, argv, "hf:s:x:y:d:X:Y:i:o:v")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          fgx = fgy = atol(optarg);
          break;
        case 'X':
          fgx = atol(optarg);
          break;
        case 'Y':
          fgy = atol(optarg);
          break;

        case 's':
          sub = atol(optarg);
          break;

        case 'x':
          sx = atol(optarg);
          break;
        case 'y':
          sy = atol(optarg);
          break;

        case 'i':
          is = atol(optarg);
          break;

        case 'd':
          display = atol(optarg);
          break;
        case 'o':
          outfilename = optarg;
          break;
        case 'v':
          verbose = true;
          break;
        case 'h': // no break
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind > 0) // ein verbleibender Parameter = Dateiname;
    {
      filename = argv[optind];
    }
  else
    usage(argv[0]);

  if (outfilename.empty()) // wenn keine Datei-Ausgabe
    display |= 1;          // dann mindestens Ergebnisanzeige

  int ox, oy, mv;
  InfImgFile(filename, ox, oy, mv);

  Image orig;
  orig.create(ox, oy, mv);

  orig.read(filename);

  if (verbose)
    cout << "Gelesen: " << filename << "  " << ox << " x " << oy << endl;

  if (display & 4)
    Show(ON, orig, "Original");

  Image low;
  low.create(ox, oy, mv);

  if (fgx == 0)
    {
      if (sub != 1)
        // "ideale" Filterung bei subsampling
        fgx = orig->xsize / 2 / sub;
      else
        // kein Filter nötig
        fgx = 0;
    }

  if (fgy == 0)
    {
      if (sub != 1)
        // "ideale" Filterung bei subsampling
        fgy = orig->ysize / 2 / sub;
      else
        // kein Filter nötig
        fgy = 0;
    }

  if (fgx > 0 || fgy > 0)
    {
      Tiefpass(orig, low, fgx, fgy);
      if (verbose)
        cout << "Tiefpass: fx= " << fgx << "   fy= " << fgy << endl;
    }
  else
    low.copy(orig);

  if (display & 2)
    Show(ON, low, "Tiefpass");

  int zx = orig->xsize / sub;
  int zy = orig->ysize / sub;

  Image res;
  res.create(zx, zy, mv);

  for (int y = 0; y < zy; y++)
    {
      int yo = y * sub + sy;

      for (int x = 0; x < zx; x++)
        {
          int xo = x * sub + sx;

          int g;
          if (is == 1)
            g = low.getPixelClipped(xo, yo);
          else
            {
              g = 0;

              for (int yh = yo; yh < yo + is; yh++)
                for (int xh = xo; xh < xo + is; xh++)
                  g += low.getPixelClipped(xh, yh);

              g = g / is / is;
            }

          res.setPixel(x, y, g);
        }
    }

  if (verbose)
    cout << "Resampled: " << zx << " x " << zy << endl;

  if (display & 1)
    Show(ON, res, "Resultat");

  if (!outfilename.empty())
    {
      res.write(outfilename);
      if (verbose)
        cout << "Ausgegeben als " << outfilename << endl;
    }
  if (display > 0)
    GetChar();

  return 0;
}

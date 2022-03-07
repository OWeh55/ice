#include <getopt.h>
#include <image_nonvis.h>
#include <VideoFile.h>

void usage(const string& pn)
{
  cout << pn << " <options> <filename_left> [<filename_right>] <last>" << endl;
  cout << "Erstellt einen [Stereo-]Film aus Einzelbildern" << endl;
  cout << "Die Filenamen fuer Bilder muessen eine C-Formatangabe fuer die Frame-Nummer enthalten." << endl;
  cout << "Die laufende Nummer beginnt bei 0 und endet bei <last>" << endl;
  cout << "options:" << endl;
  cout << "-h       diese Hilfe" << endl;
  cout << "-o <fn>  Ausgabe in die Datei fn (default:out.avi)" << endl;
  cout << "-s<mc>   Stereo-Modus" << endl;
  cout << "         mc = m (mono) Kein Stereo (default bei einem Filenamen)" << endl;
  cout << "         mc = r (red/cyan) Anaglyph(rot/cyan)" << endl;
  cout << "         mc = s (sidebyside) nebeneinander (default bei zwei Filenamen)" << endl;
  cout << "         mc = S (sidebyside) nebeneinander, halbe x Auflösung" << endl;
  cout << "         mc = o (overunder) übereinander" << endl;
  cout << "         mc = O (overunder) übereinander, halbe y Auflösung" << endl;
  cout << "         mc = i (interlaced) zeilenweise abwechselnd" << endl;
  cout << "-d <nr>  Framenummer-Offset für rechtes Bild" << endl;
  cout << "-S <ofs> Stereoshift" << endl;
  cout << "-v       Ausgabe der Frame-Nummern (verbose)" << endl;
  cout << "-V       Ausgabe von Aktivitäten (more verbose)" << endl;
  cout << "-z <fac> Zoom mit Faktor fac (fac>0 - Vergrößerung, fac<0 - Verkleinerung)" << endl;
  cout << "-f <fps> Frames pro Sekunde (default:25)" << endl;
  cout << "-r <br>  Bitrate (default:6000) - nur wirksam, wenn keine Qualitätseinstellung (-q) erfolgt" << endl;
  cout << "-q <qu>  Qualität der Kodierung (0..100) - überschreibt Einstellung der Bitrate" << endl;
  cout << "-b       Schwarz/Weiss-Modus" << endl;
  cout << "-c       Benutze convert zum Lesen der Bilder" << endl;
  cout << "-i       Inverse Reihenfolge" << endl;
  cout << "-a <n>   Start-Framenummer (default:0)" << endl;
  cout << "-A <n>   Startframe n mal zeigen" << endl;
  cout << "-L <n>   Endframe n mal zeigen" << endl;
  cout << "-m <m>   Zielbildgroesse als Vielfaches von m" << endl;
  exit(1);
}

void error(const string& pn, const string& msg)
{
  cout << "error: " << msg << endl;
  usage(pn);
}

string basename(const string& filename)
{
  string res = filename;
  int pos = res.length() - 1;

  while ((pos >= 0) && (res[pos] != '.')) pos--; // find last '.'

  if (pos >= 0) // Punkt gefunden?
    res = res.substr(0, pos);

  return res;
}

string nfilename(const string& fn, int fnr)
{
  char buffer[300];
  snprintf(buffer, 300, fn.c_str(), fnr);
  return buffer;
}

int main(int argc, char* argv[])
{
  string filename1;
  string filename2;

  string filenamed = "out.avi";

  int stshift = 0;
  int first = 0;
  int firstrep = 1;
  int last = 5;
  int lastrep = 1;
  int step = 1;
  bool inverse = false;

  int fac = 1;
  int div = 1;

  // video codec parameter
  int fps = 25;
  int brate = 6000;
  int offset = 0;

  int quality = -1;

  enum dmode_t { unknown, rotcyan, overunder, sidebyside,
                 overunder2, sidebyside2, interlaced, mono
               };

  dmode_t dmodus = unknown;
  int bsmod = 1;

  bool bw = false;
  bool verbose = false;
  bool moreverbose = false;
  bool useconvert = true;

  int rc;

  while ((rc = getopt(argc, argv, "m:d:a:ho:q:S:z:vVbs:cr:f:A:L:i")) >= 0)
    {
      switch (rc)
        {
        case 's':
        {
          char hmode = optarg[0];

          switch (hmode)
            {
            case 'm':
              dmodus = mono;
              break;
            case 'r':
              dmodus = rotcyan;
              break;
            case 's':
              dmodus = sidebyside;
              break;
            case 'S':
              dmodus = sidebyside2;
              break;
            case 'i':
              dmodus = interlaced;
              break;
            case 'o':
              dmodus = overunder;
              break;
            case 'O':
              dmodus = overunder2;
              break;
            default:
              usage(argv[0]);
            }
        }
        break;
        case 'c':
          useconvert = true;
          break;
        case 'a':
          first = atol(optarg);
          break;
        case 'd':
          offset = atol(optarg);
          break;
        case 'z':
          fac = atol(optarg);
          break;
        case 'S':
          stshift = atol(optarg);
          break;
        case 'r':
          brate = atol(optarg);
          break;
        case 'q':
          quality = atol(optarg);
          break;
        case 'f':
          fps = atol(optarg);
          break;
        case 'o':
          filenamed = optarg;
          break;
        case 'v':
          verbose = true;
          break;
        case 'V':
          verbose = moreverbose = true;
          break;
        case 'b':
          bw = true;
          break;
        case 'm':
          bsmod = atol(optarg);
          break;
        case 'L':
          lastrep = atol(optarg);
          break;
        case 'A':
          firstrep = atol(optarg);
          break;
        case 'i':
          inverse = true;
          break;
        //-----------------
        case 'h': // no break !!
        default:
          usage(argv[0]);
        }
    }

  if (argc > optind + 2)
    {
      filename1 = argv[optind];
      filename2 = argv[optind + 1];
      last = atol(argv[optind + 2]);

      if (dmodus == unknown)
        dmodus = sidebyside;

      if (dmodus == mono)
        error(argv[0], "Falsche Anzahl Parameter");
    }
  else if (argc > optind + 1)
    {
      filename1 = argv[optind];
      filename2 = argv[optind];
      last = atol(argv[optind + 1]);

      if (dmodus == unknown)
        dmodus = mono;

      if (dmodus != mono)
        error(argv[0], "Falsche Anzahl Parameter");
    }
  else
    {
      usage(argv[0]);
    }

  if (fac < 0)
    {
      div = -fac;
      fac = 1;
    }

  if (inverse)
    {
      int h = last;
      last = first;
      first = h;
      h = lastrep;
      lastrep = firstrep;
      firstrep = h;
      step = -1;
    }

  //  cout << nfilename(filename1,0) << " " << filename1 << endl;

  int xs1, ys1;
  int xs2, ys2;
  int mv = 255;
  int ch;

  if (useconvert)
    {
      filename1 = "PPM:|convert " + filename1 + " PPM:-";
      filename2 = "PPM:|convert " + filename2 + " PPM:-";
    }

  InfImgFile(nfilename(filename1, first), xs1, ys1, mv, ch); // Groesse aus 0. Frame
  InfImgFile(nfilename(filename2, first + offset), xs2, ys2, mv, ch); // Groesse aus 0. Frame

  VideoFile vfd;

  vfd.open(filenamed, ios_base::out);
  // cout << vfd.getpara() << endl;

#if 0
  char buffer[1000];

  //  snprintf(buffer,1000,"-ovc lavc -lavcopts vcodec=libx264:vbitrate=%d",brate);
  if (quality >= 0)
    snprintf(buffer, 1000, "-ovc x264 -x264encopts crf=%f", (1.0 + (100 - quality) * 0.49));
  else
    snprintf(buffer, 1000, "-ovc x264 -x264encopts bitrate=%d", brate);

  if (verbose)
    cout << buffer << endl;

  vfd.setpara(buffer);
#endif

  int xs, ys;

  xs = min(xs1, xs2);
  ys = min(ys1, ys2);


  // cout << xs << " " << ys << " " << mv << " "  << fac << endl;

  ColorImage in1;
  in1.create(xs1, ys1, mv);
  ColorImage in1s =   in1(Window(0, 0, xs - 1, ys - 1));

  ColorImage in2;
  in2.create(xs2, ys2, mv);
  ColorImage in2s = in2(Window(0, 0, xs - 1, ys - 1));

  ColorImage destimg;
  int xd = (xs + stshift) * fac / div;
  int yd = ys * fac / div;

  while (xd % bsmod > 0)
    xd--;

  while (yd % bsmod > 0)
    yd--;

  // cout << xd << " " << yd << " " << mv << endl;

  if (dmodus == sidebyside)
    {
      destimg.create(2 * xd, yd, mv);
    }
  else if (dmodus == overunder)
    {
      destimg.create(xd, 2 * yd, mv);
    }
  else
    {
      destimg.create(xd, yd, mv);
    }


  vfd.setPara(destimg.xsize,
              destimg.ysize,
              mv, fps);

  for (int fnr = first; fnr != last + step; fnr += step)
    {
      if (verbose)
        cout << "frame: " << fnr << endl;

      in1.read(nfilename(filename1, fnr));

      if (dmodus != mono)
        in2.read(nfilename(filename2, fnr + offset));

      if (moreverbose)
        cout << "read " << fnr << endl;

      for (int y = 0; y < yd; y++)
        {
          int yr = y * div / fac;

          for (int x = 0; x < xd; x++)
            {
              int xr = x * div / fac;

              ColorValue v1;

              if (xr < xs1)
                v1 = in1.getPixel(xr, yr);
              else
                v1 = ColorValue(255, 255, 255);

              if (bw)
                v1 = ColorValue(v1.getGray());

              ColorValue v2;

              if (dmodus != mono)
                {
                  int xs = xr - stshift;

                  if ((xs >= 0) && (xs < xs2))
                    v2 = in2.getPixel(xs, yr);
                  else
                    v2 = ColorValue(255, 255, 255);

                  if (bw)
                    v2 = ColorValue(v2.getGray());
                }

              switch (dmodus)
                {
                case mono:
                {
                  destimg.setPixel(x, y, v1.Limited(mv));
                }
                break;

                case rotcyan:
                {
                  ColorValue v(v1.red, v2.green, v2.blue);
                  destimg.setPixel(x, y, v.Limited(mv));
                }
                break;

                case interlaced:
                {
                  if (y % 2 == 0)
                    destimg.setPixel(x, y, v1.Limited(mv));
                  else
                    destimg.setPixel(x, y, v2.Limited(mv));
                }
                break;

                case sidebyside:
                {
                  destimg.setPixel(x, y, v1);
                  destimg.setPixel(x + xd, y, v2);
                }
                break;

                case sidebyside2:
                {
                  if (x % 2 == 0)
                    {
                      destimg.setPixel(x / 2, y, v1);
                      destimg.setPixel(x / 2 + xd / 2, y, v2);
                    }
                }
                break;


                case overunder:
                {
                  destimg.setPixel(x, y, v2);
                  destimg.setPixel(x, yd + y, v1);
                }
                break;

                case overunder2:
                {
                  if (y % 2 == 0)
                    {
                      destimg.setPixel(x, y / 2, v2);
                      destimg.setPixel(x, yd / 2 + y / 2, v1);
                    }
                }
                break;
                }
            }
        }

      if (moreverbose)
        cout << "writing: " << fnr << endl;

      if (fnr == first)
        for (int k = 0; k < firstrep; k++)
          vfd.write(destimg);
      else if (fnr == last)
        for (int k = 0; k < lastrep; k++)
          vfd.write(destimg);
      else
        vfd.write(destimg);

      if (moreverbose)
        cout << "wrote " << fnr << endl;

    }

  return OK;
}

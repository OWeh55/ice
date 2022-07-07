#include <unistd.h>
#include <stdlib.h>
#include <image.h>
#include <fstream>

class cImage
{
public:
  cImage(int x, int y, int min, int max)
  {
    r.create(x, y, min, max);
    g.create(x, y, min, max);
    b.create(x, y, min, max);
  }
  void clear()
  {
    r.set(0);
    g.set(0);
    b.set(0);
  }
  ImageD r, g, b;
};

void usage(const string& pn)
{
  cout << pn << endl;
  cout << "Verwendung:" << endl;
  cout << pn << " <options> filename" << endl;
  cout << "Liest die Videodatei filename und mittelt über alle Bilder" << endl;
  cout << "Optionen:" << endl;
  cout << "-f <n>   Erstes zu lesendes Bild (frame)" << endl;
  cout << "-l <n>   Letztes zu lesendes Bild" << endl;
  cout << "-x <x>   Zeitlich abklingender Mittelwert (default:0)" << endl;
  cout << "-O <fn>  Ausgabe des Ergebnisbildes als Datei fn" << endl;
  cout << "-o <fn>  Ausgabe als Videodatei fn" << endl;
  cout << "-z <n>   Zeitraffer, schreibt jedes n. Bild" << endl;
  cout << "-p <n>   Einrahmen (Vor- und Abspann) mit n frames Standbild" << endl;
  cout << "-d       Zeige Differenzbild an" << endl;
  cout << "-e       Wende Histogramm-Egalisierung auf Eingabebilder an" << endl;
  cout << "-n       Normalisiere Eingabebilder (Strecken auf 0..255)" << endl;
  cout << "-s <f>   Erhöhe Sättigung der Eingabebilder um Faktor f" << endl;
  cout << "-i       Setze Intensität auf 0.5" << endl;
  exit(1);
}

int main(int argc, char* argv[])
{
  string sourceFileName;
  string videoFileName;
  string imageFileName;

  int first = 0;
  int last = 0;
  int zeitraffer = 1;
  int zcount = 0;
  int framing = 0;
  bool firstFrame = true;

  bool hequal = false;
  bool normalise = false;
  double saturateFactor = 1.0;
  bool saturate = false;
  bool intensify = false;

  const int dMittel = 1;
  const int dDifference = 2;

  int output = dMittel;

  double expire = 0;

  int rc;
  while ((rc = getopt(argc, argv, "mM:o:z:ef:l:x:O:4nas:p:hdi")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          first = atol(optarg);
          break;
        case 'l':
          last = atol(optarg);
          break;
        case 'e':
          hequal = true;
          break;
        case 'o':
          videoFileName = optarg;
          break;
        case 'O':
          imageFileName = optarg;
          break;
        case 'z':
          zeitraffer = atol(optarg);
          break;
        case 'p':
          framing = atol(optarg);
          break;
        case 'x':
          expire = atof(optarg);
          break;
        case 'n':
          normalise = true;
          break;
        case 's':
          saturateFactor = atof(optarg);
          saturate = true;
          break;
        case 'i':
          intensify = true;
          break;
        case 'd':
          output = dDifference;
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc > optind)
    {
      sourceFileName = argv[optind];
    }
  else
    {
      cerr << argv[0] << " <filename>" << endl;
      exit(2);
    }

  int xs1, ys1;
  int mv, fps;

  VideoFile vf(sourceFileName);

  vf.getPara(xs1, ys1, mv, fps);

  Printf("input format %d x %d    0 .. %d   %d fps\n", xs1, ys1, mv, fps);

  VideoFile vfd;
  if (!videoFileName.empty())
    {
      vfd.open(videoFileName, ios_base::out);
      //    cout << vfd.getpara() << endl;
      //    vfd.setpara("-ovc lavc -lavcopts vcodec=mpeg4:vbitrate=2400");
      vfd.setPara(xs1, ys1, mv, fps);

    }

  int xs, ys;

  xs = xs1;
  ys = ys1;

  ColorImage in1;
  in1.create(xs1, ys1, mv);
  Show(ON, in1, sourceFileName);

  ColorImage in1t;
  in1t.create(xs1, ys1, mv); // preprocessed image

  bool ok = true;

  while (ok && first > vf.FrameNumber())
    if (!vf.read(in1))
      ok = false;

  bool preprocess = false;

  //  cout << xs << " " << ys << endl;
  if (normalise || saturate || hequal || intensify)
    {
      preprocess = true;
      Show(ON, in1t, "Vorverarbeitet");
    }

  cImage summe(xs, ys, 0, 20000000);
  summe.clear();

  ColorImage result;
  result.create(xs, ys, mv);
  result.clear();
  Show(ON, result, "Resultat");

  double ct = 0;
  double expire1 = 1.0 - expire;
  int frameNr = 0;
  do
    {
      ok = vf.read(in1);

      ct = ct * expire1 + 1.0;

      Printf("Frame: %d ", vf.FrameNumber());

      if (ok)
        {
          // create copy
          in1t.copy(in1);

          // color manipulation
          if (saturate || intensify)
            {
              WindowWalker w(in1);
              for (w.init(); !w.ready(); w.next())
                {
                  ColorValue v = in1.getPixel(w);
                  double h, s, i;
                  RgbToHsi(v, h, s, i);
                  //h = 0.0;
                  //i = 0.5;
                  if (intensify)
                    i = 0.5;
                  if (saturate)
                    {
                      s = s * saturateFactor;
                      if (s > 1.0)
                        s = 1.0;
                    }
                  HsiToRgb(h, s, i, v, mv);
                  in1t.setPixelLimited(w, v);
                }
            }

          if (hequal)
            {
              HistogramEqualization(in1.redImage(), in1t.redImage());
              HistogramEqualization(in1.greenImage(), in1t.greenImage());
              HistogramEqualization(in1.blueImage(), in1t.blueImage());
            }

          if (normalise)
            {
              GrayNormalize(in1.redImage(), in1t.redImage(), GV_QUANTILE);
              GrayNormalize(in1.greenImage(), in1t.greenImage(), GV_QUANTILE);
              GrayNormalize(in1.blueImage(), in1t.blueImage(), GV_QUANTILE);
            }

#if 0
          WindowWalker w(in1t);

          for (w.init(); !w.ready(); w.next())
            {
              ColorValue v1 = preprocess ? in1t.getPixelUnchecked(w) : in1.getPixelUnchecked(w);

              double sr = summe.r.getPixelUnchecked(w) * expire1 + v1.red;
              double sg = summe.g.getPixelUnchecked(w) * expire1 + v1.green;
              double sb = summe.b.getPixelUnchecked(w) * expire1 + v1.blue;

              summe.r.setPixelUnchecked(w, sr);
              summe.g.setPixelUnchecked(w, sg);
              summe.b.setPixelUnchecked(w, sb);

              double r = sr / ct;
              double g = sg / ct;
              double b = sb / ct;
              double dr = r - v1.red;
              double dg = g - v1.green;
              double db = b - v1.blue;

              switch (output)
                {
                case dMittel:
                  // result.setPixelLimited(w, ColorValue(r, g, b));
                  result.setPixelUnchecked(w, ColorValue(r, g, b));
                  break;
                case dDifference:
                  result.setPixelLimited(w, ColorValue(dr, dg, db) + ColorValue(mv / 2));

                  break;
                } // switch
            }
#else

#ifdef OPENMP
          #pragma omp parallel for schedule(dynamic,30)
#endif
          for (int y = 0; y < ys; y++)
            for (int x = 0; x < xs; x++)
              {
                IPoint w(x, y);
                ColorValue v1 = preprocess ? in1t.getPixelUnchecked(w) : in1.getPixelUnchecked(w);

                double sr = summe.r.getPixelUnchecked(w) * expire1 + v1.red;
                double sg = summe.g.getPixelUnchecked(w) * expire1 + v1.green;
                double sb = summe.b.getPixelUnchecked(w) * expire1 + v1.blue;

                summe.r.setPixelUnchecked(w, sr);
                summe.g.setPixelUnchecked(w, sg);
                summe.b.setPixelUnchecked(w, sb);

                double r = sr / ct;
                double g = sg / ct;
                double b = sb / ct;
                double dr = r - v1.red;
                double dg = g - v1.green;
                double db = b - v1.blue;

                switch (output)
                  {
                  case dMittel:
                    // result.setPixelLimited(w, ColorValue(r, g, b));
                    result.setPixelUnchecked(w, ColorValue(r, g, b));
                    break;
                  case dDifference:
                    result.setPixelLimited(w, ColorValue(dr, dg, db) + ColorValue(mv / 2));

                    break;
                  } // switch
              }

#endif

          zcount++;
          if (zcount == zeitraffer)
            {
              Print("+");
              if (vfd.isOpen())
                {
                  vfd.write(result);
                  if (firstFrame)
                    {
                      for (int n = 0; n < framing; n++)
                        vfd.write(result);
                      firstFrame = false;
                    }
                }
              zcount = 0;
            }
        }
      frameNr++;
      Print("\n");
    }
  while (ok && ((last == 0) || (last > vf.FrameNumber())));

  if (vfd.isOpen())
    for (int n = 0; n < framing; n++)
      vfd.write(result);

  if (!imageFileName.empty()) // if image output file given
    {
      result.write(imageFileName);
    }

  if (videoFileName.empty() && imageFileName.empty()) // interactive usage ?
    GetChar();
  return OK;
}

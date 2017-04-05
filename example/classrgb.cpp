// test zu den Klassifikatoren
// benutzt Farbwerte der Pixel eines Bildes
// als Parameter

#include <image.h>

//#define PROBS 0
#define REGIONS 1

#define CLASSES 3
int classes = CLASSES;

#define FEATURES 1
int features = FEATURES;

#define NEIGHBOURS 1
int neighbours = NEIGHBOURS;

bool combining = false;
bool writereadtest = false;
bool timing = false;
bool sortieren = false;

#define tagfilename basename(filename)+"_tags.bmp"

string basename(const string& filename)
{
  string res = filename;
  int pos = res.length() - 1;

  while ((pos >= 0) && (res[pos] != '.')) pos--; // find first '.'

  res = res.substr(0, pos);
  return res;
}

void rgbhsi(double r, double g, double b, double& h, double& s, double& i)
{
  double sum = r + g + b;

  if (sum == 0.0)
    {
      h = s = i = 0.0;
      return;
    }

  double fx = r - 0.5 * (g + b) / sum;
  double fy = (g - b) * 0.866 / sum;
  i = sum * 0.3333333;
  s = sqrt(fx * fx + fy * fy);
  h = atan2(fy, fx);
}

void getPixel(const Image& r, const Image& g, const Image& b,
              int x, int y, Vector& feat1, Vector& feat2, Vector& feat3)
{
  double ir = r->maxval - GetVal(r, x, y);
  double ig = g->maxval - GetVal(g, x, y);
  double ib = b->maxval - GetVal(b, x, y);
  double yy = ir + ig + ib;

//  cout << feat1.size() << " " << feat1.Alloc() << endl;
  switch (features)
    {
    case 1:
      feat1.Append(ir / 255.0);
      feat2.Append(ig / 255.0);
      feat3.Append(ib / 255.0);
      break;
    case 2:
    {
      double h, s, i;
      rgbhsi(ir, ig, ib, h, s, i);
      feat1.Append(h);
      feat2.Append(s);
      feat3.Append(i / 255);
    }
    break;
    case 3:
    {
      double u = ir + 0.5 * (ig + ib);
      double v = (ig - ib) * 0.82;
      feat1.Append(u / yy);
      feat2.Append(v / yy);
      feat3.Append(yy / 255);
    }
    break;
    }
}

void getFeatures(const Image& r, const Image& g, const Image& b,
                 int x, int y, Vector& feat1)
{
  Vector feat2, feat3;
  feat1 = Vector();

  // fenster am Rand in das Bild verschieben
  if (x < neighbours)
    x = neighbours;

  if (y < neighbours)
    y = neighbours;

  if (x >= r->xsize - neighbours)
    x = r->xsize - neighbours - 1;

  if (y >= r->ysize - neighbours)
    y = r->ysize - neighbours - 1;

  // alle Nachbarn aufsammeln
  for (int dy = -neighbours; dy <= neighbours; dy++)
    for (int dx = -neighbours; dx <= neighbours; dx++)
      {
        getPixel(r, g, b, x + dx, y + dy, feat1, feat2, feat3);
      }

  if (sortieren)
    {
      feat1.Sort();
      feat2.Sort();
      feat3.Sort();
    }

  feat1.Append(feat2);
  feat1.Append(feat3);
  int osize = feat1.size();

  if (combining)
    {
      for (int i = 0; i < osize; i++)
        for (int j = 0; j < osize; j++)
          feat1.Append(feat1[i]*feat1[j]);
    }
}

int usage(const string& pn)
{
  cout << "Aufruf: " << pn << " [<options>] [filename]" << endl;
  cout << "options:" << endl;
  cout << "-f n   Merkmalssatz m verwenden (default " << FEATURES << ")" << endl;
  cout << "       1 - RGB " << endl;
  cout << "       2 - HSI " << endl;
  cout << "       3 - YUV " << endl;
  cout << "-m     'Merkmals-Produkte' verwenden" << endl;
  cout << "-w     Ausschreiben und Einlesen des Klassifikators testen" << endl;
  cout << "-n n   Umgebungsgröße für Merkmale (default " << NEIGHBOURS << ")" <<  endl;
  cout << "-s n   SoftClassify mit Bewertung n verwenden" << endl;
  cout << "-p     Punkte als Klassen-Repräsentant wählen" << endl;
  cout << "-i     Lernstichprobe aus Bilddatei einlesen" << endl;
  cout << "-o     Lernstichprobe als Bilddatei ausschreiben" << endl;
  cout << "-O     Ergebnisbild ausschreiben" << endl;
  cout << "-t     Timing durchführen" << endl;
  cout << "-c n   Zahl der Klassen festlegen (default:" << CLASSES << ")" << endl;
  cout << "-a     Sortieren der Werte aus der Umgebung" << endl;
  cout << "-h     Diese Hilfe ausgeben" << endl;
  exit(1);
}

int main(int argc, char* argv[])
{
  string filename = "test_rgb.jpg"; // filename for testimage
  bool readtags = false; // read class tags from file? (image)
  bool writetags = false; // write class tags to file? (image)
  bool writeresult = false; // write result as class tags to file? (image)
  int softclassify = -1; // use SoftClassify with mode n and display probabilities
  int selectpoints = 0; // Select n single points only
  int rc;

  while ((rc = getopt(argc, argv, "ac:f:imn:oOp:s:twh")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          features = atol(optarg);
          break;
        case 'm':
          combining = true;
          break;
        case 'w':
          writereadtest = true;
          break;
        case 'n':
          neighbours = atol(optarg);
          break;
        case 's':
          softclassify = atol(optarg);
          break;
        case 'p':
          selectpoints = atol(optarg);
          break;
        case 'i':
          readtags = true;
          break;
        case 'o':
          writetags = true;
          break;
        case 'O':
          writeresult = true;
          break;
        case 't':
          timing = true;
          break;
        case 'c':
          classes = atol(optarg);
          break;
        case 'a':
          sortieren = true;
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  //  cout << optind << " " << argc << endl;
  if (argc > optind) filename = argv[optind];

  Image r, g, b, m1, m2;

  int xs, ys, mv;
  int x, y;
  int c;

  Display(ON);
  Alpha(ON);
  ClearAlpha();
  InfImgFile(filename, xs, ys, mv);
  r = NewImg(xs, ys, mv);
  g = NewImg(xs, ys, mv);
  b = NewImg(xs, ys, mv);
  Show(_RGB, r, g, b, "Ausgangsbild");

  ReadImg(filename, r, g, b);

  m1 = NewImg(r);
  clearImg(m1);

  if (readtags)
    {
      ReadImg(basename(filename) + "_tags.bmp", m1);
      WindowWalker w(m1);
      for (w.init(); !w.ready(); w.next())
        {
          int clnr = m1.getPixel(w) - 1;

          if (clnr >= 0)
            {
              if (clnr >= classes) classes = clnr + 1;
            }
        }
    }

  vector<Image> probs(classes); // Bewertungen der Klassen

  if (softclassify >= 0)
    {
      for (int i = 0; i < classes; i++)
        {
          probs[i] = NewImg(r, 255);
          Show(ON, probs[i], "Klasse " + NumberString(i));
        }
    }


  //  cout << basename(filename) << endl;

  if (writereadtest)
    {
      m2 = NewImg(r);
      clearImg(m2);
    }

  Vector feat;
  getFeatures(r, g, b, 5, 5, feat);
  Printf("Klassifikator mit %d Merkmalen\n", feat.Size());

  //  ClassifierPE cl(classes,feat.Size());
  //  ClassifierNN cl(classes,feat.Size());
  // ClassifierMD cl(classes,feat.Size());
  // ClassifierB2 cl(2,2,0.5,0.5,1,1);

  //  ClassifierNBay cl(classes,feat.Size());

  //ClassifierLinear cl(classes, feat.Size());
  ClassifierBayes cl(classes, feat.Size());

  //   vector<double> ap(classes);
  //   for (int i=0;i<classes;i++)
  //     ap[i]=1.0/classes;
  //   ClassifierBayes cl(feat.size(),ap);

  int samples = 0;
  Show(OVERLAY, r, m1);

  if (!readtags) // interaktive Auswahl
    {
      if (selectpoints == 0)
        {
          Print("Select regions\n");

          for (c = 0; c < classes; c++)
            {
              Printf("class %d\n", c);
              Contur cn = SelContur(m1, true);
              FillRegion(cn, c + 1, m1);
              MarkContur(cn, c + 1, m1);
            }
        }
      else
        {
          Print("Select points\n");

          for (c = 0; c < classes; c++)
            {
              Printf("class %d\n", c);

              for (int p = 0; p < selectpoints; p++)
                {
                  IPoint pp = SelPoint(m1);
                  PutVal(m1, (int)pp.X(), (int)pp.Y(), c + 1);
                }
            }
        }
    }
  else // read class tags
    {
      ReadImg(basename(filename) + "_tags.bmp", m1);
    }

  samples = 0;
  for (int y = 0; y < m1.ysize; y++)
    for (int x = 0; x < m1.xsize; x++)
      {
        int clnr = GetVal(m1, x, y) - 1;

        if (clnr >= 0)
          {
            getFeatures(r, g, b, x, y, feat);
            cl.Train(clnr, feat);
            samples++;
          }
        // fortschrittsanzeige, nur wenn tags nicht mehr benötigt
        if (!writetags && x & 1 && y & 1)
          PutVal(m1, x, y, 0);
      }
  Printf("Training beendet (%d samples)\n", samples);

  if (writetags) WriteImg(m1, basename(filename) + "_tags.bmp");

  //  cl.Write("testclassifier_not_finished");
  double ta = TimeD(TM_PROCESS);
  cl.Finish();
  double tt = TimeD(TM_PROCESS) - ta;
  double tf = 0;

  if (timing)
    {
      Printf("Finish dauerte %g s\n", tt);

      // Testschleife für Laufzeit Featureermittlung
      ta = TimeD(TM_PROCESS);
      for (int y = 0; y < m1.ysize; y++)
        for (int x = 0; x < m1.xsize; x++)
          {
            getFeatures(r, g, b, x, y, feat);
          }

      tf = TimeD(TM_PROCESS) - ta;

      Printf("Merkmalsberechnung dauert %g s\n", tf);
    }

  ta = TimeD(TM_PROCESS);
  for (int y = 0; y < m1.ysize; y++)
    for (int x = 0; x < m1.xsize; x++)
      {
        getFeatures(r, g, b, x, y, feat);
#if 0
        if (softclassify >= 0)
          {
            Vector v(classes * 2);
            int cs = cl.SoftClassify(feat, v, softclassify) + 1;
            PutVal(m1, x, y, cs);

            //  cout << v << endl;
            for (int i = 0; i < classes; i++)
              {
                PutVal(probs[(int)v[i * 2]], x, y, (int)(v[i * 2 + 1] * 255));
              }
          }
        else
#endif
          {
            PutVal(m1, x, y, cl.Classify(feat) + 1);
          }
      }
  double tc = TimeD(TM_PROCESS) - ta;

  Printf("Klassifikation beendet.\n");

  if (timing)
    {
      Printf("Klassifikation dauert %g s\n", tc);
      Printf("Klassifikation dauert %g us pro Pixel\n",
             (tc - tf) / r->xsize / r->ysize * 1e6);
    }

  if (writeresult) WriteImg(m1, basename(filename) + "_tags.bmp");

  GetChar();

#if 0
  WriteImg(c1, "rot.jpg");
  WriteImg(c2, "gruen.jpg");
  WriteImg(c3, "blau.jpg");
#endif

  if (writereadtest)
    {
      cl.write("testclassifier");

      Show(OVERLAY, r, m2);

      ClassifierBayes clx;
      //ClassifierLinear clx;
      clx.read("testclassifier");

      ta = TimeD(TM_PROCESS);
      for (int y = 0; y < m1.ysize; y++)
        for (int x = 0; x < m1.xsize; x++)
          {
            getFeatures(r, g, b, x, y, feat);
            PutVal(m2, x, y, clx.Classify(feat) + 1);
          }
      tc = TimeD(TM_PROCESS) - ta;

      Printf("Klassifikation dauert %g s\n", tc);
      Printf("Klassifikation dauert %g us pro Pixel\n",
             (tc - tf) / r->xsize / r->ysize * 1e6);
      GetChar();
    }

  return 0;
}

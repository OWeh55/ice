#include <image.h>
#include <fstream>

const int m_free = 0;
const int m_marker = 255;

void usage(const string& pname)
{
  cout << pname << " - Kalibriermustererzeugung" << endl << endl;
  cout << "Aufruf" << "  " << pname << " [<options>]*" << endl << endl;
  cout << "Optionen:" << endl;

  cout << "-o outfile Ausgabe des Musters als Bildes als \"outfile\"" << endl;
  cout << "           Standard: keine Ausgabe" << endl;

  cout << "-c include Ausgabe des Polygons als C-Datei include" << endl;
  cout << "           Standard: keine Ausgabe" << endl;

  cout << "-x         Breite des Musters" << endl;
  cout << "-y         Hoehe des Musters" << endl;
  cout << "-f         Vergroesserungsfaktor fuer bildliche Darstellung (default: 17)" << endl;
  cout << "-m  d      Markerabstand d (d>=4, default: d=4)" << endl;
  cout << "-M  i      Markertyp" << endl;
  cout << "           i=1 Loch (default)" << endl;
  cout << "           i=2 Kreuz" << endl;
  cout << "           i=3 Loch - breite Verbindungen" << endl;
  cout << "-R n       n Lochreihen als Rand ohne Verbindungen" << endl;
  cout << "-r n       Initialisierung des Zufallszahlengenerators mit n (default:12345)" << endl;

  cout << "-d         Darstellung der Bilder" << endl;
  cout << "-w         Warten auf Tastendruck" << endl;

  cout << "-h         Ausgabe dieser Hilfe" << endl;
  exit(0);
}

void error(const string& pname, const string& msg)
{
  cout << "Error: " << msg << endl << endl;
  usage(pname);
}

void setMarkerA(Image& lab, int x, int y, int val)
{
  if (lab.inside(x - 1, y - 1) && lab.inside(x + 1, y + 1))
    {
      PutVal(lab, x - 1, y - 1, val);
      PutVal(lab, x, y - 1, val);
      PutVal(lab, x + 1, y - 1, val);
      PutVal(lab, x - 1, y, val);
      PutVal(lab, x + 1, y, val);
      PutVal(lab, x - 1, y + 1, val);
      PutVal(lab, x, y + 1, val);
      PutVal(lab, x + 1, y + 1, val);
    }
}

void setMarkerB(Image& lab, int x, int y, int val)
{
  if (lab.inside(x - 1, y - 1) && lab.inside(x, y))
    {
      PutVal(lab, x - 1, y, val);
      PutVal(lab, x, y - 1, val);
      PutVal(lab, x, y + 1, val);
      PutVal(lab, x, y, val);
      PutVal(lab, x + 1, y, val);
    }
}

Polygon getPoly(const Contur& c)
{
  Polygon poly;
  LimitingPolygon(c, poly);
  // cout << poly.size() <<endl;
  Polygon rpoly = poly.ReducedToPrecision(1.5);
  return rpoly;
}

void WritePoly(ofstream& cf, const Polygon& rpoly)
{
  cf << "const int pattern_corners=" << rpoly.size() << ";" << endl;
  cf << "const double pattern_x[pattern_corners]={" << endl << rpoly[0].x;
  for (int i = 1; i < rpoly.size(); i++)
    cf << "," << rpoly[i].x;
  cf << "};" << endl;
  cf << "const double pattern_y[pattern_corners]={" << endl << rpoly[0].y;
  for (int i = 1; i < rpoly.size(); i++)
    cf << "," << rpoly[i].y;
  cf << "};" << endl;
}

void WriteGrating(ofstream& cf,
                  int cols, double x0, double dx,
                  int rows, double y0, double dy)
{
  cf << "const int pattern_cols=" << cols << ";" << endl;
  cf << "const double pattern_x0=" << x0  << ";" << endl;
  cf << "const double pattern_dx=" << dx  << ";" << endl;

  cf << "const int pattern_rows=" << rows  << ";" << endl;
  cf << "const double pattern_y0=" << y0  << ";" << endl;
  cf << "const double pattern_dy=" << dy  << ";" << endl;
}

int main(int argc, char** argv)
{
  int rseed = 12345;

  int xs = 90;
  int ys = 64;
  int mdist = 4;
  int mtyp = 1;
  int display = false;
  int wait = false;
  int factor = 17;
  int rand = 0;
  string cfilename = "";
  string imgfilename = "";
  int rc;

  // Alpha(ON);

  while ((rc = getopt(argc, argv, "hx:y:m:M:f:dwc:o:r:R:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          xs = atoi(optarg);
          break;
        case 'y':
          ys = atoi(optarg);
          break;
        case 'M':
          mtyp = atoi(optarg);
          break;
        case 'm':
          mdist = atoi(optarg);
          break;
        case 'f':
          factor = atoi(optarg);
          break;
        case 'd':
          display = true;
          break;
        case 'w':
          wait = true;
          break;
        case 'c':
          cfilename = optarg;
          break;
        case 'o':
          imgfilename = optarg;
          break;
        case 'r':
          rseed = atoi(optarg);
          break;
        case 'R':
          rand = atoi(optarg);
          break;
        case 'h': // no break
        default:
          usage(argv[0]);
        }
    }

  if (mdist < 4)
    mdist = 4;

  if (wait)
    Alpha(ON);

  Image lab;
  lab.create(xs, ys, 255);

  if (display)
    Show(ON, lab);

  SetImg(lab, m_free);

  vector<Point> markers;

  srand(rseed);

  int nrow = (lab->ysize - 1) / mdist;
  int ncol = (lab->xsize - 1) / mdist;

  for (int iy = 0; iy < nrow; iy++)
    {
      int y = iy * mdist;
      for (int ix = 0; ix < ncol; ix++)
        {
          int x = ix * mdist;
          switch (mtyp)
            {
            case 1:
            case 3:
              setMarkerA(lab, x + 2, y + 2, m_marker);
              break;
            case 2:
              setMarkerB(lab, x + 2, y + 2, m_marker);
              break;
            default:
              error(argv[0], "Wrong pattern type");
            }
          if (iy >= rand && ix >= rand && iy < nrow - rand && ix < ncol - rand)
            markers.push_back(Point(x + 2 + RandomD() * 0.3, y + 2 + RandomD() * 0.3));
        }
    }

  Forest mintree;
  computeMinTree(markers, mintree); // minimal aufspannenden Baum ermitteln

  for (int i = 0; i < mintree.Size(); i++)
    {
      int father = mintree.Father(i); // Vater des aktuellen Knotens
      if (father != Forest::rootval) // Knoten ist nicht die Wurzel
        {
          int x1 = (int)markers[i].x;
          int y1 = (int)markers[i].y;
          int x2 = (int)markers[father].x;
          int y2 = (int)markers[father].y;
          if (x1 == x2) // senkrecht
            {
              if (y1 > y2)
                {
                  swap(y1, y2);
                }
              for (int y = y1 + 1; y < y2 - 1; y++)
                {
                  PutVal(lab, x1, y, m_marker);
                  if (mtyp > 2)
                    {
                      PutVal(lab, x1 - 1, y, m_marker);
                      PutVal(lab, x1 + 1, y, m_marker);
                    }
                }
            }
          else
            {
              if (y1 != y2)
                cerr << "Baum falsch!!" << endl;
              if (x1 > x2)
                swap(x1, x2);
              for (int x = x1 + 1; x < x2 - 1; x++)
                {
                  PutVal(lab, x, y1, m_marker);
                  if (mtyp > 2)
                    {
                      PutVal(lab, x, y1 + 1, m_marker);
                      PutVal(lab, x, y1 - 1, m_marker);
                    }
                }
            }
        }
    }

  Image big;
  big.create(xs * factor, ys * factor, 255);
  Image mark;
  mark.create(xs * factor, ys * factor, 255);

  if (display)
    Show(OVERLAY, big, mark);

  for (int y = 0; y < big->ysize; y++)
    {
      int yo = y / factor;
      for (int x = 0; x < big->xsize; x++)
        {
          int xo = x / factor;
          PutVal(big, x, y, GetVal(lab, xo, yo));
        }
    }

  ClearImg(mark);
  //  IPoint start(0, 0);
  IPoint start((rand * mdist)*factor, (rand * mdist + 2)*factor);

  Contur c;
  Polygon p;
  if (SearchStart(big, mark, NULL, 251, 1, start) == OK)
    {
      c = CalcContur(big, mark, NULL, 251, start);

      MarkContur(c, 1, mark);
      p = getPoly(c);
      draw(p, mark, 2);
      // cout << p.size() <<endl;
      for (int i = 0; i < p.size(); i++)
        Marker(2, p[i], 3, factor / 2, mark);
    }

  if (wait)
    {
      GetChar();
      ClearImg(mark);
      GetChar();
    }

  if (!imgfilename.empty())
    WriteImg(big, imgfilename);

  if (!cfilename.empty())
    {
      ofstream cf(cfilename.c_str());
      WritePoly(cf, p);
      WriteGrating(cf,
                   ncol, 2.5 * factor - 0.5, mdist * factor,
                   nrow, 2.5 * factor - 0.5, mdist * factor);
    }

  return OK;
}

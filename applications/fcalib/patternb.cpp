#include <image.h>
#include <fstream>

void usage(const string &pname)
{
  cout << pname << " - Kalibriermustererzeugung" << endl << endl;
  cout << "Aufruf" << "  " << pname << " [<options>]*" << endl << endl;
  cout << "Optionen:" << endl;

  cout << "-o outfile Ausgabe des Musters als Bildes als \"outfile\"" << endl;
  cout << "           Standard: keine Ausgabe" << endl;

  cout << "-c include Ausgabe des Polygons als C-Datei include" << endl;
  cout << "           Standard: keine Ausgabe" << endl;

  cout << "-x         Zahl der Marker in X-Richtung" << endl;
  cout << "-y         Zahl der Marker in Y-Richtung" << endl;
  cout << "-f         Vergroesserungsfaktor fuer bildliche Darstellung (default: 17)" << endl;
  cout << "-r n       n Lochreihen als Rand" << endl;
  cout << "-p n       weißen Rand entsprechend n Reihen ergänzen" << endl;

  cout << "-d         Darstellung der Bilder" << endl;
  cout << "-D         Darstellung der Bilder ohne Marker für Contur" << endl;
  cout << "-w         Warten auf Tastendruck" << endl;

  cout << "-h         Ausgabe dieser Hilfe" << endl;
  exit(0);
}

void error(const string &pname, const string &msg)
{
  cout << "Error: " << msg << endl << endl;
  usage(pname);
}

void setMarkerA(Image &lab, int x, int y)
{
  int val = lab.maxval;
  for (int xo = -1; xo < 2; ++xo)
    for (int yo = -1; yo < 2; ++yo)
      {
        PutVal(lab, x + xo, y + yo, val);
      }
  PutVal(lab, x, y, 0);
}

void WritePoly(ofstream &cf, int x1, int y1, int x2, int y2, int x3, int y3)
{
  cf << "const int pattern_corners = 6;" << endl;
  cf << "const double pattern_x[pattern_corners]={" << endl;
  cf << x1 << "," << x2 + 1 << "," << x2 + 1 << "," << x3 + 1 << "," << x3 + 1 << "," << x1;
  cf << "};" << endl;
  cf << "const double pattern_y[pattern_corners]={" << endl;
  cf << y1 << "," << y1 << "," << y3 + 1 << "," << y3 + 1 << "," << y2 + 1 << "," << y2 + 1;
  cf << "};" << endl;
}

void WriteGrating(ofstream &cf,
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

int main(int argc, char **argv)
{
  int padding = 1;
  int rand = 3;

  int nCols = 30;
  int nRows = 20;
  int display = false;
  int displayc = false;
  int wait = false;
  int factor = 12;
  string cfilename = "";
  string imgfilename = "";
  int rc;

  // Alpha(ON);

  while ((rc = getopt(argc, argv, "hx:y:f:dDwc:o:r:p:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          nCols = atoi(optarg);
          break;
        case 'y':
          nRows = atoi(optarg);
          break;
        case 'f':
          factor = atoi(optarg);
          break;
        case 'd':
          display = true;
          break;
        case 'D':
          displayc = true;
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
        case 'p':
          padding = atoi(optarg);
          break;
        case 'r':
          rand = atoi(optarg);
          break;
        case 'h': // no break
        default:
          usage(argv[0]);
        }
    }

  if (displayc) // darstellung für Bildaufnahme
    {
      wait = true;
      display = true;
    }

  if (nCols % 2 != 0)
    --nCols;
  int xs = nCols * 4 + 1;

  if (nRows % 2 != 0)
    --nRows;
  int ys = nRows * 4 + 1;

  if (wait)
    Alpha(ON);

  Image lab;
  lab.create(xs, ys, 255);

  if (display)
    Show(ON, lab);

  lab.set(0);

  int bx1 = rand * 4 + 1;
  int by1 = rand * 4 + 1;
  int bx2 = (nCols - 1 - rand) * 4 + 3;
  int by2 = (nRows - 1 - rand) * 4 + 3;
  int bx3 = nCols / 2 * 4 - 1;
  int by3 = nRows / 2 * 4 - 1;

  //cout << bx1 << " " << bx2 << " " << bx3 << endl;
  //cout << by1 << " " << by2 << " " << by3 << endl;
  lab(Window(bx1, by1, bx2, by3)).set(lab.maxval);
  lab(Window(bx1, by1, bx3, by2)).set(lab.maxval);

  for (int iy = 0; iy < nRows; iy++)
    {
      int y = iy * 4;
      for (int ix = 0; ix < nCols; ix++)
        {
          int x = ix * 4;
          setMarkerA(lab, x + 2, y + 2);
        }
    }

  Image out; // vergrößertes Bild mit Rand
  out.create((xs + padding * 4 * 2) * factor, (ys + padding * 4 * 2) * factor, 255);
  out.set(0);
  Image big(out(Window(padding * 4 * factor, padding * 4 * factor, (padding * 4 + xs)*factor - 1, (padding * 4 + ys)*factor - 1)));

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

  if (wait)
    {
      GetChar();
    }

  if (!imgfilename.empty())
    WriteImg(out, imgfilename);

  if (!cfilename.empty())
    {
      ofstream cf(cfilename.c_str());
      WritePoly(cf, bx1, by1, bx2, by2, bx3, by3);
      WriteGrating(cf,
                   nCols, 2.5, 4,
                   nRows, 2.5, 4);
    }

  return OK;
}

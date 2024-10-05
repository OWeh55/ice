#include <image.h>

#define ANZAHL 30
#define XS 1024
#define YS 1024

void usage(const string& prog)
{
  cout << prog << ":" << endl;
  cout << "Aufruf: " << prog << " [options] <filename>" << endl;
  cout << "options:" << endl;
  cout << "-l    Level, setzt Level2 auf den gleichen Wert" << endl;
  cout << "-L    Level2, muss nach -l kommen, damit der Wert nicht wieder überschrieben wird." << endl;
  cout << "-n    Umgebungsgröße" << endl;
  cout << "-a    Anzahl der Durchläufe (default:" << ANZAHL << " für Benchmarking)" << endl;
  cout << "-h    Hilfe" << endl;
  exit(2);
}

int main(int argc, char* argv[])
{
  int x, y;
  int anz = 0;
  int rc;

  int level = 50;
  int level2 = 20;
  int neighbor = 11;

  string outfilename;
  bool colorout = false;
  bool grayout = false;

  Image i1;

  while ((rc = getopt(argc, argv, "hl:L:n:a:o:O:")) >= 0)
    {
      switch (rc)
        {
        case 'h':
          usage(argv[0]);
          break;
        case 'l':
          level = atol(optarg); // nobreak;
        case 'L':
          level2 = atol(optarg);
          break;
        case 'n':
          neighbor = atol(optarg);
          break;
        case 'a':
          anz = atol(optarg);
          break;
        case 'o':
          outfilename = optarg;
          grayout = true;
          break;
        case 'O':
          outfilename = optarg;
          colorout = true;
          break;
        default:
          usage(argv[0]);
        }
    }

  if (argc - optind > 0) // mindestens ein verbleibende Parameter = Dateiname
    {
      string fn = argv[optind];
      i1 = ReadImg(fn);

      if (anz == 0) anz = 1;
    }
  else
    {
      // artificial image for runtime check
      i1 = NewImg(XS, YS, 255);

      for (int y = 0; y < i1.ysize; y++)
        for (int x = 0; x < i1.xsize; x++)
          {
            if ((x / 20) & 1) PutVal(i1, x, y, y / 4);
            else PutVal(i1, x, y, 255 - (y / 4));
          }

      if (anz == 0) anz = ANZAHL;
    }

  Image m1 = NewImg(i1);
  clearImg(m1);

//  Image mi=NewImg(i1);
//  Image ma=NewImg(i1);

//  Show(ON,mi);
//  Show(ON,ma);

//  MinMaxImg(i1,5,5,mi,ma);

  Show(ON, i1);
  Show(OVERLAY, i1, m1);

  double ta = TimeD(TM_PROCESS);

  for (int i = 0; i < anz; i++)
    LocalSeg(i1, m1, neighbor, level, level2);

  double t = (TimeD(TM_PROCESS) - ta) / anz;
  Printf("Im Bild %d x %d dauert LocalSeg %8.2f Sekunden\n",
         XS, YS, t);
  Printf("Das sind %6.2f us pro Pixel\n", t * 1e6 / XS / YS);

  if (colorout)
    {
      int i, j;
      int rv[] = {127, 0, 255, 255, 0, 127};
      int gv[] = {127, 255, 0, 255, 0, 0};
      int bv[] = {127, 255, 255, 0, 255, 127};

      Image rot, gruen, blau;
      rot = NewImg(i1->xsize, i1->ysize, 255);
      gruen = NewImg(i1->xsize, i1->ysize, 255);
      blau = NewImg(i1->xsize, i1->ysize, 255);

      for (int y = 0; y < rot.ysize; y++)
        for (int x = 0; x < rot.xsize; x++)
          {
            int m;
            m = GetVal(m1, i, j) % 6;
            PutVal(rot, i, j, rv[m]);
            PutVal(gruen, i, j, gv[m]);
            PutVal(blau, i, j, bv[m]);
          }

      WriteImg(rot, gruen, blau, outfilename);
    }

  if (grayout)
    {
      int i, j;
      int gv[] = {127, 0, 255, 192, 64};
      Image gray;
      gray = NewImg(i1->xsize, i1->ysize, 255);
      for (int y = 0; y < gray.ysize; y++)
        for (int x = 0; x < gray.xsize; x++)
          {
            int m = GetVal(m1, x, y) % 6;
            PutVal(gray, x, y, gv[m]);
          }

      WriteImg(gray, outfilename);
    }

  GetChar();
  return OK;
}

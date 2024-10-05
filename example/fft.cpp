#include <image.h>

// programm zur berechnung von fft und abgeleiteten Transformationen
// von gegebenen bildern

void usage(const string& pn)
{
  cout << "Aufruf: " << pn << " [<options>] [filename]" << endl;
  cout << "options:" << endl;
  cout << "-m     Art der Transformation" << endl;
  cout << "     1 Leistungsspektrum" << endl;
  cout << "     2 Amplitudenspektrum" << endl;
  cout << "     3 Logarithmiertes Leistungsspektrum" << endl;
  cout << "     4 Hartley-Transformation" << endl;
  cout << "     5 Cepstrum" << endl;
  cout << "-g     Grauwerte statt Farbe" << endl;
  cout << "-o fn  Ausgabe in Datei fn" << endl;
  cout << "-H f   Hochpass mit Grenzfrequenz f" << endl;
  cout << "-T f   Tiefpass mit Grenzfrequenz f" << endl;
  cout << "-h     Diese Hilfe ausgeben" << endl;
  exit(1);
}

void TP(ImageD& id, int tp)
{
  if (tp > 0)
    {
      int xm = id.xsize / 2;
      int ym = id.ysize / 2;

      for (int y = 0; y < id.ysize; y++)
        for (int x = 0; x < id.xsize; x++)
          {
            int dx = abs(x - xm);
            int dy = abs(y - ym);

            if (dx > tp || dy > tp)
              PutValD(id, x, y, 0.0);
          }
    }
}

void HP(ImageD& id, int hp)
{
  if (hp > 0)
    {
      int xm = id.xsize / 2;
      int ym = id.ysize / 2;
      for (int y = 0; y < id.ysize; y++)
        for (int x = 0; x < id.xsize; x++)
          {
            int dx = abs(x - xm);
            int dy = abs(y - ym);

            if (dx < hp && dy < hp)
              PutValD(id, x, y, 0.0);
          }
    }
}

int hp = 0;
int tp = 0;
bool inverse = false;
bool inter = true;

void Transform(const Image& i, int kind)
{
//  cout << kind << endl;
  ImageD is = NewImgD(i->xsize, i->ysize);
  ImageD id = NewImgD(i->xsize, i->ysize);
  ConvImgImgD(i, is);

  switch (kind)
    {
    case 1:
      PowerSpectrumImgD(is, id, MD_POWER);
      break;
    case 2:
      PowerSpectrumImgD(is, id, MD_MAGNITUDE);
      break;
    case 3:
      PowerSpectrumImgD(is, id, MD_LOG);
      break;
    case 4:
      HartleyImgD(is, id);
      break;
    case 5:
      CepstrumImgD(is, id);
      break;
    }

  HP(id, hp);

  TP(id, tp);

  if (inverse)
    {
      HartleyImgD(id, id);
    }

  ConvImgDImg(id, i);
}

int main(int argc, char** argv)
{
  int rc;
  int mode = 1;
  bool gray = false;
  string fn;
  string ofn = "";

  while ((rc = getopt(argc, argv, "m:gh:o:H:T:ib")) >= 0)
    {
      switch (rc)
        {
        case 'b':
          inter = false;
          break;
        case 'm':
          mode = atol(optarg);
          break;
        case 'g':
          gray = true;
          break;
        case 'h':
          hp = atol(optarg);
          break;
        case 'o':
          ofn = optarg;
          break;
        case 'H':
          hp = atol(optarg);
          break;
        case 'T':
          tp = atol(optarg);
          break;
        case 'i':
          inverse = true;
          break;
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind > 0) // ein verbleibender Parameter = Dateiname;
    {
      fn = argv[optind];
    }
  else
    usage(argv[0]);

  ColorImage orig;
  orig.read(fn);
  Show(ON, orig, fn);

  Transform(orig.redImage(), mode);
  Transform(orig.greenImage(), mode);
  Transform(orig.blueImage(), mode);

  if (ofn != "")
    {
      orig.write(ofn);
    }

  if (inter) GetChar();

  return 0;
}

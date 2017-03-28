#include <image.h>
#include <iomanip>

// programm zur Filterung im Frequenzbereich

class CImage
{
public:
  ImageD real;
  ImageD imag;
  int xSize()
  {
    return real->xsize;
  }
  int ySize()
  {
    return real->ysize;
  }
  CImage(int x, int y)
  {
    real = NewImgD(x, y);
    imag = NewImgD(x, y);
  }
};

class CFImage
{
public:
  int xs, ys;
  CImage Red;
  CImage Green;
  CImage Blue;
  int xSize()
  {
    return Red.xSize();
  }
  int ySize()
  {
    return Red.ySize();
  }
  CFImage(int x, int y): xs(x), ys(y), Red(x, y), Green(x, y), Blue(x, y) {}
  CFImage(ColorImage& s): xs(s.xSize()), ys(s.ySize()), Red(xs, ys), Green(xs, ys), Blue(xs, ys) {}
};

void usage(const string& pn)
{
  cout << "Aufruf: " << pn << " [<options>] [filename]" << endl;
  cout << "options:" << endl;
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
      int xm = id->xsize / 2;
      int ym = id->ysize / 2;
      int x, y;
      wloop(id, x, y)
      {
        int dx = abs(x - xm);
        int dy = abs(y - ym);

        if (dx > tp || dy > tp)
          PutValD(id, x, y, 0.0);
      }
    }
}

void TP(CImage& ci, int tp)
{
  TP(ci.real, tp);
  TP(ci.imag, tp);
}

void TP(CFImage& cf, int tp)
{
  TP(cf.Red, tp);
  TP(cf.Green, tp);
  TP(cf.Blue, tp);
}

void HP(ImageD& id, int hp)
{
  if (hp > 0)
    {
      int xm = id->xsize / 2;
      int ym = id->ysize / 2;
      int x, y;
      wloop(id, x, y)
      {
        int dx = abs(x - xm);
        int dy = abs(y - ym);

        if (dx < hp && dy < hp)
          PutValD(id, x, y, 0.0);
      }
    }
}

void HP(CImage& ci, int hp)
{
  HP(ci.real, hp);
  HP(ci.imag, hp);
}

void HP(CFImage& cf, int hp)
{
  HP(cf.Red, hp);
  HP(cf.Green, hp);
  HP(cf.Blue, hp);
}

void Fourier(ColorImage& src, CFImage& dest)
{
  ImageD rs = NewImgD(src.xSize(), src.ySize());
  ImageD is = NewImgD(src.xSize(), src.ySize());
  ClearImgD(is);

  ConvImgImgD(src.R(), is, NORMALIZED, SIGNED);
  FourierImgD(rs, is, NORMAL, dest.Red.real, dest.Red.imag);

  ConvImgImgD(src.G(), is, NORMALIZED, SIGNED);
  FourierImgD(rs, is, NORMAL, dest.Green.real, dest.Green.imag);

  ConvImgImgD(src.B(), is, NORMALIZED, SIGNED);
  FourierImgD(rs, is, NORMAL, dest.Blue.real, dest.Blue.imag);
  FreeImgD(is);
}

void IFourier(CFImage& src, ColorImage& dest)
{
  ImageD rs = NewImgD(src.xSize(), src.ySize());
  ImageD is = NewImgD(src.xSize(), src.ySize());

  FourierImgD(src.Red.real, src.Red.imag, INVERS, rs, is);
  ConvImgDImg(is, dest.R(), NORMALIZED, SIGNED);

  FourierImgD(src.Green.real, src.Green.imag, INVERS, rs, is);
  ConvImgDImg(is, dest.G(), NORMALIZED, SIGNED);

  FourierImgD(src.Blue.real, src.Blue.imag, INVERS, rs, is);
  ConvImgDImg(is, dest.B(), NORMALIZED, SIGNED);

  FreeImgD(is);
}

int hp = 0;
int tp = 0;
int n = 1;
bool inverse = false;
bool inter = true;

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
          n = atol(optarg);
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
  orig.Read(fn);

  CFImage origf(orig);

  Fourier(orig, origf);

  TP(origf, tp);
  HP(origf, hp);

  IFourier(origf, orig);

  Show(ON, orig);

  if (ofn != "")
    {
      orig.Write(ofn);
    }

  if (inter) GetChar();

  return 0;
}

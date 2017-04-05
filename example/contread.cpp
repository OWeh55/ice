#include <image.h>

#define XSIZE 752
#define YSIZE 480
#define MAXVALUE (256*256-1)

void usage(const string& prog)
{
  cout << prog << ":" << endl;
  cout << "Aufruf: " << prog << " [options] <filename>" << endl;
  cout << "options:" << endl;
  cout << "-c    Farbbild erzeugen" << endl;
  cout << "-C <datei> Farbbild-Bild entbayern UND speichern" << endl;
  cout << "-b    Grauwert-Bild entbayern" << endl;
  cout << "-B <datei> Grauwert-Bild entbayern UND speichern" << endl;
  cout << "-q    Keine Visualisierung/GetChar()" << endl;
  exit(2);
}

int VD(const Image& img, int x, int y)
{
  // Interpolation eines Wertes "diagonal"
  // * . *
  // . + .
  // * . *
  int val = 0, ct = 0;

  if ((x > 0) && (y > 0))
    {
      val += GetVal(img, x - 1, y - 1);
      ct++;
    }

  if ((x > 0) && (y < YSIZE - 1))
    {
      val += GetVal(img, x - 1, y + 1);
      ct++;
    }

  if ((x < XSIZE - 1) && (y > 0))
    {
      val += GetVal(img, x + 1, y - 1);
      ct++;
    }

  if ((x < XSIZE - 1) && (y < YSIZE - 1))
    {
      val += GetVal(img, x + 1, y + 1);
      ct++;
    }

  return val / ct;
}

int VK(const Image& img, int x, int y)
{
  int val = 0, ct = 0;

  // Interpolation eines Wertes "kreuzweise"
  // . * .
  // * + *
  // . * .
  if (x > 0)
    {
      val += GetVal(img, x - 1, y);
      ct++;
    }

  if (x < XSIZE - 1)
    {
      val += GetVal(img, x + 1, y);
      ct++;
    }

  if (y > 0)
    {
      val += GetVal(img, x, y - 1);
      ct++;
    }

  if (y < YSIZE - 1)
    {
      val += GetVal(img, x, y + 1);
      ct++;
    }

  return val / ct;
}

int VH(const Image& img, int x, int y)
{
  // Interpolation eines Wertes "horizontal"
  // * + *
  int val = 0, ct = 0;

  if (x > 0)
    {
      val += GetVal(img, x - 1, y);
      ct++;
    }

  if (x < XSIZE - 1)
    {
      val += GetVal(img, x + 1, y);
      ct++;
    }

  return val / ct;
}

int VV(const Image& img, int x, int y)
{
  // Interpolation eines Wertes "vertikal"
  //  *
  //  +
  //  *
  int val = 0, ct = 0;

  if (y > 0)
    {
      val += GetVal(img, x, y - 1);
      ct++;
    }

  if (y < YSIZE - 1)
    {
      val += GetVal(img, x, y + 1);
      ct++;
    }

  return val / ct;
}

int main(int argc, char** argv)
{
  string fn;
  bool debayer = false;
  bool color = false;
  string cfilename = "";
  string bfilename = "";
  int rc;
  bool quiet = false;

  while ((rc = getopt(argc, argv, "bhcB:C:q")) >= 0)
    {
      switch (rc)
        {
        case 'b':
          debayer = true;
          color = true;
          break;
        case 'B':
          debayer = true;
          color = true;
          bfilename = optarg;
          break;
        case 'c':
          color = true;
          break;
        case 'C':
          color = true;
          cfilename = optarg;
          break;
        case 'q':
          quiet = true;
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind > 0) // ein verbleibende Parameter = Dateinamen;
    {
      fn = argv[optind]; // zunaechst nur einen auswerten
    }
  else
    usage(argv[0]);

  if (!quiet)
    {
      OpenAlpha("ReadCont");
      Alpha(ON);
    }

//  cout << "-->" << fn << "<---" << endl;
  Image img;
  img.create(XSIZE, YSIZE, MAXVALUE);
  clearImg(img);

  if (!quiet)
    Show(ON, img);

  ReadImg("PGM:|convert " + fn + " PGM:-", img);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        PutVal(img, x, y, (GetVal(img, x, y) << 6) & 0xffff);
      }

  ColorImage farbe(img, img, img);
  farbe.clear();

  if (color)
    {
      if (!quiet)
        Show(ON, farbe, "Farbbild");

      ColorValue rgb;
      rgb.red = rgb.green = rgb.blue = MAXVALUE;

      for (unsigned int y = 0; y < YSIZE; y += 2)
        {
          // gerade Zeilen
          for (unsigned int x = 0; x < XSIZE; x += 2)
            {
              //gerade Spalten
              rgb.red = VD(img, x, y);
              rgb.green = VK(img, x, y);
              rgb.blue = GetVal(img, x, y);
              farbe.setPixel(x, y, rgb);

              //ungerade Spalten
              rgb.red = VV(img, x + 1, y);
              rgb.green = GetVal(img, x + 1, y);
              rgb.blue = VH(img, x + 1, y);
              farbe.setPixel(x + 1, y, rgb);
            }

          // ungerade Zeilen
          for (unsigned int x = 0; x < XSIZE; x += 2)
            {
              //gerade Spalten
              rgb.red = VH(img, x, y + 1);
              rgb.green = GetVal(img, x, y + 1);
              rgb.blue = VV(img, x, y + 1);
              farbe.setPixel(x, y + 1, rgb);

              //ungerade Spalten
              rgb.red = GetVal(img, x + 1, y + 1);
              rgb.green = VK(img, x + 1, y + 1);
              rgb.blue = VD(img, x + 1, y + 1);
              farbe.setPixel(x + 1, y + 1, rgb);
            }
        }

      if (!cfilename.empty())
        farbe.write(cfilename);
    }

  if (debayer)
    {
      for (int y = 0; y < img.ysize; y++)
        for (int x = 0; x < img.xsize; x++)
          {
            ColorValue cv = farbe.getPixel(x, y);
            PutVal(img, x, y, (cv.red + cv.green + cv.blue) / 3);
          }

      if (!bfilename.empty()) WriteImg(img, bfilename);
    }

  if (!quiet)
    GetChar();

  return 0;
}

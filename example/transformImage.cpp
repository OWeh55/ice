#include <image.h>

void usage(const string& prog)
{
  cout << "Aufruf: " << endl;
  cout << "    " << prog << " [options] " << endl << endl;
  cout << "Optionen" << endl;
  cout << "-d       Keine Darstellung" << endl;
  cout << "-i file  Eingabe aus Bilddatei" << endl;
  cout << "-o file  Ausgabe in Datei" << endl;
  cout << "-w       Warten" << endl;
  cout << "-h       Hilfestellung" << endl;
  exit(5);
}

int main(int argc, char* argv[])
{
  bool display = true;
  bool wait = false;
  bool color = false;
  string ofn = "";
  string ifn = "";
  int mode = DEFAULT;
  int rc;

  while ((rc = getopt(argc, argv, "dhwIi:o:")) >= 0)
    {
      switch (rc)
        {
        case 'd':
          display = false;
          break;
        case 'w':
          wait = true;
          break;
        case 'I':
          mode = INTERPOL;
          break;
        case 'i':
          ifn = optarg;
          break;
        case 'o':
          ofn = optarg;
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  // Originalbild
  Image img;
  Image img2;
  ColorImage cimg;
  ColorImage cimg2;
  if (ifn.empty())
    {
      img.create(512, 512, 255);
      for (int y = 0; y < img.ysize; ++y)
        for (int x = 0; x < img.xsize; ++x)
          {
            if (((x / 8) & 1) ^ ((y / 8) & 1))
              img.setPixel(x, y, 0);
            else
              img.setPixel(x, y, img.maxval);
          }
    }
  else
    {
      int xs, ys, mv, ch;
      InfImgFile(ifn, xs, ys, mv, ch);
      if (ch == 1)
        {
          img.read(ifn);
          img2.create(img);
          img2.set(0);

          if (display)
            {
              Show(GRAY, img, "Original");
              Show(GRAY, img2, "Transformiert");
            }

        }
      else
        {
          color = true;
          cimg.read(ifn);
          cimg2.create(cimg);
          cimg2.set(ColorValue(0));
          if (display)
            {
              Show(ON, cimg, "Original");
              Show(ON, cimg2, "Transformiert");
            }
        }
    }

  // eine transformation konstruieren

  matrix<double> m(3, 3);
  m[0][0] = 1;
  m[0][1] = 0.01;
  m[0][2] = 10;
  m[1][0] = -0.01;
  m[1][1] = 1;
  m[1][2] = 20;
  m[2][0] = 1e-6;
  m[2][1] = 2e-6;
  m[2][2] = 1;

  Trafo tr(m);

  if (color)
    Transform(tr, cimg, cimg2, mode);
  else
    Transform(tr, img, img2, mode);

  if (!ofn.empty())
    {
      if (color)
        cimg2.write(ofn);
      else
        img2.write(ofn);
    }

  if (display)
    GetChar();

  return OK;
}


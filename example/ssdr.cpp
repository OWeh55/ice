// tests zu subpixel-SDR
#include <image.h>

int usage(const string& pn)
{
  cout << pn << endl;
  cout << "Usage: " << pn << " <options> filename1 filename2" << endl;
  cout << "options:" << endl;
  cout << "-h      diese Hilfe" << endl;
  cout << "-f fak  Auflösungsfaktor" << endl;
  cout << "-b beta Regularisierungswert" << endl;
  cout << "-t fg   Grenzfrequenz Tiefpass" << endl;
  cout << "-w xlo,ylo,width,height" << endl;
  cout << "        Fenster der Originalbilder" << endl;
  cout << "-d dis  Anzeige-Schalter (ODER-verknüpfte Werte, default=1)" << endl;
  cout << "         dis=1  Peakbild total" << endl;
  cout << "         dis=2  Originalbilder" << endl;
  cout << "         dis=4  Spektren" << endl;
  cout << "         dis=8  SDR Spektrum" << endl;
  cout << "         dis=16 Peakbild Ausschnitt" << endl;
  exit(1);
}

int error(const string& pn, const string& msg)
{
  cout << pn << ": " << msg << endl;
  exit(2);
}

int main(int argc, char** argv)
{
  int subPixelFactor = 1;
  int fg = 0; // Grenzfrequenz TP, 0 = kein Tiefpass
  int display = 0;
  string fn1, fn2;
  double beta = 0.01;
  int x1 = -1, y1, x2 = 0, y2 = 0, dx, dy;

  int rc;

  while ((rc = getopt(argc, argv, "hf:d:b:t:w:")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          subPixelFactor = atol(optarg);
          break;
        case 't':
          fg = atol(optarg);
          break;
        case 'd':
          display = atol(optarg);
          break;
        case 'b':
          beta = atof(optarg);
          break;
        case 'w':

          if (sscanf(optarg, "%d,%d,%d,%d", &x1, &y1, &dx, &dy) != 4)
            usage(argv[0]);

//  cout << x1 << "," << y1 << "  " << dx <<"*" << dy << endl;
          x2 = x1 + dx - 1;
          y2 = y1 + dy - 1;
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind > 1) // verbleibende Parameter = Dateinamen;
    {
      fn1 = argv[optind];
      fn2 = argv[optind + 1];
    }
  else
    usage(argv[0]);

  if (display > 0)
    Printf("SSDR %s %s\n", fn1.c_str(), fn2.c_str());

  Image img1 = ReadImg(fn1);

  if (!IsImg(img1))
    error(argv[0], "Kann " + fn1 + " nicht lesen");

  Image img2 = ReadImg(fn2);

  if (!IsImg(img2))
    error(argv[0], "Kann " + fn2 + " nicht lesen");

  if ((img1->xsize != img2->xsize) || (img1->xsize != img2->xsize))
    error(argv[0], "Bilder sind ungleich groß");

  Image imgw1 = img1;
  Image imgw2 = img2;

  Window w(x1, y1, x2, y2);

  if (x1 >= 0) // Fenster angegeben?
    {
      if (!Inside(img1, w))
        error(argv[0], "Fenster nicht im Bild");

      imgw1 = NewImg(img1, w);
      imgw2 = NewImg(img2, w);

    }

  if (display & 2)
    {
      Show(ON, imgw1, "orig 1");
      Show(ON, imgw2, "orig 2");
    }

  ImageD img1h = NewImgD(imgw1);
  ImageD img2h = NewImgD(imgw2);

  HartleyImg(imgw1, img1h);
  HartleyImg(imgw2, img2h);

  Image img1hv = NewImg(imgw1);
  Image img2hv = NewImg(imgw2);

  if (display & 4)
    {
      ConvImgDImg(img1h, img1hv);
      Show(ON, img1hv, "Hartley 1");
      ConvImgDImg(img2h, img2hv);
      Show(ON, img2hv, "Hartley 2");
    }

  ImageD imghp = NewImgD(imgw1);
  InvConvolutionHImgD(img1h, img2h, imghp, beta);

  ImageD imgxhp = imghp;

  if (subPixelFactor != 1)
    {
      imgxhp = NewImgD(imghp.xsize * subPixelFactor, imghp.ysize * subPixelFactor);
      imgxhp.set(0.0);

      for (int y = 0; y < imghp.ysize; y++)
        {
          int yo = y + (imgxhp.ysize - imghp.ysize) / 2;

          for (int x = 0; x < imghp.xsize; x++)
            {
              int xo = x + (imgxhp.xsize - imghp.xsize) / 2;
//  double val=GetValD(imghp,x,y);
//  if (val!=0) cout << x << " " << y << " " << GetValD(imghp,x,y) << endl;
              PutValD(imgxhp, xo, yo, GetValD(imghp, x, y));
            }
        }
    }

  if (fg != 0)
    {
      int fg2 = fg * fg;

      for (int y = 0; y < imgxhp.ysize; y++)
        {
          int fy = y - imgxhp.ysize / 2;
          int fy2 = fy * fy;

          for (int x = 0; x < imgxhp.xsize; x++)
            {
              int fx = x - imgxhp.xsize / 2;

              if (fx * fx + fy2 > fg2)
                PutValD(imgxhp, x, y, 0);
            }
        }
    }

  Image imghpv = NewImg(imgxhp.xsize, imgxhp.ysize, 255);

  if (display & 8)
    {
      ConvImgDImg(imgxhp, imghpv);
      Show(ON, imghpv, "sdr - Hartley");
    }

  ImageD imgp = NewImgD(imgxhp);
  HartleyImgD(imgxhp, imgp);

  double maxv = GetValD(imgp, 0, 0);
  int xm = 0, ym = 0;

  for (int y = 0; y < imgp.ysize; y++)
    for (int x = 0; x < imgp.xsize; x++)
      {
        double v = GetValD(imgp, x, y);

        if (v > maxv)
          {
            maxv = v;
            xm = x;
            ym = y;
          }
      }

  double xShift = ((double)xm - imgp.xsize / 2) / subPixelFactor;
  double yShift = ((double)ym - imgp.ysize / 2) / subPixelFactor;

  if (display > 0)
    Printf("Maximum: %d %d\n", xm, ym);

  Image imgpv = NewImg(imgxhp.xsize, imgxhp.ysize, 255);

  if (display & 1)
    {
      ConvImgDImg(imgp, imgpv);
      Show(ON, imgpv, "sdr");
    }

  Image imgpvp = NewImg(256, 256, 255);

  if (display & 16)
    {
      ImageD himg = NewImgD(256, 256);

      for (int y = ym - 128; y < ym + 128; y++)
        for (int x = xm - 128; x < xm + 128; x++)
          {
            PutValD(himg, x - xm + 128, y - ym + 128, GetValD(imgp, x, y));
          }

      ConvImgDImg(himg, imgpvp);
      Show(ON, imgpvp, "sdr peak");
    }
  cout << xShift << ", " << yShift << endl;
  if (display > 0)
    GetChar();

  return 0;
}

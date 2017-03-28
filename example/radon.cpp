// test Radon-Transformation
#include <iostream>
#include <string>
#include <image.h>
#include <omp.h>

using namespace std;

// Image debug;

int ImgFromRadon0(Image& radonimg, Image& resimg)
{
  // trivialste "Invertierung" Radon durch Rückprojektion
  RadonCTrafo tr(resimg->xsize, resimg->ysize,
                 radonimg->xsize, radonimg->ysize);

  double vfac = (double)resimg->maxval / (radonimg->maxval * radonimg -> ysize);

  for (int y = 0; y < resimg->ysize; y++)
    for (int x = 0; x < resimg->xsize; x++)
      {
        int sum = 0;

        for (int yr = 0; yr < radonimg->ysize; yr++)
          {
            tr.setYR(yr);
            int xr = tr.getXR(x, y);

            if (Inside(radonimg, xr, yr))
              {
                sum += GetVal(radonimg, xr, yr);
              }
          }

        PutVal(resimg, x, y, RoundInt(sum * vfac));
      }

  return 0;
}

void Filter(Image& resimg, int fmax)
{
  // nachträgliche Filterung
  ImageD hartley = NewImgD(resimg->xsize, resimg->ysize);
  ImageD result = NewImgD(resimg->xsize, resimg->ysize);
  HartleyImg(resimg, hartley);
  double x0 = hartley->xsize / 2;
  double y0 = hartley->ysize / 2;

  for (int y = 0; y < hartley->ysize; y++)
    {
      double fy = y - y0;

      for (int x = 0; x < hartley->xsize; x++)
        {
          double fx = x - x0;
          double f = sqrt(fx * fx + fy * fy);

          if (f <= fmax)
            PutValD(hartley, x, y, GetValD(hartley, x, y)*f);
          else
            PutValD(hartley, x, y, 0);
        }
    }

  HartleyImgD(hartley, result);
  ConvImgDImg(result, resimg);
  FreeImgD(hartley);
  FreeImgD(result);
}

void usage(const string& pn)
{
  cout << pn << " - Radontrafo ... " << endl;
  cout << "Aufruf:" << endl;
  cout << pn << " [<optionen>] " << endl;
  cout << "Optionen:" << endl;
  cout << " -h       Help" << endl;
  cout << " -x <nr>  Radonbildgröße X" << endl;
  cout << " -y <nr>  Radonbildgröße Y" << endl;
  cout << " -i <fn>  Nutze Bildfile fn" << endl;
  cout << " -o <fn>  Ausgabe in Bildfile fn" << endl;
  cout << " -f <f>   maximale Frequenz f bei Filterung (default: unendlich)" << endl;
  //  cout << " -s <n>   Schrittweite n (default: 0.1)"<< endl;
  cout << " -r <n>   Rekonstruktionsmethode" << endl;
  cout << "          n = 0  Keine Rekonstruktion, Radonbild wird mit -o gespeichert" << endl;
  cout << "          n = 1  Rückprojektion" << endl;
  cout << "          n = 2  Rückprojektion mit nachfolgender Filterung" << endl;
  cout << "          n = 3  \"gefilterte\" Rückprojektion" << endl;
  exit(2);
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  Image srcimg;
  Image radonimg;
  Image resimg;

  int rxs = -1;
  int rys = -1;

  int rmethode = 3;

  double fmax = 1000000;
  bool nowait = false;
  string outfilename = "";

  int rc;

  while ((rc = getopt(argc, argv, "hx:y:i:f:r:wo:")) >= 0)
    {
      switch (rc)
        {
        case 'h':
          usage(argv[0]);
          break;
        case 'x':
          rxs = atol(optarg);
          break;
        case 'y':
          rys = atol(optarg);
          break;
        case 'i':
          fn = optarg;
          break;
        case 'f':
          fmax = atof(optarg);
          break;
        case 'r':
          rmethode = atol(optarg);
          break;
        case 'w':
          nowait = true;
          break;
        case 'o':
          outfilename = optarg;
          break;
        }
    }

  srcimg = ReadImg(fn);

  int diag = sqrt(srcimg->xsize * srcimg->xsize + srcimg->ysize * srcimg->ysize);

  if (rxs < 0)
    {
      rxs = diag;
    }

  if (rys < 0)
    {
      rys = rxs;
    }

  radonimg = NewImg(rxs, rys, diag * srcimg->maxval);
  ClearImg(radonimg);

  //  debug=NewImg(srcimg);
  //  ClearImg(debug);
  //  Show(ON,debug,"DEBUG");

  resimg = NewImg(srcimg->xsize, srcimg->ysize, 16000);
  ClearImg(resimg);

  Show(ON, srcimg, "Source");
  Show(ON, radonimg, "Radon");
  Show(ON, resimg, "Rekonstruiert");

  double ta = TimeD();

  RadonImg(srcimg, radonimg);
  cout << "Radon:" << TimeD() - ta << endl;

  //  SmearImg(radonimg,radonimg,7,7);
  //  GetChar();

  if (rmethode == 1 || rmethode == 2)
    {
      ta = TimeD();
      ImgFromRadon0(radonimg, resimg);
      cout << "Inv1:" << TimeD() - ta << endl;

      if (rmethode == 2)
        {
          Filter(resimg, fmax);
        }
      else
        GrayNormalize(resimg);
    }

  if (rmethode == 3)
    {
      ta = TimeD();
      InvRadonImg(radonimg, resimg, fmax);
      cout << "Invers:" << TimeD() - ta << endl;
    }

  if (!outfilename.empty())
    {
      if (rmethode != 0)
        {
          Image outimg = NewImg(resimg->xsize, resimg->ysize, 255);
          RenormImg(resimg, outimg);
          WriteImg(outimg, outfilename);
        }
      else
        {
          Image outimg = NewImg(radonimg->xsize, radonimg->ysize, 255);
          RenormImg(radonimg, outimg);
          WriteImg(outimg, outfilename);
        }
    }

  if (!nowait)
    while (GetChar() != 13);

  return 0;
}

#include <getopt.h>
#include <image.h>

void usage(const string& prog)
{
  cout << prog << ":" << endl;
  cout << "Aufruf: " << prog << " [options] <filename>" << endl;
  cout << "options:" << endl;
  cout << "-d    dcraw zur Konvertierung benutzen" << endl;
  cout << "-c    convert zur Konvertierung benutzen" << endl;
  cout << "-h    Histogramm-Ausgleich anwenden" << endl;
  cout << "-m n  Medianfilter in der n*n-Umgebung anwenden (n ungerade!)" << endl;
  exit(2);
}

int main(int argc, char* argv[])
{
  string fn = "test_rgb.jpg";
  int mx, my, mv;
  int oret;

  bool convert = false;
  bool dcraw = false;
  bool histequal = false;
  int medianSize = 0;
  bool blackwhite = false;
  int zoom = 1;

  while ((oret = getopt(argc, argv, "bdchm:z:")) >= 0)
    {
      switch (oret)
        {
        case 'b':
          blackwhite = true;
          break;
        case 'c':
          convert = true;
          break;
        case 'd':
          dcraw = true;
          break;

        case 'h':
          histequal = true;
          break;
        case 'm':
          medianSize = atol(optarg);
          break;

        case 'z':
          zoom = atol(optarg);
          break;
        default:
          usage(argv[0]);
          break;
        }
    }

  if (argc - optind > 0) // verbleibender Parameter => Dateiname
    {
      fn = argv[optind];
    }
  else
    usage(argv[0]);

  if (medianSize > 0)
    {
      if ((medianSize & 1) == 0)
        usage(argv[0]);
    }

  if (convert)
    {
      fn = "pbm:|convert " + fn + " PNM:-";
    }
  else
    {
      if (dcraw)
        {
          fn = "pbm:|dcraw -4 -c " + fn;
        }
    }

  OpenAlpha("Read image");
  Alpha(ON);
  Print("Reading " + fn + "\n");
  InfImgFile(fn, mx, my, mv);

  Image imgr = NewImg(mx, my, mv);
  Image imgg = NewImg(mx, my, mv);
  Image imgb = NewImg(mx, my, mv);

  if (blackwhite)
    {
      ReadImg(fn, imgr);
      CopyImg(imgr, imgg);
      CopyImg(imgr, imgb);
    }
  else
    ReadImg(fn, imgr, imgg, imgb);

  Print("Displaying\n");
  Display(ON);
  Show(_RGB, imgr, imgg, imgb);
//  Zoom(imgr,zoom);

  if (histequal)
    {
      Print("Histogramm-Ausgleich\n");
      HistogramEqualization(imgr, imgr);
      HistogramEqualization(imgg, imgg);
      HistogramEqualization(imgb, imgb);
    }

  if (medianSize > 0)
    {
      Print("Medianfilter\n");
      int fsize = medianSize;
      int rank = fsize * fsize / 2;
      // cout << fsize << " " << rank << endl;
      RankImg(imgb, fsize, rank, imgb);
      RankImg(imgr, fsize, rank, imgr);
      RankImg(imgg, fsize, rank, imgg);
    }

  Printf("Groesse: %dx%d\n", imgr->xsize, imgr->ysize);
  Printf("Werte: 0..%d\n", imgr->maxval);

  Enter("Weiter mit <Enter>");
  return OK;
}

#include <getopt.h>
#include <image.h>

#include <PbmReader.h>

void usage(const string& prog)
{
  cout << prog << ":" << endl;

  cout << "Aufruf: " << prog << " [options] <filename>" << endl;
  cout << "options:" << endl;
  cout << "-d    dcraw zur Konvertierung benutzen" << endl;
  cout << "-c    convert zur Konvertierung benutzen" << endl;
  cout << "-f    ffmpeg zur Filmkonvertierung benutzen" << endl;
  cout << "-m    Mehrere Bilder" << endl;
  cout << "-w    Nicht warten" << endl;
  exit(2);
}

int main(int argc, char* argv[])
{
  string fn = "test_rgb.jpg";
  int mx, my, mv;
  int oret;

  bool convert = false;
  bool dcraw = false;
  bool ffmpeg = false;

  bool blackwhite = false;
  bool multiple = false;
  bool noWait = false;

  int nr = 0;
  while ((oret = getopt(argc, argv, "dcfmw")) >= 0)
    {
      switch (oret)
        {
        case 'c':
          convert = true;
          break;
        case 'd':
          dcraw = true;
          break;
        case 'f':
          ffmpeg = true;
          break;
        case 'm':
          multiple = true;
          break;

        case 'w':
          noWait = true;
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

  if (convert)
    fn = "|convert " + fn + " PNM:-";
  else if (dcraw)
    fn = "|dcraw -4 -c " + fn;
  else if (ffmpeg)
    fn = "|ffmpeg -i " + fn + " -codec:v ppm -f rawvideo -sn -an -v error - ";

  OpenAlpha("Read image");
  Alpha(ON);
  Print("Reading " + fn + "\n");

  PbmReader pr(fn);

  int ch;
  pr.getInfo(mx, my, mv, ch);
  if (ch < 3)
    blackwhite = true;

  Image imgr = NewImg(mx, my, mv);
  Image imgg = NewImg(mx, my, mv);
  Image imgb = NewImg(mx, my, mv);
  Show(_RGB, imgr, imgg, imgb);

  try
    {
      while (nr < 1 || multiple)
        {
          if (blackwhite)
            {
              pr.getImage(imgr);
              CopyImg(imgr, imgg);
              CopyImg(imgr, imgb);
            }
          else
            {
              pr.getImage(imgr, imgg, imgb);
            }

          Print("Displaying\n");
          Display(ON);

          Printf("Groesse: %d x %d\n", imgr->xsize, imgr->ysize);
          Printf("Werte: 0..%d\n", imgr->maxval);


          if (multiple)
            {
              if (!noWait)
                Enter("Weiter mit <Enter>");
              pr.nextImage();
            }
          ++nr;
        }
    }

  catch (const exception& ex)
    {
      cout << "exception: " << ex.what() << endl;
    }

  catch (const char* msg)
    {
      cout << "Exception: " << msg << endl;
    }
  Enter("Weiter mit <Enter>");
  return OK;
}

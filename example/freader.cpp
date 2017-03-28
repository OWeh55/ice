#include <unistd.h>
#include <image.h>

void usage(const string& pn)
{
  cout << pn << " - simpler Video-Filereader und Konverter" << endl;
  cout << "eine eventuelle Tonspur wird ignoriert" << endl;
  cout << "Aufruf: " << pn << " [<optionen>] <infilename> [<outfilename>]" << endl;
  cout << "Optionen:" << endl;
  cout << " -t <format>  Ausgabeformat format" << endl;
  cout << " -f <rate>    Ausgabe-Framerate rate" << endl;
  cout << " -b <rate>    Ausgabe-Bitrate rate" << endl;
  exit(2);
}

int main(int argc, char* argv[])
{
  int xs, ys;
  int mv;
  int fps = 0;
  string filename;
  string outfile;
  string format;
  int brate = 0;
  int verbose = 0;

  int rc;

  while ((rc = getopt(argc, argv, "ht:f:b:lv")) >= 0)
    {
      switch (rc)
        {
        case 't':
          format = optarg;
          break;
        case 'f':
          fps = atol(optarg);
          break;
        case 'b':
          brate = atol(optarg);
          break;
        case 'v':
          verbose++;
          break;

        case 'h':
        default:
          usage(argv[0]);
        }
    }

  if (argc > optind)
    filename = argv[optind];
  else
    {
      usage(argv[0]);
    }

  if (argc > optind + 1)
    outfile = argv[optind + 1];

  if (verbose > 0)
    cout << "In: " << filename << endl;

  VideoFile vf(filename);

  int sfps;
  vf.getPara(xs, ys, mv, sfps);

  if (verbose > 0)
    cout << xs << "x" << ys << " " << mv << " " << sfps << endl;

  if (fps == 0)
    fps = sfps;

  VideoFile of;

  if (!outfile.empty())
    {
      if (verbose > 0)
        cout << "Out: " << outfile << endl;

      of.open(outfile, ios_base::out);

      if (!format.empty())
        of.setCPara(format);

      if (!outfile.empty())
        of.setPara(xs, ys, mv, fps, brate);
    }


  ColorImage rgb;
  rgb.create(xs, ys, mv);

  Show(ON, rgb);

  while (vf.read(rgb))
    {
      int c = vf.FrameNumber();

      if ((verbose > 1) && (c % 1000 == 0))
        cout << "Frame " << c << endl;

      if (!outfile.empty())
        of.write(rgb);
    }

  GetChar();
  return OK;
}


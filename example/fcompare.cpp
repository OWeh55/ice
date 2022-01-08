#include <image.h>
#include <readcache.h>
#include <VideoFile.h>

void usage(const string& pn)
{
  cout << pn << " <options> <filename> <filename>" << endl;
  cout << "options:" << endl;
  cout << "-h       diese Hilfe" << endl;
  cout << "-s<mc>   Darstellungs-Modus der beiden Bilder" << endl;
  cout << "         mc = d (difference) Differenz" << endl;
  cout << "         mc = r (red/cyan) Anaglyph(rot/cyan)" << endl;
  cout << "         mc = s (sidebyside) nebeneinander" << endl;
  cout << "         mc = i (interlaced) zeilenweise abwechselnd" << endl;
  cout << "-t <ofs> Startwert Zeitverschiebung (in Frames)" << endl;
  cout << "-S <ofs> Stereoshift" << endl;
  cout << "-o <fn>  Ausgabe in die Datei fn" << endl;
  cout << "-v       Anzeige der Frame-Nummern (verbose)" << endl;
  cout << "-z <n>   Zeitraffer - Ausgabe jedes n. Frames" << endl;
  cout << "-f <fac> Zoom mit Faktor fac" << endl;
  cout << "-b       Schwarz/Weiss-Modus" << endl;
  cout << "-d <ms>  Verzögerung zwischen Bildern in ms (interaktiver Modus)" << endl;
  cout << "-f <nr>  Erster Frame" << endl;
  cout << "-F <nr>  Faktor" << endl;
  exit(1);
}

string basename(const string& filename)
{
  string res = filename;
  int pos = res.length() - 1;

  while ((pos >= 0) && (res[pos] != '.')) pos--; // find first '.'

  res = res.substr(0, pos);
  return res;
}

void msleep(int ms)
{
  usleep(1000 * ms);
}

int main(int argc, char* argv[])
{
  string filename1;
  string filename2;
  string filenamed;

  int offset = 0;
  int stshiftmax = 0;
  int stshift = 0;
  int fac = 1;
  int first = 0;
  int last = -1;
  int cachesize = 200;
  int delay = 0;

  int zeitraffer = 1;

  bool pause = false;

  enum dmode_t { diff, rotcyan, sidebyside, interlaced};
  int dmodus = diff;

  bool bw = false;
  bool verbose = false;
  int rc;

  while ((rc = getopt(argc, argv, "ho:t:S:z:vbs:f:d:F:pl:c:")) >= 0)
    {
      switch (rc)
        {
        case 's':
        {
          char hmode = optarg[0];

          switch (hmode)
            {
            case 'd':
              dmodus = diff;
              break;
            case 'r':
              dmodus = rotcyan;
              break;
            case 's':
              dmodus = sidebyside;
              break;
            case 'i':
              dmodus = interlaced;
              break;
            }
        }

        case 't':
          offset = atol(optarg);
          break;
        case 'F':
          fac = atol(optarg);
          break;
        case 'f':
          first = atoi(optarg);
          break;
        case 'l':
          last = atoi(optarg);
          break;
        case 'S':
          stshiftmax = stshift = atol(optarg);
          break;
        case 'o':
          filenamed = optarg;
          break;
        case 'z':
          zeitraffer = atol(optarg);
          break;
        case 'v':
          verbose = true;
          break;
        case 'b':
          bw = true;
          break;
        case 'd':
          delay = atoi(optarg);
          break;
        case 'p':
          pause = true;
          break;
        case 'c':
          cachesize = atoi(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  if (argc > optind + 1)
    {
      filename1 = argv[optind];
      filename2 = argv[optind + 1];
    }
  else if (argc > optind)
    {
      filename1 = argv[optind];
      filename2 = argv[optind];
    }
  else
    {
      usage(argv[0]);
    }

  //  cout << filename1 << " " << filename2 << endl;

  int xs1, ys1;
  int xs2, ys2;
  int mv, fps;

  VideoFile vf1;
  VideoFile vf2;

  ReadColorImageCache<VideoFile>* vfc1;
  ReadColorImageCache<VideoFile>* vfc2;

  bool equalfiles = filename1 == filename2;

  if (equalfiles)
    {
      vf1.open(filename1);
      vf1.getPara(xs1, ys1, mv, fps);
      vfc1 = vfc2 = new ReadColorImageCache<VideoFile>(vf1, xs1, ys1, mv, cachesize);
      xs2 = xs1;
      ys2 = ys1;
    }
  else
    {
      vf1.open(filename1);
      vf1.getPara(xs1, ys1, mv, fps);
      vfc1 = new ReadColorImageCache<VideoFile>(vf1, xs1, ys1, mv, cachesize);
      vf2.open(filename2);
      vf2.getPara(xs2, ys2, mv, fps);
      vfc2 = new ReadColorImageCache<VideoFile>(vf2, xs2, ys2, mv, cachesize);
    }

  VideoFile vfd;

  if (!filenamed.empty())
    {
      vfd.open(filenamed, ios_base::out);
#if 0
      cout << vfd.getpara() << endl;
      vfd.setpara("-ovc lavc -lavcopts vcodec=mpeg4:vbitrate=2400");
#endif
    }

  int xs = min(xs1, xs2);
  int ys = min(ys1, ys2);

  Printf("%s - %d*%d\n", filename1.c_str(), xs1, ys1);

  if (!equalfiles)
    Printf("%s - %d*%d\n", filename2.c_str(), xs2, ys2);

  if (!filenamed.empty())
    Printf("--> %s - %d*%d\n", filenamed.c_str(), xs, ys);
  else
    Printf("--> %d*%d\n", xs, ys);

  ColorImage in1;
  in1.create(xs1, ys1, mv);
  Show(ON, in1, filename1 + " (1)");
  ColorImage in1s(in1, Window(0, 0, xs - 1, ys - 1));

  ColorImage in2;
  in2.create(xs2, ys2, mv);
  Show(ON, in2, filename2 + " (2)");
  ColorImage in2s(in2, Window(0, 0, xs - 1, ys - 1));

  int framenr1 = first;
  int framenr2 = first;

  if (offset > 0)
    framenr1 += offset;
  else
    framenr2 -= offset;

  int ff = 0;

  while ((ff < framenr1) && (ff < framenr2))
    {
      vfc1->read(in1, ff);
      vfc2->read(in2, ff);
      Printf("Skip Frames: %d \n", ff);
      ff += 100;
    }

  int xd = (xs + stshiftmax) * fac;
  int yd = (ys * fac);
  int xri = xd;
  int yri = yd;

  if (dmodus == sidebyside)
    xri *= 2;

  ColorImage dif;
  dif.create(xri, yri, mv);

  Show(ON, dif, "1 <-> 2");

  int abort = false;
  bool upd = true;
  bool eof = false;
  int lastframe1 = framenr1;
  int lastframe2 = framenr2;
  double time1 = TimeD();
  double dfps = 20.0;

  do
    {
      if (upd)
        {
          upd = false;

          if (vfc1->read(in1, framenr1) && vfc2->read(in2, framenr2))
            {
              // letztes erfolgreiches Lesen
              lastframe1 = framenr1;
              lastframe2 = framenr2;
            }
          else
            {
              // Vorzustand wieder herstellen
              framenr1 = lastframe1;
              framenr2 = lastframe2;
              // test auf EOF
              eof = vfc1->getError() == ReadColorImageCache<VideoFile>::past_eof;

              if (!eof)
                eof = vfc2->getError() == ReadColorImageCache<VideoFile>::past_eof;
            }

          Printf("%d %d (%d)\n", framenr1, framenr2, stshift);
          double time2 = TimeD();
          dfps = dfps * 0.99 + 0.01 / (time2 - time1);
          Print(NumberString(dfps) + " fps\n");
          time1 = time2;

          switch (dmodus)
            {
            case diff:

              for (int y = 0; y < yd; y++)
                {
                  int yr = y / fac;

                  for (int x = 0; x < xd; x++)
                    {
                      int xr = x / fac;

                      ColorValue v1;

                      if (xr < xs1)
                        v1 = in1.getPixelUnchecked(xr, yr);
                      else
                        v1 = ColorValue(255, 255, 255);

                      if (bw)
                        v1 = ColorValue(v1.getGray());

                      ColorValue v2;
                      int xs = xr - stshift;

                      if ((xs >= 0) && (xs < xs2))
                        v2 = in2.getPixelUnchecked(xs, yr);
                      else
                        v2 = ColorValue(255, 255, 255);

                      if (bw)
                        v2 = ColorValue(v2.getGray());

                      ColorValue v = v1 - v2 + ColorValue(mv / 2, mv / 2, mv / 2);
                      dif.setPixel(x, y, v.Limited(dif.maxval));
                    }
                }

              break;
            case rotcyan:

              for (int y = 0; y < yd; y++)
                {
                  int yr = y / fac;

                  for (int x = 0; x < xd; x++)
                    {
                      int xr = x / fac;

                      ColorValue v1;

                      if (xr < xs1)
                        v1 = in1.getPixelUnchecked(xr, yr);
                      else
                        v1 = ColorValue(255, 255, 255);

                      if (bw)
                        v1 = ColorValue(v1.getGray());

                      ColorValue v2;
                      int xs = xr - stshift;

                      if ((xs >= 0) && (xs < xs2))
                        v2 = in2.getPixelUnchecked(xs, yr);
                      else
                        v2 = ColorValue(255, 255, 255);

                      if (bw)
                        v2 = ColorValue(v2.getGray());

                      ColorValue vc(v1.red, v2.green, v2.blue);
                      dif.setPixel(x, y, vc.Limited(dif.maxval));

                    }
                }

              break;
            case interlaced:

              for (int y = 0; y < yd; y++)
                {
                  int yr = y / fac;

                  for (int x = 0; x < xd; x++)
                    {
                      int xr = x / fac;

                      ColorValue v1;

                      if (xr < xs1)
                        v1 = in1.getPixelUnchecked(xr, yr);
                      else
                        v1 = ColorValue(255, 255, 255);

                      if (bw)
                        v1 = ColorValue(v1.getGray());

                      ColorValue v2;
                      int xs = xr - stshift;

                      if ((xs >= 0) && (xs < xs2))
                        v2 = in2.getPixelUnchecked(xs, yr);
                      else
                        v2 = ColorValue(255, 255, 255);

                      if (bw)
                        v2 = ColorValue(v2.getGray());

                      if (y % 2 == 1)
                        dif.setPixel(x, y, v1.Limited(dif.maxval));
                      else
                        dif.setPixel(x, y, v2.Limited(dif.maxval));
                    }
                }

              break;
            case sidebyside:

              for (int y = 0; y < yd; y++)
                {
                  int yr = y / fac;

                  for (int x = 0; x < xd; x++)
                    {
                      int xr = x / fac;

                      ColorValue v1;

                      if (xr < xs1)
                        v1 = in1.getPixelUnchecked(xr, yr);
                      else
                        v1 = ColorValue(255, 255, 255);

                      if (bw)
                        v1 = ColorValue(v1.getGray());

                      ColorValue v2;
                      int xs = xr - stshift;

                      if ((xs >= 0) && (xs < xs2))
                        v2 = in2.getPixelUnchecked(xs, yr);
                      else
                        v2 = ColorValue(255, 255, 255);

                      if (bw)
                        v2 = ColorValue(v2.getGray());

                      dif.setPixel(x, y, v1);
                      dif.setPixel(x + dif.xsize / 2, y, v2);

                    }
                }

              break;
            } // switch(dmodus)
        }

      if (!filenamed.empty())
        {
          // batchmode (non-interactive)

          if (eof)
            {
              abort = true;
            }
          else
            {
              vfd.write(dif);
              framenr1 += zeitraffer;
              framenr2 += zeitraffer;
              upd = true;
            }

        }
      else
        {
          if (!pause)
            {
              framenr1 += zeitraffer;
              framenr2 += zeitraffer;
              upd = true;
              /*
                  if (delay>0)
                msleep(delay);
              */
            }

          int c = GetKey();

          if (c > 0)
            {
              switch (c)
                {
                case ' ':
                  pause = !pause;
                  break;
                case 's':
                case 'S': // step
                case '8':
                  pause = true;
                  framenr1++;
                  framenr2++;
                  upd = true;
                  break;
                case '2':
                  pause = true;
                  framenr1--;
                  framenr2--;
                  upd = true;
                  break;
                case '7':
                  pause = true;
                  framenr1++;
                  upd = true;
                  break;
                case '9':
                  pause = true;
                  framenr2++;
                  upd = true;
                  break;
                case '1':
                  pause = true;
                  framenr1--;
                  upd = true;
                  break;
                case '3':
                  pause = true;
                  framenr2--;
                  upd = true;
                  break;
                case '=':
                case '5':

                  if (framenr1 > framenr2)
                    framenr1 = framenr2;
                  else
                    framenr2 = framenr1;

                  upd = true;
                  break;
                case 'w':
                {
                  string path;
                  string name;
                  string ext;
                  SplitFilename(filename1, path, name, ext);
                  string imagefilename = path + "/" + name + "_" + NumberString(framenr1) + "a.jpg";
                  Print("Schreibe " + imagefilename + "\n");
                  in1s.write(imagefilename);
                  SplitFilename(filename2, path, name, ext);
                  imagefilename = path + "/" + name + "_" + NumberString(framenr1) + "b.jpg";
                  Print("Schreibe " + imagefilename + "\n");
                  in2s.write(imagefilename);
                }
                break;
                case 'W':
                {
                  string path1;
                  string name1;
                  string ext;
                  SplitFilename(filename1, path1, name1, ext);
                  string path2;
                  string name2;
                  SplitFilename(filename2, path2, name2, ext);

                  string filename = path1 + "/" + name1 + "_" + name2 + "_" +
                                    NumberString(framenr1) + "_" + NumberString(framenr2) + ".jpg";
                  Print("Schreibe " + filename + "\n");
                  dif.write(filename);
                }
                break;
                case 'b':
                  bw = !bw;
                  upd = true;
                  break;
                case '-':

                  if (stshift > 0) stshift--;

                  upd = true;
                  break;
                case '+':

                  if (stshift < stshiftmax) stshift++;

                  upd = true;
                  break;
                case 'q':
                case 'Q':
                case 'X':
                case 'x':
                  abort = Ask("Abbrechen");
                  break;
                }
            }
        }

      if (last > 0 && (framenr1 > last || framenr2 > last))
        abort = true;
    }
  while (!abort);

  delete vfc1;

  if (!equalfiles)
    delete vfc2;

  return OK;
}

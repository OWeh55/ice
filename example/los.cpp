#include <getopt.h>
#include <image.h>

int main(int argc, char** argv)
{
  int oret;
  string ifn = "test_gray.jpg";

  int nn = 15;
  int nd = 30;

  while ((oret = getopt(argc, argv, "i:n:d:")) >= 0)
    {
      switch (oret)
        {
        case 'i':
          ifn = string(optarg);
          break;
        case 'n':
          nn = atol(optarg);
          break;
        case 'd':
          nd = atol(optarg);
          break;
        }
    }

  OpenAlpha("Terminal");
  Alpha(ON);
  ClearAlpha();
  Print("using file : " + ifn + "\n");

  int xs, ys, mv;
  InfImgFile(ifn, xs, ys, mv);
  Printf("x:%d y:%d\n\n", xs, ys);

  Printf("Parameter: \n  Nachbarschaft %d\n", nn);
  Printf("  Minimaler Unterschied: %d\n", nd);

  Image grw;
  grw.create(xs, ys, 255);
  Image mrk;
  mrk.create(xs, ys, 7);
  mrk.set(0);

  Show(OVERLAY, grw, mrk);
  Zoom(grw);

  grw.read(ifn);

  LocalSeg(grw, mrk, nn, nd);
  Printf("LocalSeg(grw, mrk, %d, %d)\n", nn, nd);
  GetChar();
  return 0;
}

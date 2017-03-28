#include <getopt.h>
#include <image.h>

int main(int argc, char** argv)
{
  int oret;
  string ifn = "test_gray.jpg";
  int xs, ys, mv;
  Image grw, mrk;

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
  Print("File : " + ifn + "\n");
  InfImgFile(ifn, xs, ys, mv);
  Printf("x:%d y:%d\n\n", xs, ys);

  Printf("Parameter: \n  Nachbarschaft %d\n", nn);
  Printf("  Minimaler Unterschied: %d\n", nd);

  grw = NewImg(xs, ys, 255);
  mrk = NewImg(xs, ys, 7);
  ClearImg(mrk);
  Display(ON);
  Show(OVERLAY, grw, mrk);
  Zoom(grw);
  ReadImg(ifn, grw);
  LocalSeg(grw, mrk, nn, 15);
  GetChar();

  bool ready = false;

  while (!ready)
    {
      ready = true;
      int x, y;
      wloop(mrk, x, y)
      {
        if (GetVal(mrk, x, y) == 2)
          {
            int no = 0;
            int nu = 0;

            for (int dir = 0; dir < 8; dir += 2)
              {
                int xt, yt;
                Freeman(dir).move(x, y, xt, yt);

                if ((xt >= 0) && (xt < xs) && (yt >= 0) && (yt < ys))
                  {
                    int v = GetVal(mrk, xt, yt);

                    if ((v == 0) || (v == 4)) no++;

                    if ((v == 1) || (v == 5)) nu++;
                  }
              }

            if ((no > 0) && (nu == 0)) PutVal(mrk, x, y, 4);

            if ((no == 0) && (nu > 0)) PutVal(mrk, x, y, 5);

            if ((no > 0) && (nu > 0)) PutVal(mrk, x, y, 6);

            if ((no == 0) && (nu == 0)) ready = false;
          }
      }
    }

  GetChar();
  ready = false;

  while (!ready)
    {
      ready = true;
      int x, y;
      wloop(mrk, x, y)
      {
        if (GetVal(mrk, x, y) == 6)
          {
            for (int dir = 0; dir < 8; dir += 2)
              {
                int xt, yt;
                Freeman(dir).move(x, y, xt, yt);

                if ((xt >= 0) && (xt < xs) && (yt >= 0) && (yt < ys))
                  {
                    int v = GetVal(mrk, xt, yt);

                    if ((v == 4) || (v == 5))
                      {
                        PutVal(mrk, xt, yt, 6);
                        ready = false;
                      }
                  }
              }

            PutVal(mrk, x, y, 7);
          }
      }
    }

  GetChar();
  int x, y;
  wloop(mrk, x, y)
  {
    PutVal(mrk, x, y, GetVal(mrk, x, y) & 3);
  }
  GetChar();

  return 0;
}

#include <getopt.h>
#include <image.h>

class ehist
{
protected:
  Window w;
  unsigned int bilder;
  Image img[3];
  Histogram h;
public:
  ehist(Image i1, Image i2, Image i3): w(0, 0, 0, 0), bilder(3), h(i1->maxval + 1)
  {
    img[0] = i1;
    img[1] = i2;
    img[2] = i3;
  }

  ehist(Image i1): w(0, 0, 0, 0), bilder(1), h(i1->maxval + 1)
  {
    img[0] = i1;
  }

  ehist(): w(0, 0, 0, 0), bilder(0) {};

  void CalcHist(Window pw)
  {
    w = pw;
    int x, y;
    unsigned int i;

    for (i = 0; i < bilder; i++)
      for (y = w.p1.y; y <= w.p2.y; y++)
        for (x = w.p1.x; x <= w.p2.x; x++)
          h.addValueUnchecked(GetValUnchecked(img[i], x, y));
  }

  void MoveR()
  {
    int y;
    unsigned int i;

    for (i = 0; i < bilder; i++)
      {
        for (y = w.p1.y; y <= w.p2.y; y++)
          h.addValue(GetVal(img[i], w.p1.x, y), -1);

        for (y = w.p1.y; y <= w.p2.y; y++)
          h.addValue(GetVal(img[i], w.p2.x + 1, y), 1);
      }

    w.p1.x++;
    w.p2.x++;
  }

  void MoveL()
  {
    int y;
    unsigned int i;

    for (i = 0; i < bilder; i++)
      {
        for (y = w.p1.y; y <= w.p2.y; y++)
          h.addValue(GetVal(img[i], w.p2.x, y), -1);

        for (y = w.p1.y; y <= w.p2.y; y++)
          h.addValue(GetVal(img[i], w.p1.x - 1, y), 1);
      }

    w.p1.x--;
    w.p2.x--;
  }

  void MoveD()
  {
    int x;
    unsigned int i;

    for (i = 0; i < bilder; i++)
      {
        for (x = w.p1.x; x <= w.p2.x; x++)
          h.addValue(GetVal(img[i], x, w.p1.y), -1);

        for (x = w.p1.x; x <= w.p2.x; x++)
          h.addValue(GetVal(img[i], x, w.p2.y + 1), 1);
      }

    w.p1.y++;
    w.p2.y++;
  }

  void Statistic(double& xm, double& xs)
  {
    int s;
    h.Statistic(s, xm, xs);
  }

} ;

int main(int argc, char** argv)
{
  int oret;
  unsigned int bilder;
  unsigned int i;
  string ifn = "test_gray.jpg";
  int xs, ys, mv;
  Image grw[3], grwn[3];
  Image std, mean;
  int nr;

  int nn = 15;
  double nd = 30;
  double k = 0.9;

  while ((oret = getopt(argc, argv, "i:n:d:k:")) >= 0)
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
          nd = atof(optarg);
          break;
        case 'k':
          k = atof(optarg);
          break;
        }
    }

  OpenAlpha("Terminal");
  Alpha(ON);
  ClearAlpha();
  Print("File : " + ifn + "\n");
  InfImgFile(ifn, xs, ys, mv, nr);
  Printf("x:%d y:%d\n\n", xs, ys);
  double mv2 = mv / 2;

  Printf("Parameter: \nNachbarschaft %d\n", nn);
  Printf("Parameter für Standard-Abweichung: %f\n", nd);

  Display(ON);

  ehist h;

  if (nr != 3)
    {
      grw[0] = NewImg(xs, ys, mv, "Originalbild");
      grwn[0] = NewImg(xs, ys, mv, "Normalisiertes Bild");

      Show(ON, grw[0]);
      Show(ON, grwn[0]);

      ReadImg(ifn, grw[0]);
      bilder = 1;
      h = ehist(grw[0]);
    }
  else
    {
      for (i = 0; i < 3; i++)
        {
          grw[i] = NewImg(xs, ys, mv);
          grwn[i] = NewImg(xs, ys, mv);
        }

      Show(_RGB, grw[0], grw[1], grw[2], "Original");
      Show(_RGB, grwn[0], grwn[1], grwn[2], "Normalisiert");

      ReadImg(ifn, grw[0], grw[1], grw[2]);
      ReadImg(ifn, grwn[0], grwn[1], grwn[2]);

      bilder = 3;
      h = ehist(grw[0], grw[1], grw[2]);
    }

  std = NewImg(xs, ys, mv);
  Show(ON, std, "Standardabweichung");
  mean = NewImg(xs, ys, mv);
  Show(ON, mean, "Mittelwert");

  int x, y;

  Print("!");


  h.CalcHist(Window(0, 0, 2 * nn, 2 * nn));
  int lx = xs - nn - 1;
  int ly = ys - nn - 1;
  double mval, sval;
  y = nn;
  x = nn;

  while (y <= ly)
    {
      while (x <= lx)
        {
          h.Statistic(mval, sval);
          PutVal(std, x, y, limited(RoundInt(sval), 0, 255));
          PutVal(mean, x, y, limited(RoundInt(mval), 0, 255));

          if (x < lx)
            h.MoveR();

          x++;
//      cout << x << endl;
        }

      x--;
      y++;

      if (y <= ly)
        {
          h.MoveD();

          while (x >= nn)
            {
              h.Statistic(mval, sval);
              PutVal(std, x, y, limited(RoundInt(sval), 0, 255));
              PutVal(mean, x, y, limited(RoundInt(mval), 0, 255));

              if (x > nn) h.MoveL();

              x--;
            }

          x++;

          if (y < ly) h.MoveD();

          y++;
        }
    }

  for (i = 0; i < bilder; i++)
    {
      Print("+");
      for (int y = 0; y < grw[i].ysize; y++)
        for (int x = 0; x < grw[i].xsize; x++)
          {
            int x1 = x - nn;
            int y1 = y - nn;
            int x2 = x + nn;
            int y2 = y + nn;

            if ((x1 >= 0) && (y1 >= 0) && (x2 <= xs) && (y2 < ys))
              {
                double xm = GetVal(mean, x, y);
                double xs = GetVal(std, x, y);
                double fac = 100 / (xs + nd);
                double relg = GetVal(grw[i], x, y) - xm;

                PutVal(grwn[i], x, y, limited(RoundInt(relg * fac + k * (xm - mv2) + mv2), 0, 255));
              }
            else
              PutVal(grwn[i], x, y, GetVal(grw[i], x, y));
          }
    }

  Print("\nFertig!\n");

  GetChar();

  return 0;
}

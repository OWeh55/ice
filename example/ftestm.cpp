#include <image.h>

int main(int argc, char* argv[])
{
  Image img;

  OpenAlpha("FitLine-Test");
  SetAttribute(C_BLACK, C_CYAN, 0, 0);
  ClearAlpha();
  Display(ON);

  img = NewImg(1000, 800, 8);
  Show(OVERLAY, img);

  bool c = true;

  while (c)
    {
      ClearImg(img);
      Printf("Bitte Punkte eingeben, Abbruch mit rechter Maustaste.\n");

      vector<Point> pl;

      int rc;

      do
        {
          IPoint p = SelPoint(DEFAULT, img, rc);

          if (!(rc < 0))
            {
              pl.push_back(p);
              Marker(1, p, 4, 15, img);
            }
        }
      while (!(rc < 0));

      ClearImg(img);

      for (unsigned int i = 0; i < pl.size(); i++)
        Marker(1, pl[i], 4, 15, img);

      //     FitLine(pl,lp,0);
      //     HesseLine(lp[0],lp[1],3,img);

      //     FitLine(pl,lp,1);
      //     HesseLine(lp[0],lp[1],4,img);

      //     FitLine(pl,lp,2);
      //     HesseLine(lp[0],lp[1],5,img);

      LineSeg ls;

      for (int i = 0; i <= 3; i++)
        {
          ls = FitLine(pl, i);
          draw(ls, img, 2 + i);
        }

//    Printf("Lineseg: %g %g    %g %g",ls.X1(),ls.Y1(),ls.X2(),ls.Y2());
      c = Ask("Noch einmal", "jn");
    }

  return 1;
}

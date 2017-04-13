#include <image.h>

int main(int argc, char* argv[])
{
  Image img;

  OpenAlpha("FitCircle-Test");
  SetAttribute(C_BLACK, C_CYAN, 0, 0);
  ClearAlpha();
  Display(ON);

  img = NewImg(800, 800, 8);
  Show(OVERLAY, img);

  bool c = true;

  while (c)
    {
      clearImg(img);
      Printf("Bitte Punkte eingeben, Abbruch mit rechter Maustaste.\n");

      Matrix pl(0, 2);

      int rc;

      do
        {
          IPoint p = SelPoint(DEFAULT, img, rc);

          if (!(rc < 0))
            {
              pl.append(Vector(p.X(), p.Y()));
              Marker(1, p, 4, 15, img);
            }
        }
      while (!(rc < 0));

      clearImg(img);

      for (int i = 0; i < pl.rows(); i++)
        Marker(1, RoundInt(pl[i][0]), RoundInt(pl[i][1]), 4, 15, img);

      double par[5];

      if (FitEllipse(pl, par, 0, 2) == OK)
        drawEllipse(par, 2, 3, NOFILL, img);
      else
        Print("Keine Ellipse\n");

      Ellipse el = FitEllipse(pl, 0, 2);
      draw(el, img, 3, 1);

      c = Ask("Noch einmal", "jn");
    }

  return 1;
}

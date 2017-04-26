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

      vector<Point> pl;

      for (int i = 0; i < 60; i++)
        {
          double phi = i / 10.0;
          Point p(300 + 200 * sin(phi), 400 + 200 * cos(phi));
          Point noise(Random(20) - 10, Random(20) - 10);
          pl.push_back(p + noise);
        }

      for (unsigned int i = 0; i < pl.size(); i++)
        Marker(1, Point(pl[i]), 4, 15, img);

      int rc;

      do
        {
          IPoint p = SelPoint(DEFAULT, img, rc);

          if (!(rc < 0))
            {
              pl.push_back(Point(p));
              Marker(1, p, 4, 15, img);
            }
        }
      while (!(rc < 0));

      clearImg(img);

      for (unsigned int i = 0; i < pl.size(); i++)
        Marker(1, Point(pl[i]), 4, 15, img);

      for (int step = 0; step < 20; step++)
        {
          Circle cc = FitCircle(pl, step);
          draw(cc, img, step + 2);
          cout << "Circle: " << cc.getR() << " " << cc.getPos().x << " " << cc.getPos().y << endl;
        }

      c = Ask("Noch einmal", "jn");
    }

  return 1;
}

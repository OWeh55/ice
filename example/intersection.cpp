#include <image.h>

LineSeg InputLine(const Image& img, const string& p)
{
  LineSeg res;
  Print("Eingabe " + p + "\n");
  IPoint p1 = SelPoint(img);
  Marker(1, p1, 3, 5, img);
  IPoint p2 = SelPoint(img);
  Marker(1, p2, 3, 5, img);
  Print("G - Gerade\n");
  Print("A - Strahl\n");
  Print("I - Inverser Strahl\n");
  Print("S - Strecke\n");
  char c = GetChar();

  switch (c)
    {
    case 'G':
    case 'g':
      res = LineSeg(p1, p2, LineSeg::line);
      break;
    case 'A':
    case 'a':
      res = LineSeg(p1, p2, LineSeg::ray);
      break;
    case 'I':
    case 'i':
      res = LineSeg(p1, p2, LineSeg::inv_ray);
      break;
    case 'S':
    case 's':
    default:
      res = LineSeg(p1, p2, LineSeg::segment);
      break;
    }

  Marker(1, p1, 0, 5, img);
  Marker(1, p2, 0, 5, img);
  return res;
}

int main(int argc, char** argv)
{
  Image img;
  img.create(1024, 1024, 255);
  Image mrk;
  mrk.create(1024, 1024, 8);
  clearImg(img);
  clearImg(mrk);
  Show(OVERLAY, img, mrk);

  while (true)
    {
      LineSeg l1 = InputLine(mrk, "Erste Linie");
      draw(l1, mrk, 1);
      LineSeg l2 = InputLine(mrk, "Zweite Linie");
      draw(l2, mrk, 2);
      Point p;
      bool inter = l1.Intersection(l2, p);
      //      cout << p.x << "," << p.y << endl;

      int color = 3;

      if (!inter)
        color = 1;

      if (Inside(mrk, IPoint(p)))
        Marker(2, p, color, 7, mrk);

      WindowWalker w(img);
      for (w.init(); ! w.ready(); w.next())
        {
          double d1 = l1.Distance(Point(w));
          double d2 = l2.Distance(Point(w));

          if (d1 < d2)
            img.setPixel(w, limited(d1, img));
          else
            img.setPixel(w, limited(d2, img));
        }

      Print("Weiter... <ENTER>\n");

      while (GetChar() != 13) /* wait*/;

      clearImg(mrk);
      for (w.init(); !w.ready(); w.next())
        {
          if (l1.RightOf(w))
            mrk.setPixel(w, 1);
        }

    }

  return OK;
}

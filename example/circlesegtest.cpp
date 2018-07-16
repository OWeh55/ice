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

Circle InputCircle(const Image& img, const string& p)
{
  Print("Eingabe " + p + "\n");
  Print("Eingabe Mittelpunkt \n");
  IPoint p1 = SelPoint(img);
  Marker(1, p1, 3, 5, img);
  Print("Eingabe Randpunkt \n");
  IPoint p2 = SelPoint(img);

  Marker(1, p1, 0, 5, img);
  return Circle(p1, (p2 - p1).r());
}

CircleSeg InputCircleSeg(const Image& img, const string& p)
{
  Print("Eingabe " + p + "\n");
  Print("Eingabe Mittelpunkt \n");
  IPoint pc = SelPoint(img);
  Marker(1, pc, 3, 5, img);
  Print("Eingabe 1. Randpunkt \n");
  IPoint p1 = SelPoint(img);
  Marker(1, p1, 3, 5, img);
  Print("Eingabe 2. Randpunkt \n");
  IPoint p2 = SelPoint(img);
  Marker(1, p2, 3, 5, img);
  // marker wieder löschen
  Marker(1, p2, 0, 5, img);
  Marker(1, p1, 0, 5, img);
  Marker(1, pc, 0, 5, img);
  Point d1 = Point(p1 - pc);
  Point d2 = Point(p2 - pc);
  // cout << pc.x << "," << pc.y << ","  << d1.r() << ","  << d1.phi() << ","  << d2.phi() << endl;
  return CircleSeg(pc, d1.r(), d1.phi(), d2.phi());
}

int main(int argc, char** argv)
{
  Image img;
  img.create(1024, 1024, 255);
  clearImg(img);

  Image mrk;
  mrk.create(1024, 1024, 8);
  clearImg(mrk);
  Show(OVERLAY, img, mrk);

  while (true)
    {
      CircleSeg c1 = InputCircleSeg(mrk, "erstes Kreissegment");
      //      c1.draw(mrk, 1);
      draw(c1, mrk, 1);
      CircleSeg c2 = InputCircleSeg(mrk, "zweites Kreissegment");
      draw(c2, mrk, 2);
#if 0
      {
        Point p;
        bool inter = c1.Intersection(c2, p);
        //      cout << p.x << "," << p.y << endl;

        int color = 3;

        if (!inter)
          color = 1;

        if (Inside(mrk, IPoint(p)))
          Marker(2, p, color, 7, mrk);
      }
#endif

      WindowWalker p(img);
      for (p.init(); !p.ready(); p.next())
        //      for (IPoint p = img.begin(); p != img.end(); img.next(p))
        {
          //    double d1=c1.Distance(Point(p));
          double d1 = c1.Distance(p);
          double d2 = c2.Distance(p);

          if (d1 < d2)
            img.setPixel(p, limited(d1, img));
          else
            img.setPixel(p, limited(d2, img));
        }

      for (int i = 0; i < 100000; i++)
        {
          IPoint p;
          p.x = Random(1023);
          p.y = Random(1023);
          int color = 1;

          if (c1.Inside(p)) color++;

          if (c2.Inside(p)) color += 2;

          mrk.setPixel(p, color);
        }

      Print("Weiter... <ENTER>\n");

      while (GetChar() != 13) /* wait*/;

      clearImg(mrk);

    }

  return OK;
}

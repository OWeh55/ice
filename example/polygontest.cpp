// test von Polygon-Funktionen

#include <image.h>

Polygon MakePoly(int n = 4)
{
  // erzeugt zufälliges Polygon ohne Überschlagung
  Point p;
  Polygon m;

  do
    {
      p = Point(Random(400) + 50, Random(400) + 50);
      m.Reset(p);

      for (int i = 1; i < n; i++)
        {
          p = Point(Random(400) + 50, Random(400) + 50);
          m.Add(p);
        }
    }
  while (! m.isValid());

  return m;
}

PolygonalCurve MakePolygonalCurve(int n = 4)
{
  Point p;
  PolygonalCurve m;

  p = Point(Random(400) + 50, Random(400) + 50);
  m.Reset(p);

  for (int i = 1; i < n; i++)
    {
      p = Point(Random(400) + 50, Random(400) + 50);
      m.Add(p);
    }

  return m;
}

int main(int argc, char* argv[])
{
  Polygon m;
  Image img;
  img.create(512, 512, 255);
  Image mark;
  mark.create(512, 512, 255);
  Show(OVERLAY, img, mark);

  while (true)
    {
      m = MakePoly(13);
      // m.Draw(mark,3,1);
      // GetChar();

      Print("\n Test inside()");

      IPoint p;
      for (p.y = 0; p.y < img->ysize; p.y++)
        for (p.x = 0; p.x < img->xsize; p.x++)
          {
            PutVal(mark, p, m.Inside(p) ? 1 : 2);
          }

      GetChar();

      Print("\n Test distance()");

      clearImg(mark);
      Contur c = m.MakeContur();
      FillRegion(c, 3, mark);
      MarkContur(c, 1, mark);

      for (p.y = 0; p.y < img->ysize; p.y++)
        for (p.x = 0; p.x < img->xsize; p.x++)
          {
            double d = m.Distance(p);

            if (d > 255)d = 255;

            PutVal(img, p, d);
          }

      GetChar();

      Print("\n Test closestCorner()");
      clearImg(mark);
#if 0
      MarkContur(c, 2, mark);
      IPoint sel;
      int rc;
      sel = SelPoint(mark, rc);

      while (rc >= 0)
        {
          int cidx = m.getClosestCorner(sel);
          Marker(1, m[cidx], 1, 5, mark);
          sel = SelPoint(mark, rc);
        }

#endif

      for (p.y = 0; p.y < img->ysize; p.y++)
        for (p.x = 0; p.x < img->xsize; p.x++)
          {
            int i = m.getClosestCorner(p);
            PutVal(mark, p, i);
          }

      draw(m, mark, 1);
      GetChar();

      Print("\n Test closestEdge()");

      for (p.y = 0; p.y < img->ysize; p.y++)
        for (p.x = 0; p.x < img->xsize; p.x++)
          {
            int i = m.getClosestEdge(p);
            PutVal(mark, p, i);
          }

      draw(m, mark, 1);
      GetChar();

    }

  return OK;
}

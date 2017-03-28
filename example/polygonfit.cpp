// test von Polygon-Methoden zum Fitten

#include <image.h>

/*const*/ int panz = 15;
const int xsize = 1200;
const int ysize = 1024;

Polygon MakePoly(int n = 4)
{
  // erzeugt zufälliges Polygon ohne Überschlagung
  Point p;
  Polygon m;

  do
    {
      p = Point(Random(xsize - 100) + 50, Random(ysize - 100) + 50);
      m.Reset(p);

      for (int i = 1; i < n; i++)
        {
          p = Point(Random(xsize - 100) + 50, Random(ysize - 100) + 50);
          m.Add(p);
        }
    }
  while (! m.isValid());

  return m;
}

int main(int argc, char* argv[])
{
  Polygon m;
  Image img(xsize, ysize, 255);
  Image mark(xsize, ysize, 255);
  Show(OVERLAY, img, mark);

  while (true)
    {
      m = MakePoly(15);
      // m=SelPolygon(mark);
      panz = m.size();
      Draw(m, mark, 3);
      Printf("Original mit %d Eckpunkten\n", panz);
      Polygon ch;
      m.ConvexHull(ch);
      Draw(ch, img, 255, 127);
      Printf("Konvexe Hülle mit %d Eckpunkten\n", ch.size());

      GetChar();
      ClearImg(img);

      for (int i = panz - 1; i > 3; i--)
        {
          Polygon r = m.Reduced(i);
          Draw(r, img, 255, 127);
          Printf("%d Eckpunkte\n", r.size());
          GetChar();
          Draw(r, img, 50, 0);
        }

      ClearImg(mark);
      ClearImg(img);
    }

  return OK;
}

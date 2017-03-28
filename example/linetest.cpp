#include <image.h>
const int steps = 48;
const double rad1 = 299;
const double rad2 = 433;

int main(int argc, char** argv)
{
  Image g(1000, 1000, 255);
  Image m(1000, 1000, 255);
  Show(OVERLAY, g, m);

  Point pc(g->xsize / 2, g->ysize / 2), p1, p2;

  for (int gv = 0; gv < 255; gv++)
    {
      for (int k = 0; k < steps; k += 2)
        {
          double kfi = k * 6.283 / steps;
          p1.x = pc.x + rad1 * sin(kfi);
          p1.y = pc.y + rad1 * cos(kfi);

          for (int i = 1; i < steps; i += 2)
            {
              double fi = i * 6.283 / steps;
              p2.x = pc.x + rad2 * sin(fi);
              p2.y = pc.y + rad2 * cos(fi);
              Draw(LineSeg(p1, p2, LineSeg::segment), g, gv);
            }
        }
    }

#if 0

  for (int i = 0; i < 1500; i++)
    {
      p1.x = Random(1000);
      p1.y = Random(1000);
      p2.x = Random(1000);
      p2.y = Random(1000);
      //      LineSeg(p1,p2).Draw(m,3);
      //      Line(p1,p2,3,0,m);
      //      Line(p1,p2,3,m);
      //      Line(IPoint(p1),IPoint(p2),3,0,m);
      Line(IVector(p1.x, p1.y), IVector(p2.x, p2.y), 2, 0, m);
    }

  for (double fi = 0; fi < 180; fi += 20)
    for (double r = -50; r < 50; r += 10)
      {
        //      HesseLine(r,Arcus(fi),2,m);
        LineSeg l(r, Arcus(fi));
        l.Shift(500, 500);
        l.Draw(m, 1);
      }

#endif

  GetChar();
  return OK;
}

/*
 * test geometrischer Objekte
 */

#include <image.h>

int main(int args, char** argv)
{
  Circle c(255, 255, 100);
  CircleSeg cs(655, 155, 10, 0, 0.9 * M_PI);
  Ellipse e(555, 255, 100, 10, 45);
  EllipseSeg es(800, 500, 200, 100, 0.2 * M_PI, 0.3 * M_PI, 1.8 * M_PI);

  Point p1(13, 13), p2(222, 222);
  Vector p(299, 9, 29, 566);
  LineSeg l(p);
  Image g, m;
  g = NewImg(1000, 700, 255);
  g.set(255);
  m = NewImg(1000, 700, 255);
  clearImg(m);
  Alpha(ON);
  ClearAlpha();
  Display(ON);
  Show(OVERLAY, g, m);

  //  circle(55,55,0).Draw(m,1,1);
  //  GetChar();

  draw(LineSeg(p1, p2), m, 1);
  draw(LineSeg(111, 4, 444, 444), m, 2);
  draw(LineSeg(p), m, 3);

  //  line(Vector(5,55,444,555)).draw(m,3);

  LineSeg h;
  h = LineSeg(Vector(5, 55, 444, 555));
  draw(h, m, 4);

  //  cs.draw(m,4,2);

  for (int i = 0; i < 10; i++)
    {
      l.Shift(55, 5);
      draw(l, m, 5);
    }

  draw(c, m, 3, 1);

//  Image test=NewImg(m);
//  Show(ON,test);

  for (double phi = 0.0; phi < 2 * M_PI; phi += M_PI / 16)
    {
      e.setPhi(phi);
      e.Shift(-5, 5);
      draw(e, m, 1, 4);
//      int x,y;
//      wloop(test,x,y)
//  if (e.Inside(x,y)) PutVal(test,x,y,255);
    }

  for (double phi = 0.0; phi < 2 * M_PI; phi += M_PI / 8)
    {
      cs.setR(100 - phi * 10);
      cs.setPhi2(phi);
      draw(cs, m, 3, 2);
//      int x,y;
//      wloop(test,x,y)
//  if (cs.Inside(x,y)) PutVal(test,x,y,255);
    }

  for (double phi = 0.0; phi < 0.2 * M_PI; phi += M_PI / 80)
    {
      es.setR(es.getR() * 0.9);
      es.setR2(es.getR2() * 0.9);
      es.setPhi(phi);
      draw(es, m, 3, 4);
//      int x,y;
//      wloop(test,x,y)
//  if (es.Inside(x,y)) PutVal(test,x,y,255);
    }

  p1 = Point(100, 500);
  c = Circle(p1, 50);
  draw(c, m, 3, 2);

  GetChar();

  return 0;
}

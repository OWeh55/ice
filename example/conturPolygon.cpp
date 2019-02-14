#include <image.h>

int main(int ragc, char** argv)
{
  Contur c(7,7);
  // add Freeman codes
  c.add(0);
  c.add(0);
  c.add(1);
  c.add(1);
  c.add(2);
  c.add(2);
  c.add(4);
  c.add(4);
  c.add(5);
  c.add(5);
  c.add(6);
  c.add(6);

  Image g;
  g.create(20,20,255);
  g.set(0);
  Image m;
  m.create(20,20,16);
  m.set(0);
  Show(OVERLAY,g,m);
  Zoom(m,16);
  
  MarkContur(c,1,m);
  GetChar();
  
  FillRegion(c,2,m);
  GetChar();

  Polygon p;
  LimitingPolygon(c,p);
  draw(p,g,255,127);
  int nCorners=p.size();
  for (int i=0;i<nCorners;i++)
    cout << p[i] << endl;
  GetChar();
  
  return OK;
}

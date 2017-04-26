#include <image.h>

int main(int argc, char** argv)
{
  Image g1, m1;
  double circle[3] = {100, 50, 45};
  double circleM[3] = {50, 90, 40};
  double circleM1[3] = {150, 90, 40};
  int ps[2] = {1, 1};
  //  int x,y;
  Matrix p1, p2;
  Contur c2;

  g1 = NewImg(255, 255, 255);
  m1 = NewImg(255, 255, 255);

  Display(ON);
//  Show(OVERLAY,g1,m1);
  Show(OVERLAY, g1, g1, "Lokal segmentiertes Bild");
  Show(OVERLAY, g1, m1, "Markierungsbild mit Konturen");

  Alpha(ON);

  ClearAlpha();
  m1.set(0);
  g1.set(0);
//  Zoom(g1,0,0,0);
//  DrawCircle(circle,255,255,DEFAULT,g1);
//  DrawCircle(circleM,2,2,DEFAULT,m1);

//  GetChar();

  drawCircle(circle, 1, 1, DEFAULT, g1);
  drawCircle(circleM, 2, 2, DEFAULT, g1);
  drawCircle(circleM1, 2, 2, DEFAULT, g1);

//  GetChar();
//  Line(100,50,150,100,255,DEFAULT,g1);

  int co = 1;

  while (SearchStart(g1, m1, LocalSegObj, 33, 1, ps, HORZ) == OK)
    {

//    double atime=TimeD();
//    for (int i=1;i<100000;i++)
      c2 = CalcContur(g1, m1, LocalSegObj, 33, ps, 0, isunknown, isobject);

//    Printf("Zeit:%5.2f\n",TimeD()-atime);
      if (c2.isClosed())
        {
          if (c2.isHole()) Printf("Loch!\n");
          else Printf("Objekt\n");
        }
      else
        Printf("Nicht geschlossen\n");

      Marker(1, ps[0], ps[1], 1, 5, m1);
      MarkContur(c2, 2, m1);
      //  c2.GetPoint(0,x,y);
      //  PutVal(m1,x,y,1);
      co = (co % 4) + 1;
      GetChar();
    }

  Printf("OK");
  GetChar();
  return 0;
}

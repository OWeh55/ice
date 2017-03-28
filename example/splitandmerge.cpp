// test von Polygon-Methoden zum Fitten

#include <image.h>

int mode = 3;
/*const*/
int panz = 15;
int xsize = 1200;
int ysize = 1024;
const double lastfi = 314.159256 / 4;
const double stepfi = lastfi / 10000 * 3;

PolygonalCurve Spirale()
{
  PolygonalCurve sp(false);

  for (double fi = 0; fi < lastfi; fi += stepfi)
    {
      double r = fi * ysize / 2 / lastfi + 4 * sin(fi * 36);
      sp.Add(Point(xsize / 2 + r * cos(fi), ysize / 2 + r * sin(fi)));
    }

  return sp;
}

PolygonalCurve Viereck()
{
  Contur c(IPoint(30, 80));
  c.Add(IPoint(800, 10));
  c.Add(IPoint(700, 500));
  c.Add(IPoint(10, 500));
  return PolygonalCurve(c, false);
}

int main(int argc, char* argv[])
{
  int mv;
//  InfImgFile("20120912_113049.jpg",xsize,ysize,mv);
  Image img(xsize, ysize, 255);
  Image lsimg(xsize, ysize, 255);
  ClearImg(img);
  //  ReadImg("20120912_113049.jpg",img);
  Image mark(xsize, ysize, 255);
  ClearImg(mark);
  // LocalSeg(img,lsimg,15,75);
  Show(OVERLAY, img, mark);
  //  Show(OVERLAY, img, lsimg);

  while (true)
    {
      //  m=MakePoly(panz);
      Contur c = SelContur(mark);
      // IPoint s=SelPoint(img);
      // SearchStart(lsimg,mark,LocalSegObj,20,1,s);
      // Contur c=CalcContur(lsimg,mark,LocalSegObj,20,
      //                     s, 0, isunknown, isunknown);

      PolygonalCurve m(c, false);
      //      PolygonalCurve m=SelPolygon(mark);
      //      PolygonalCurve m=Spirale();
      //      PolygonalCurve m=Viereck();
      panz = m.size();
      Draw(m, mark, 3);
      Printf("Original mit %d Eckpunkten, Länge %4.2f\n", panz, m.Length());
      // cout << "Closed? " << m.isClosed() << endl;
      /*
      Polygon ch;
      m.ConvexHull(ch);
      Draw(ch,img,255,127);
      Printf("Konvexe Hülle mit %d Eckpunkten\n",ch.size());
      */
      GetChar();
      //      ClearImg(img);
      ClearImg(mark);
#if 0
      if (panz > 50)
        panz = 50;

      for (int i = panz - 1; i >= 3; i--)
        {
          //      Polygon r=m.ReducedPolygon(i);
          PolygonalCurve r;
          m.Reduced(i, r, mode);
          Draw(r, img, 255, 127);
          Printf("%d Eckpunkte, Länge %4.2f\n", i, r.Length());
          GetChar();
          Draw(r, img, 50, 0);
        }

#endif
#if 0

      for (int y = 0; y < img->ysize; y++)
        for (int x = 0; x < img->xsize; x++)
          {
            double d = m.Distance(Point(x, y)) * 3;

            if (d > 255) d = 255;

            PutVal(img, x, y, d);
          }

#else
      Draw(m, img, 255, 127);
#endif
      ClearImg(mark);

      for (double prec = 1.0; prec < 200; prec *= 1.414)
        {
          PolygonalCurve r(true);
          //Polygon r;
          m.ReducedToPrecision(prec, r, mode);
          //    cout << "Closed? " << r.isClosed() << endl;
          ClearImg(mark);
          Draw(r, mark, 3, -1);
          //          Point p;

          for (int i = 0; i < r.size(); i++)
            Marker(1, r[i], 3, 5, mark);

          Printf("%d Eckpunkte, Länge %4.2f, prec %4.2f\n", r.size(), r.Length(), prec);
#if 0
          vector<Point> fitted = FitPolygonPointlist(r.PointList(), m.PointList(), 3, 3);
          Polygon fpoly(fitted);
          Draw(fpoly, mark, 1, -1);
          for (int i = 0; i < r.size(); i++)
            Marker(1, fpoly[i], 1, 5, mark);
#endif
          GetChar();

#if 0
          r = m.Reduced(r.size(), mode);
          Draw(r, mark, 1, -1);

          for (int i = 0; i < r.size(); i++)
            Marker(1, r[i], 3, 5, mark);

          Printf("%d Eckpunkte, Länge %4.2f\n", r.size(), r.Length());
          GetChar();
#endif
        }

      ClearImg(mark);
      ClearImg(img);
    }

  return OK;
}

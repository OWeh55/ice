#include <image.h>

int main(int argc, char* argv[])
{
  string fn = "test_gray.jpg";

  if (argc > 1)
    fn = argv[1];

  // i1=ReadImg("nummern.jpg");
  // i1=ReadImg("zeichen_05_06.tif");

  Image i1 = ReadImg(fn);

  Image i2 = NewImg(i1->xsize, i1->ysize, 15);
  clearImg(i2);
  Display(ON);
  Show(OVERLAY, i1, i2);
  int co = 1;

  while (true)
    {
      IPoint p = SelPoint(i1);
      // RegionGrow(p.x, p.y, i1, i2, co, 100000);
      // Region r = RegionGrowGrw(p.X(),p.Y(),i1,10.0,30000);

      //Region r = RegionGrow(p, i1, 100000, 0); // 0 - Minimumsuche
      //Region r = RegionGrow(p, i1, 100000, i1.maxval); // maxval=Maximumsuche
      Region r = RegionGrow(p, i1, 100000); // Ähnichkeit zu Keimpunkt
      r.draw(i2, co);

#if 0
      int x1, y1, x2, y2;
      r.getRect(x1, y1, x2, y2);
      cout << i1->xsize << "*" << i1->ysize << endl;
      cout << x1 << "," << y1 << " " << x2 << "," << y2 << endl;
#endif

      co++;

      if (co > 15) co = 1;

      // GetChar();
    }

  GetChar();
  return 0;
}

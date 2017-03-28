// tests zur Akkumulation in ice
#include <image.h>
#include <iostream>
#include <iomanip>

using namespace ice;
using namespace std;

class neartest
{
  LineSeg line;
public:
  neartest(LineSeg l): line(l) {}

  bool operator()(const Point& p)
  {
    return line.Distance(p) < 3.0;
  }
};

int main(int argc, char** argv)
{
  string fn = "g3z.tif";

  if (argc > 1)
    fn = argv[1];

  Image img = ReadImg(fn);
  GradImg(img, 4, img);
  Image mrk = NewImg(img);
  Show(OVERLAY, img, mrk);
  ClearImg(mrk);

  //  DoBImg(img,img,3,5);

  vector<Point> pl;

  Print("Suche Punkte\n");
#if 1

  for (int y = 0; y < img->ysize; y++)
    for (int x = 0; x < img->xsize; x++)
      {
        int v = GetVal(img, x, y);

        if (v > 150)
          {
            pl.push_back(Point(x, y));
            PutVal(mrk, x, y, 3);
          }
      }

#else

  for (int y = 1; y < img->ysize - 1; y++)
    for (int x = 1; x < img->xsize - 1; x++)
      {
        int v = GetVal(img, x, y);

        if (
          v >= GetVal(img, x - 1, y) &&
          v >= GetVal(img, x + 1, y) &&
          v >= GetVal(img, x, y - 1) &&
          v >= GetVal(img, x, y + 1)
        )
          {
            pl.push_back(Point(x, y));
            PutVal(mrk, x, y, 3);
          }
      }

#endif

  Printf("%d Punkte gefunden\n", pl.size());
  Print("Suche Linien\n");

  vector<LineSeg> lines;

  while (pl.size() > 1000)
    //  for (int n=0;n<10;n++)
    {
      //     LineSeg line=DetectLine(pl,pl.size()*100);
      Printf("Punktliste mit %d Punkten\n", pl.size());
      vector<Point> pl2; // Hilfsliste

      LineSeg line = DetectLine(pl, pl.size() * 100);

      // Punkte in der nähe aufsammeln
      pl2.clear();

      for (int i = 0; i < pl.size(); i++)
        if (line.Distance(pl[i]) < 2.0)
          pl2.push_back(pl[i]);

      line = FitLine(pl2); // angepasste Linie

      //      Draw(line,mrk,1);
      lines.push_back(line);

      // Punkte in der Nähe der Linie aus Liste entfernen
      pl2.clear();

      for (int i = 0; i < pl.size(); i++)
        if (line.Distance(pl[i]) > 2.0)
          pl2.push_back(pl[i]);

      pl = pl2;

      Printf("Punktliste noch mit %d Punkten\n", pl.size());

      ClearImg(mrk);

      for (int i = 0; i < lines.size(); i++)
        Draw(lines[i], mrk, 1);

      for (int i = 0; i < pl.size(); i++)
        PutVal(mrk, IPoint(pl[i]), 3);

      GetChar();
    }

  Print("<ENTER>\n");
  GetChar();
  return 0;
}

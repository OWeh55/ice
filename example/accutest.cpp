// tests zur Akkumulation in ice
#include <image.h>
#include <iostream>
#include <iomanip>

using namespace ice;
using namespace std;

int main(int argc, char** argv)
{
  Image img;
  img.create(999, 999, 255);
  Image mrk;
  mrk.create(999, 999, 255);

  Show(OVERLAY, img, mrk);
  ClearImg(mrk);

  ClearImg(img);
  LineSeg aline(755, 55, 77, 777);
  draw(aline, img, 90);

  double pO, phiO;
  aline.CalcHesse(pO, phiO);
  cout << "given:   p: " << pO << " phi: " << phiO << endl;

  WindowWalker p(img);
  for (p.init(); ! p.ready(); p.next())
    img.setPixel(p, img.getPixel(p) + Random(100));

  vector<Point> pl;

  Print("Suche Punkte\n");

  for (p.init(); ! p.ready(); p.next())
    {
      int v = img.getPixel(p) - 6;

      if (v > 90)
        {
          pl.push_back(p);
          mrk.setPixel(p, 3);
        }
    }

  Printf("%d Punkte gefunden\n", pl.size());
  Print("Suche Linie\n");

  double ta = TimeD();
  LineSeg line = detectLine(pl, pl.size() * 100);

  double pF, phiF;
  line.CalcHesse(pF, phiF);
  cout << "found:    p: " << pF << " phi: " << phiF << endl;

  cout << TimeD() - ta << " s benötigt" << endl;

  Print("<ENTER>\n");
  GetChar();
  img.set(255);
  draw(line, mrk, 1);

  for (int i = 0; i < (int)pl.size(); i++)
    mrk.setPixel(IPoint(pl[i]), 2);

  Print("<ENTER>\n");
  GetChar();
  return 0;
}

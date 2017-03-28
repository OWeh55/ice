#include <image.h>
#include <region.h>

const int fak = 15;

using namespace ice;

int main(int argc, char* argv[])
{
  int ps[2];
  Image img = NewImg(100 * fak, 100 * fak, 255);
  Image mark = NewImg(img);

  ClearImg(img);
  ClearImg(mark);
  Show(OVERLAY, img, mark);
  ClearAlpha();
  Display(ON);

  Region s[3];

  Contur c;

  Circle k1(30 * fak, 30 * fak, 20 * fak);
  Circle k2(60 * fak, 30 * fak, 20 * fak);

  draw(k1, img, 255, 200);

  ps[0] = 0;
  ps[1] = 1;
  SearchStart(img, Image(), NULL, 5, 5, ps);
  s[0] = Region(CalcContur(img, Image(), NULL, 5, ps));

  Print("Region 1\n");
  GetChar();
  ClearImg(img);

  draw(k2, img, 255, 100);

  ps[0] = 0;
  ps[1] = 1;
  SearchStart(img, Image(), NULL, 5, 5, ps);
  s[1] = Region(CalcContur(img, Image(), NULL, 5, ps));
  Print("Region 2\n");
  GetChar();

  ClearImg(img);

  WindowWalker ww(img);
  for (ww.init(); !ww.ready(); ww.next())
    {
      if (((ww.x ^ ww.y) & 1) == 0)
        {
          s[2].add(ww);
          img.setPixel(ww, 255);
        }
    }

  Print("Punkte der Region 3 (als Grauwerte)\n");
  GetChar();

  s[2].draw(mark, 2);
  Print("Region 3 (markiert)\n");
  GetChar();

  ClearImg(img);

  int i;
  int sel;

  for (i = 0; i < 3; i++)
    s[i].draw(mark, i + 1);

  GetChar();

  Print("Berechne \"Summe\"\n");
  Region ss = s[0] + s[1] + s[2];
  ClearImg(mark);
  ss.draw(mark, 3);

  double ta = TimeD();

  Print("Subtrahiere/Addiere s[2] 10 mal\n");

  for (int i = 0; i < 10; i++)
    {
      ss = ss - s[2];
      ss = ss + s[2];
    }

  Printf("done (%5.2f)\n", TimeD() - ta);

  ClearImg(mark);
  ss.draw(mark, 3);

  GetChar();

  bool ende = false;

  i = Random(2);

  while (!ende)
    {
      i = Random(2);
      sel = Random(3);
      double ta = TimeD();

      switch (sel)
        {
        case 0:
        case 1:
        {
          Printf("Add %d\n", i);
          ss.add(s[i]);
        }
        break;
        case 2:
        {
          Printf("Del %d\n", i);
          ss.del(s[i]);
        }
        break;
        case 3:
        {
          Printf("intersect %d\n", i);
          ss.intersect(s[i]);
        }
        break;
        }

      Printf("%5.2f\n", (TimeD() - ta) * 1000);
      ClearImg(mark);
      ss.draw(mark, 3);
      ende = GetChar() == 'q';
    }

  return OK;
}

#include <image.h>

void drawBox(int x1, int y1, int x2, int y2, const Image& img)
{
  for (int y = y1; y <= y2; y++)
    for (int x = x1; x <= x2; x++)
      PutVal(img, x, y, 255);
}

int main(int argc, char** argv)
{
  Image g1, m1;

  int ps[2] = {1, 1};
  //  int x,y;
  Matrix p1, p2;
  Contur c2;

  g1 = NewImg(256, 512, 255);
  m1 = NewImg(256, 512, 255);

  Show(OVERLAY, g1, m1, "Bild und Konturen");

  Alpha(ON);

  ClearAlpha();

  ClearImg(g1);
  ClearImg(m1);

  drawBox(5, 5, 5, 5, g1);
  drawBox(5, 25, 6, 26, g1);
  drawBox(5, 45, 14, 54, g1);

  drawBox(5, 65, 6, 65, g1);
  drawBox(5, 85, 5, 86, g1);

  drawBox(5, 105, 104, 204, g1);

  drawBox(5, 225, 104, 225, g1);
  drawBox(5, 245, 5, 344, g1);

  drawBox(15, 255, 34, 264, g1);
  drawBox(15, 255, 24, 274, g1);

  InvertImg(g1);

  while (SearchStart(g1, m1, NULL, 1, 1, ps, HORZ) == OK)
    {
      c2 = CalcContur(g1, m1, NULL, 1, ps, 0, isunknown, isobject);

      if (c2.isClosed())
        {
          if (c2.isHole()) Printf("Loch - ");
          else Printf("Objekt - ");
        }
      else
        Printf("Nicht geschlossen");

      MarkContur(c2, 2, m1);
      double length, area, form, conv;
      if (c2.isClosed())
        {
          FeatureContur(c2, length, area, form, conv);
          Printf("length: %lf  area: %lf  form: %lf  conv: %lf", length, area, form, conv);
//      cout << length << " " <<  area << " " << form << " " << conv << endl;
        }
      Print("\n");
      //    GetChar();
    }

  Printf("OK");
  GetChar();
  return 0;
}

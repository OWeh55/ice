/*
 * programm zum testen der Funktionen ReducePolygon und FitPolygonContur
 */

#include <image.h>

void reduceAndShow(const Contur& c1, Image& mark)
{
  Matrix pl = ReducePolygon(c1, 4);
  drawPolygon(pl, 1, mark);

  for (int i = -1; i < 3; i++)
    {
      Matrix pl1 = FitPolygonContur(pl, c1, i);
      drawPolygon(pl1, 3 + i, mark);
      GetChar();
    }
}

int main(int argc, char* argv[])
{
  Image img;
  img.create(600, 600, 255);
  Image mark;
  mark.create(600, 600, 8);
  Show(OVERLAY, img, mark);
  clearImg(img);
  clearImg(mark);

  // ideales Rechteck
  Contur c1;
  Matrix pl;
  pl = Vector(410, 420) && Vector(180, 410) && Vector(190, 180) && Vector(420, 190);
  c1 = PolygonContur(pl);
  FillRegion(c1, 255, img);
  reduceAndShow(c1, mark);

  // Kreis
  clearImg(img);
  clearImg(mark);
  Circle ci(300, 300, 140);
  draw(ci, img, 255, 255);
  IPoint pp(300, 300);
  SearchStart(img, mark, NULL, 15, 1, pp);
  c1 = CalcContur(img, mark, NULL, 15, pp);
  reduceAndShow(c1, mark);

  // abgerundetes Rechteck
  clearImg(mark);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        if (abs(x - 300) + abs(y - 300) > 165) PutVal(img, x, y, 0);
      }
  pp = IPoint(300, 300);
  SearchStart(img, mark, NULL, 15, 1, pp);
  c1 = CalcContur(img, mark, NULL, 15, pp);
  reduceAndShow(c1, mark);

  //
  clearImg(img);
  clearImg(mark);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        if (abs(x - 300) + abs(y - 300) < 165) PutVal(img, x, y, 255);
      }

  for (int i = 0; i < 5; i++)
    dilateImg(img, img, 3, 3);

  pp = IPoint(300, 300);
  SearchStart(img, mark, NULL, 15, 1, pp);
  c1 = CalcContur(img, mark, NULL, 15, pp);
  reduceAndShow(c1, mark);

  // gestörtes Rechteck
  clearImg(img);
  clearImg(mark);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        if (abs(x - 300) + abs(y - 300) < 165) PutVal(img, x, y, 255);

        if (Random(19) < 1) PutVal(img, x, y, 255);

        if (Random(19) < 1) PutVal(img, x, y, 0);
      }
  dilateImg(img, img, 3, 3);
  erodeImg(img, img, 3, 3);
  pp = IPoint(300, 300);
  SearchStart(img, mark, NULL, 15, 1, pp);
  c1 = CalcContur(img, mark, NULL, 15, pp);
  reduceAndShow(c1, mark);

  return OK;
}

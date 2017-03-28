/********************************************************************
base_4    Test Falschfarben - Colorierung
********************************************************************/
#include <stdio.h>
#include <image.h>

#define NUMB_RASTER 11

#define COUNT 2.0

using namespace ice;

int main(int argc, char* argv[])
{
  Image img2, img1;
  int xsize, ysize;
  int x, y, i, j, k, x1, y1, y2, dx, dy;
  int a[11] = {0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 255};
  char t[11][4] =
  {
    {' ', ' ', '0', 0},
    {' ', '2', '5', 0},
    {' ', '5', '0', 0},
    {' ', '7', '5', 0},
    {'1', '0', '0', 0},
    {'1', '2', '5', 0},
    {'1', '5', '0', 0},
    {'1', '7', '5', 0},
    {'2', '0', '0', 0},
    {'2', '2', '5', 0},
    {'2', '5', '5', 0}
  };
  char t1[11][5] =
  {
    {' ', ' ', '0', '%', 0},
    {' ', '1', '0', '%', 0},
    {' ', '2', '0', '%', 0},
    {' ', '3', '0', '%', 0},
    {' ', '4', '0', '%', 0},
    {' ', '5', '0', '%', 0},
    {' ', '6', '0', '%', 0},
    {' ', '7', '0', '%', 0},
    {' ', '8', '0', '%', 0},
    {' ', '9', '0', '%', 0},
    {'1', '0', '0', '%', 0}
  };

  ClearAlpha();
  SetAlphaCursor(10, 5);
  Printf("Test Falschfarben - Colorierung\n");
  SetAlphaCursor(18, 18);
  Printf("Bitte warten !!!\n");
  Delay(5.0);

  SetOverlayColor(4, 100, 100, 100);
  SetOverlayColor(5, 0, 0, 0);
  xsize = InfVis(MAXX) / 2;
  ysize = InfVis(MAXY) / 2;
  dy = ysize / (NUMB_RASTER + 3);
  y1 = 2 * dy;
  y2 = 3 * dy;
  dx = dy;
  x1 = (xsize - (NUMB_RASTER * dx)) / 2;
  img1 = NewImg(xsize, ysize, 127);
  img2 = NewImg(xsize, ysize, 127);
  Show(OVERLAY, img1, img2);
  Display(ON);

  for (y = 0; y < y2; y++)
    for (x = 0; x < xsize; x++)
      PutVal(img2, x, y, 4);

  for (y = y2; y < ysize; y++)
    for (x = 0; x < x1; x++)
      PutVal(img2, x, y, 4);

  for (y = y2 + 11 * dy; y < ysize; y++)
    for (x = x1; x < xsize; x++)
      PutVal(img2, x, y, 4);

  for (y = y2; y < y2 + 11 * dy; y++)
    for (x = x1 + 11 * dx; x < xsize; x++)
      PutVal(img2, x, y, 4);

  Line(1, y1, xsize - 1, y1, 5, 0, img2);
  Line(1, y2, xsize - 1, y2, 5, 0, img2);
  Line(x1, y1, x1, ysize - 1, 5, 0, img2);

  for (i = 0; i < 11; i++)
    Line(x1 + (i + 1)*dx, y2, x1 + (i + 1)*dx, y2 + 11 * dy, 5, 0, img2);

  for (i = 0; i < 11; i++)
    Line(x1, y2 + (i + 1)*dy, x1 + 11 * dx, y2 + (i + 1)*dy, 5, 0, img2);

  for (i = 0; i < 11; i++)
    for (j = 0; j < 11; j++)
      for (x = x1 + j * dx; x < x1 + (j + 1)*dx; x++)
        for (y = y2 + i * dy; y < y2 + (i + 1)*dy; y++)
          PutVal(img1, x, y, 11 * i + j);



  /* Colorierung mit konstantem Rotanteil */
  Text("Colorierung mit konstantem Rotanteil", 5, 5, 1, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 3, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 2, 0, img2);

  for (k = 0; k < 11; k++)
    {
      Text((char*)t1[k], 5, 25, 1, 1, img2);

      for (i = 0; i < 11; i++)
        for (j = 0; j < 11; j++) SetGreyColor((11 * i) + j, a[k], a[i], a[j]);

      Delay(COUNT);
      Text((char*)t1[k], 5, 25, 4, 1, img2);
    }

  Text("Colorierung mit konstantem Rotanteil", 5, 5, 4, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 4, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 4, 0, img2);

  /*************************************************************/
  /* Colorierung mit konstantem Grnanteil */
  Text("Colorierung mit konstantem Grnanteil", 5, 5, 2, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 3, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 1, 0, img2);

  for (k = 0; k < 11; k++)
    {
      Text((char*)t1[k], 5, 25, 2, 1, img2);

      for (i = 0; i < 11; i++)
        for (j = 0; j < 11; j++) SetGreyColor((11 * i) + j, a[i], a[k], a[j]);

      Delay(COUNT);
      Text((char*)t1[k], 5, 25, 4, 1, img2);
    }

  Text("Colorierung mit konstantem Grnanteil", 5, 5, 4, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 4, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 4, 0, img2);

  /*************************************************************/
  /* Colorierung mit konstantem Blaunteil */
  Text("Colorierung mit konstantem Blauanteil", 5, 5, 3, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 2, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 1, 0, img2);

  for (k = 0; k < 11; k++)
    {
      Text((char*)t1[k], 5, 25, 3, 1, img2);

      for (i = 0; i < 11; i++)
        for (j = 0; j < 11; j++) SetGreyColor((11 * i) + j, a[i], a[j], a[k]);

      Delay(COUNT);
      Text((char*)t1[k], 5, 25, 4, 1, img2);
    }

  Text("Colorierung mit konstantem Blauanteil", 5, 5, 4, 1, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 + i * dx + 5, y2 - 12, 4, 0, img2);

  for (i = 0; i < 11; i++)
    Text((char*)t[i], x1 - 30, y2 + i * dy + 5, 4, 0, img2);

  Display(OFF);
  return 0;
}

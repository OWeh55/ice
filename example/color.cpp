#include <cstdio>
#include <image.h>

using namespace ice;
using namespace std;

#define STEP_NR 11

#define DELAY 0.5

void GrundMuster(Image& img, Image& mrk, int& dx, int& dy, int& x1, int& y2)
{
  int xsize = img->xsize;
  int ysize = img->ysize;
  dy = ysize / (STEP_NR + 4);
  int y1 = 2 * dy;
  y2 = 3 * dy;
  dx = dy;
  x1 = (xsize - (STEP_NR * dx)) / 2;
  int x2 = x1 + STEP_NR * dx;

  // Rand markieren mit 4
  for (int y = 0; y < y2; y++)
    for (int x = 0; x < xsize; x++)
      PutVal(mrk, x, y, 4);

  for (int y = y2; y < ysize; y++)
    for (int x = 0; x < x1; x++)
      PutVal(mrk, x, y, 4);

  for (int y = y2 + STEP_NR * dy; y < ysize; y++)
    for (int x = x1; x < xsize; x++)
      PutVal(mrk, x, y, 4);

  for (int y = y2; y < y2 + STEP_NR * dy; y++)
    for (int x = x1 + STEP_NR * dx; x < xsize; x++)
      PutVal(mrk, x, y, 4);

  Line(1, y1, xsize - 1, y1, 5, 0, mrk);
  Line(1, y2, xsize - 1, y2, 5, 0, mrk);
  Line(x1, y1, x1, ysize - 1, 5, 0, mrk);
  Line(x2, y1, x2, ysize - 1, 5, 0, mrk);

  for (int i = 0; i < STEP_NR; i++)
    Line(x1 + (i + 1)*dx, y2, x1 + (i + 1)*dx, y2 + STEP_NR * dy, 5, 0, mrk);

  for (int i = 0; i < STEP_NR; i++)
    Line(x1, y2 + (i + 1)*dy, x1 + STEP_NR * dx, y2 + (i + 1)*dy, 5, 0, mrk);

  // farbige Quadrate
  for (int i = 0; i < STEP_NR; i++)
    for (int j = 0; j < STEP_NR; j++)
      for (int x = x1 + j * dx; x < x1 + (j + 1)*dx; x++)
        for (int y = y2 + i * dy; y < y2 + (i + 1)*dy; y++)
          PutVal(img, x, y, STEP_NR * i + j);
}

void Menu()
{
  ClearAlpha();
  Print("R - Rot als Grundfarbe\n");
  Print("G - Gruen als Grundfarbe\n");
  Print("B - Blau als Grundfarbe\n");
  Print("+ - Intensitaet erhoehen\n");
  Print("- - Intensitaet vermindern\n");
  Print("X - Exit\n");
  Print("G - Quit\n");
}
int main(int argc, char* argv[])
{
  Image img2, img1;
  int xsize, ysize;

  bool abort = false;

  int a[STEP_NR] = {0, 25, 50, 75, 100, 125, 150, 175, 200, 225, 255};

  vector<string> colors;
  colors.push_back("rot");
  colors.push_back("gruen");
  colors.push_back("blau");

  //  xsize=InfVis(MAXX)-2;ysize=InfVis(MAXY)-2;
  xsize = 1000;
  ysize = 800;

  img1 = NewImg(xsize, ysize, 127);
  img2 = NewImg(xsize, ysize, 127);
  Show(OVERLAY, img1, img2);

  SetOverlayColor(4, 100, 100, 100);
  SetOverlayColor(5, 0, 0, 0);

  int x1, y2;
  int dx, dy;

  Menu();

  int sel = 0;
  int intensity = STEP_NR / 2;

  while (!abort)
    {
      GrundMuster(img1, img2, dx, dy, x1, y2);

      Text("Colorierung mit konstantem Anteil " + colors[sel], 5, 5, 1, 1, img2);
      Text(NumberString(a[intensity] * 100 / 255) + "%", 5, 25, 1, 1, img2);

      switch (sel)
        {
        case 0:

          // Zahlenwerte in farbiger Markierung
          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 + i * dx + 5, y2 - 12, 3, 0, img2);

          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 - 30, y2 + i * dy + 5, 2, 0, img2);

          // Farbtabelle setzen
          for (int i = 0; i < STEP_NR; i++)
            for (int j = 0; j < STEP_NR; j++)
              SetGrayColor((STEP_NR * i) + j, a[intensity], a[i], a[j]);

          break;

        case 1:

          // Zahlenwerte in farbiger Markierung
          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 + i * dx + 5, y2 - 12, 3, 0, img2);

          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 - 30, y2 + i * dy + 5, 1, 0, img2);

          for (int i = 0; i < STEP_NR; i++)
            for (int j = 0; j < STEP_NR; j++)
              SetGrayColor((STEP_NR * i) + j, a[i], a[intensity], a[j]);

          break;

        case 2:

          // Zahlenwerte in farbiger Markierung
          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 + i * dx + 5, y2 - 12, 2, 0, img2);

          for (int i = 0; i < STEP_NR; i++)
            Text(NumberString(a[i]), x1 - 30, y2 + i * dy + 5, 1, 0, img2);

          for (int i = 0; i < STEP_NR; i++)
            for (int j = 0; j < STEP_NR; j++)
              SetGrayColor((STEP_NR * i) + j, a[i], a[j], a[intensity]);

          break;
        }

      int input = GetChar();

      switch (input)
        {
        case 'q':
        case 'x':
        case 'Q':
        case 'X':
          abort = true;
          break;
        case 'r':
        case 'R':
          sel = 0;
          break;
        case 'g':
        case 'G':
          sel = 1;
          break;
        case 'b':
        case 'B':
          sel = 2;
          break;
        case '+':

          if (intensity < STEP_NR - 1)
            intensity++;

          break;
        case '-':

          if (intensity > 0)
            intensity--;

          break;
        }

    }

  Display(OFF);

  return 0;
}

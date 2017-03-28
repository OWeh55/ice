#include <image.h>

int main(int argc, char* argv[])
{
  int xs, ys, i;
  Image img1, img2;
  xs = InfVis(MAXX) / 2;
  ys = InfVis(MAXY) / 2;
  img1 = NewImg(xs - 2, ys, 255);
  img2 = NewImg(xs - 2, ys, 255);
  Display(ON);
  Show(ON, img1, "Grau");
  Show(OVERLAY, img1, img2, "OVERLAY");
  /*
  Show(OVERLAY1, img1, img2, "OVERLAY1");
  Show(OVERLAY2, img1, img2, "OVERLAY2");
  Show(OVERLAY3, img1, img2, "OVERLAY3");
  */
  for (i = 0; i < xs - 2; i++) Line(i, 0, i, img1->ysize - 1, i / 2 % 256, DEFAULT, img1);

  for (i = 0; i < ys - 2; i++) Line(0, i, img1->xsize - 1, i, i / 16 % 8, DEFAULT, img2);

  GetChar();

  Print("Linie auswählen\n");
  LineSeg l = SelLine(img1);
  draw(l, img1, 255);
  Print("Fertig\n");

  GetChar();

  Display(OFF);

  return 0;
}

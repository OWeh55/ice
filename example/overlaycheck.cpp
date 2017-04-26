#include <image.h>

int main(int argc, char* argv[])
{

  Image p[2], m;

  OpenAlpha("overlaycheck");
  p[0] = NewImg(512, 512, 64);
  p[1] = NewImg(512, 512, 255);
  m = NewImg(512, 512, 64);

  for (int y = 0; y < p[0].ysize; y++)
    for (int x = 0; x < p[0].xsize; x++)
      PutVal(p[0], x, y, x % (p[0]->maxval));

  for (int y = 0; y < p[1].ysize; y++)
    for (int x = 0; x < p[1].xsize; x++)
      PutVal(p[1], x, y, y % (p[1]->maxval));

  Printf("Show picture with MaxIndex = %d\t\t", p[1]->maxval);
  Show(ON, p[1]);
  Printf("OK\n\n");
  GetChar();

  Printf("Show overlay pic where\n\tbaseimage has MaxIndex = %d\n\toverlayimage has MaxIndex = %d\t\t",
         p[0]->maxval, m->maxval);
  Show(OVERLAY, p[0], m);
  Printf("OK\n\n");
  GetChar();

  Printf("Drawing diagonal line in overlay image with color RED\t\t");
  Line(0, 0, p[0]->xsize - 1, p[0]->ysize - 1, 1, m);
  Printf("OK\n\n");
  GetChar();

  Printf("Changing overlay color for value RED to white (255,255,255)\t\t");
  SetOverlayColor(1, 255, 255, 255);
  Printf("OK\n\n");
  GetChar();

  Printf("Now changing GreyLUT to have Greyvalues from 0..255 in the intervall [16,128]\t\t");
  SetGreyLUT(16, 128);
  Printf("OK\n\n");
  GetChar();

  Printf("Changing greyvalue  at index 64 to GREEN (0,255,0)");
  SetGreyColor(64, 0, 255, 0);
  Printf("OK\n\n");
  GetChar();


  Printf("Now switching overlay colors cyclically on keypress. end with typing 'X'");
  int i = 1;

  while (GetChar() != 'X')
    setImg(m, (i++) % 64);

  return 0;
}

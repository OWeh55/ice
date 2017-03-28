#include <image.h> // needed for translation of main to Main
using namespace ice;

const int xsize = 256;
const int ysize = 256;
const int gsize = 256;

int main(int argc, char* argv[])
{
  Image ir, ig, ib;
  int x, y;
  Alpha(ON);
  ClearAlpha();
  Display(ON);
  ir = NewImg(xsize, ysize, gsize - 1);
  ig = NewImg(xsize, ysize, gsize - 1);
  ib = NewImg(xsize, ysize, gsize - 1);
  Image ir1, ig1, ib1;
  ir1 = NewImg(xsize, ysize, gsize - 1);
  ig1 = NewImg(xsize, ysize, gsize - 1);
  ib1 = NewImg(xsize, ysize, gsize - 1);
  Show(_RGB, ir, ig, ib);
  Show(_RGB, ir1, ig1, ib1);
  Print("Generiere Testbild\n");
  wloop(ir, x, y)
  {
    PutVal(ir, x, y, (x + y)*gsize / (xsize + ysize));
    PutVal(ig, x, y, (xsize - 1 - x + y)*gsize / (xsize + ysize));
    PutVal(ib, x, y, (x + ysize - 1 - y)*gsize / (xsize + ysize));
  }

  Print("Schreibe Testbilder RGB und GRAU\n");

  WriteJPEGImg(ir, ig, ib, "test_rgb.jpg");
  WriteJPEGImg(ir, "test_gray.jpg");

  GetChar();

  Print("Bestimme Bild-Datei-Informationen\n");
  int xs, ys, gm, nr;

  InfJPEGFile("test_rgb.jpg", xs, ys, gm, nr);
  Print("  test_rgb.jpg : " + NumberString(xs) + "*" + NumberString(ys) + " "
        + NumberString(gm) + " " + NumberString(nr) + "\n");

  InfJPEGFile("test_gray.jpg", xs, ys, gm, nr);
  Print("  test_gray.jpg : " + NumberString(xs) + "*" + NumberString(ys) + " "
        + NumberString(gm) + " " + NumberString(nr) + "\n");

  GetChar();

  Print("Lese Farb-Bilder ein (rechtes Bild):\n");
  ClearImg(ir1);
  ClearImg(ig1);
  ClearImg(ib1);
  Print("test_rgb.jpg\n");
  ReadJPEGImg("test_rgb.jpg", ir1, ig1, ib1);
  GetChar();
  Print("a2.jpg\n");
  ReadJPEGImg("a2.jpg", ir1, ig1, ib1);
  GetChar();
  Show(OFF, ir);
  Show(OFF, ir1);
  Show(ON, ir);
  Show(ON, ir1);
  Print("Lese Grauwert-Bilder ein (rechtes Bild):\n");
  ClearImg(ir1);
  Print("test_gray.jpg\n");
  ReadJPEGImg("test_gray.jpg", ir1);
  GetChar();
  Print("test_rgb.jpg\n");
  ReadJPEGImg("test_rgb.jpg", ir1);
  GetChar();
  Print("a2.jpg\n");
  ReadJPEGImg("a2.jpg", ir1);
  GetChar();

  return 0;
}








#include <image.h>

int main(int argc, char** argv)
{
  OpenAlpha("Teilbild-Test");
  Alpha(ON);

  Print("Test-Programm für Bilder und Sub-Bilder\n");
  Image img = ReadImg("test_gray.jpg");
  Image sub = NewImg(img, Window(5, 5, 311, 311));
  Image subsub = NewImg(sub, Window(95, 15, 211, 211));
  Show(ON, img, "base image");
  Show(ON, sub, "subimage");
  GetChar();
  Printf("Image %d %d %d\n", img->xsize, img->ysize, img->maxval);
  Printf("Sub %d %d %d\n", sub->xsize, sub->ysize, sub->maxval);
  Printf("SubSub %d %d %d\n", subsub->xsize, subsub->ysize, subsub->maxval);
  Printf("\nInvertiere sub\n");
  InvertImg(sub);
  GetChar();
  Printf("\nInvertiere subsub\n");
  InvertImg(subsub);
  GetChar();
  Printf("\nInvertiere img(Window(222,333,444,389))\n");
  InvertImg(img(Window(222, 333, 444, 389)));
  GetChar();
  return OK;
}

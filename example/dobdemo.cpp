#include <image.h>


int main(int argc, char** argv)
{
  string fn = "fax.tif";

  Image img = ReadImg(fn);
  Show(ON, img, "Original");

  WindowWalker ww(img);
  for (ww.init(); !ww.ready(); ww.next())
    {
      int shift = 20 * cos(ww.x * 0.02) + 20;
      img.setPixelLimited(ww, img.getPixel(ww) + shift);
    }

  Image smear1 = NewImg(img);
  smearImg(img, smear1, 7);
  Show(ON, smear1, "Smear1");
  Image smear2 = NewImg(img);
  smearImg(img, smear2, 111);
  Show(ON, smear2, "Smear2");
  Image dobimg = NewImg(img);
  Show(ON, dobimg, "DoB");
  subImg(smear1, smear2, dobimg, SMD_POSITIVE);

  IPoint p1, p2;
  IPoint ph;
  int rc = 0;
  while (rc >= 0)
    {
      Print("Auswahl links/oben\n");
      ph = SelPoint(dobimg, rc);
      if (rc >= 0)
        {
          p1 = ph;
          Print("Auswahl rechts/unten\n");
          ph = SelPoint(dobimg, rc);
          if (rc >= 0)
            p2 = ph;
        }
    }
  Printf("(%d,%d),(%d,%d)\n", p1.x, p1.y, p2.x, p2.y);
  Window w(p1, p2);
  WriteImg(img(w), "original.jpg");
  WriteImg(smear1(w), "smear1.jpg");
  WriteImg(smear2(w), "smear2.jpg");
  WriteImg(dobimg(w), "dob.jpg");
  GetChar();
}

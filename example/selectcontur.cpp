#include <image.h>

int main(int argc, char** argv)
{
  Image img = NewImg(999, 888, 255);
  Show(ON, img);
  cout << "Contur auswählen" << endl;
  Contur c = SelContur(img, false);
  cout << "Contur fertig" << endl;
  if (c.isValid())
    {
      if (c.isClosed())
        FillRegion(c, 127, img);
      else
        MarkContur(c, 127, img);
    }
  GetChar();
  return OK;
}

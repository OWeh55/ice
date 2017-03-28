#include <image.h>
#include <region.h>

using namespace ice;

int main(int argc, char* argv[])
{
  Image img;
  img = ReadImg("test_gray.jpg");
  Image mark = NewImg(img);
  ClearImg(mark);
  Show(OVERLAY, img, mark);
  ClearAlpha();
  Display(ON);
  Region s1;
  Region s2, s3;

  int x, y;

  cout << "Test Region (bildliche Darstellung)" << endl;
  wloop(img, x, y)
  {
    if (GetVal(img, x, y) & 1)
      {
        s1.add(x, y);
      }
  }
  s1.draw(mark, 1);

  GetChar();

  wloop(img, x, y)
  {
    if (GetVal(img, x, y) & 2)
      {
        s2.add(x, y);
      }
  }
  s2.draw(mark, 2);

  GetChar();

  wloop(img, x, y)
  {
    if (GetVal(img, x, y) & 4)
      {
        s3.add(x, y);
      }
  }
  s3.draw(mark, 3);

  GetChar();

  s1.add(s2);
  s1.draw(mark, 4);
  GetChar();

  s1.add(s3);
  s1.draw(mark, 4);
  GetChar();

  s1.del(s3);
  s1.draw(mark, 2);
  GetChar();

  s1.del(s2);
  s1.draw(mark, 1);
  GetChar();

  return OK;
}

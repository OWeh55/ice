#include <image.h>

#define XSIZE 752
#define YSIZE 480

int main(int argc, char** argv)
{
  if (argc < 2)
    {
      cerr << argv[0] << " filename" << endl;
      return 1;
    }

  string fn = argv[1];
  cout << "-->" << fn << "<---" << endl;
  Image img;
  img.create(XSIZE, YSIZE, 256 * 256 - 1);
  clearImg(img);
  Show(ON, img);
  ReadImg(fn, img);
#if 0
  int x, y;
  int min, max;
  min = max = GetVal(img, 0, 0);
  wloop(img, x, y)
  {
    int g = GetVal(img, x, y);

    if (g > max) max = g;

    if (g != 0)
      if (g < min) min = g;
  }
  Printf("Max: %d   Min: %d   Dif: %d\n", max, min, max - min);
  int dg = max - min + 1;
  wloop(img, x, y)
  {
    int g = limited(MulDiv(GetVal(img, x, y) - min, (256 * 256 - 1), dg), 256 * 256 - 1);
    PutVal(img, x, y, g);
  }
//#else
  int x, y;
  wloop(img, x, y)
  {
    PutVal(img, x, y, (GetVal(img, x, y) << 6) & 0xffff);
  }
#endif
  GetChar();
  return 0;
}

#include <image.h>

int main(int argc, char** argv)
{
  Image orig;
  Image grad;
  Image lsi;
  Image diff;
  orig = ReadImg("test_gray.jpg");
  Show(ON, orig, "Original");
  grad = NewImg(orig);
  Show(ON, grad, "GradX");
  lsi = NewImg(orig);
  Show(ON, lsi, "lsi");
  diff = NewImg(orig);
  Show(ON, diff, "diff");
  IMatrix fm(0, 3);
  fm.Append(IVector(-1, 0, 1));
  fm.Append(IVector(-1, 0, 1));
  fm.Append(IVector(-1, 0, 1));
  LSIFilter f(fm, 6);
  double ta = TimeD();

  for (int i = 0; i < 100; ++i)
    GradXImg(orig, grad);

  cout << "Zeit GradXImg " << TimeD() - ta << endl;
  ta = TimeD();

  for (int i = 0; i < 100; ++i)
    LSIImg(orig, lsi, f, lsi->maxval / 2);

  cout << "Zeit LSIImg " << TimeD() - ta << endl;

  WindowWalker p(diff);
  for (p.init(); ! p.ready(); p.next())
    {
      PutVal(diff, p, limited(3 * (GetVal(grad, p) - GetVal(lsi, p)) + 128, diff));
    }

  GetChar();
  return 0;
}

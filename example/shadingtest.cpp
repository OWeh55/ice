#include <image.h>

void showShading(const Image& src, const Image& dst, const Image& shading,
                 Function2dWithFitting& fn)
{
  correctShading(src, dst, fn);
  SetImg(shading, fn);
  GetChar();
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  if (argc > 1)
    fn = argv[1];
  Image src = ReadImg(fn);
  Image dst = NewImg(src);
  Image shading = NewImg(src);
  Show(ON, src, "Ausgangsbild");
  Show(ON, dst, "Zielbild");
  Show(ON, shading, "die Shading-Funktion");

  Constant2d fnc;
  showShading(src, dst, shading, fnc);

  Polynom2d1o fn1;
  showShading(src, dst, shading, fn1);

  Polynom2d2o fn2;
  showShading(src, dst, shading, fn2);

  int ord = 3;

  do
    {
      Printf("aktuelle Ordnung: %d\n", ord);
      Polynom2d fn(ord);
      showShading(src, dst, shading, fn);
      ord++;
    }
  while (ord < 1000);

  Print("Fertig\n");
  GetChar();

  return 0;
}

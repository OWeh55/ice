#include <image.h>

int main(int argc, char** argv)
{
  if (argc <= 4)
    {
      cout << "Aufruf: " << argv[0] << " umgebung grad i j" << endl;
      exit(1);
    }

  int umg = atol(argv[1]);
  int grad = atol(argv[2]);
  int i = atol(argv[3]);
  int j = atol(argv[4]);

  double p, m;
  LSIFilter f = mkPolynomFilter(umg, grad, i, j);
  f.rep->sumpm(p, m);
  cout << p << " " << m << endl;
  f.rep->normalize();

  Image src = ReadImg("test_gray.jpg");
  Image dst = NewImg(src);

  cout << f << endl;
  cout << f.getOffset(dst) << endl;

  Show(ON, dst);
  f.Filter(src, dst, f.getOffset(dst));
  GetChar();
  return OK;
}

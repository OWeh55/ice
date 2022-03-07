#include <image.h>

int main(int argc, char** argv)
{
  Accumulator a(2000, 0, 10, false, 3);
  srand48(time(NULL));
  for (int i = 0; i < 100000; i++)
    {
      double v = 0;
      for (int k = 0; k < 10; k++)
        v += drand48();
      a.add(v);
    }
  double v, f;
  a.getMax(v, f);
  cout << "value: " << v << "  frequency: " << f << endl;
  return 0;
}

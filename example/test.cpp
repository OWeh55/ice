#include <image.h>

int main(int argc, char** argv)
{
  Image img(1000, 700, 255);
  Show(ON, img);
  Line(1010, 5, 5, 705, 127, img);
  for (int x = -100; x < 1000; x += 10)
    {
      cout << x << ",20, 330," << x << endl;
      Line(x, 20, 330, x, 255, img);
    }
  GetChar();
}

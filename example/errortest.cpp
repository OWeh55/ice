#include <image.h>

int main(int argc, char** argv)
{
  Image img;
  img.create(222, 333, 255);
  img.setPixel(500, 5, 3);
}

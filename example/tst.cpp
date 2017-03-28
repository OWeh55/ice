#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <image.h>

using namespace std;

int main(int main, char** argv)
{
  Printf("Hallo\n");
  FILE* fd = popen("mplayer -really-quiet -identify -frames 0 a.mov", "r");

  if (fd)
    {
      while (!feof(fd))
        {
          int g = fgetc(fd);
          Printf("%c", g);
        }
    }
  else
    {
      Printf("Kann mplayer nicht öffnen");
    }

  GetChar();
  return 0;
}

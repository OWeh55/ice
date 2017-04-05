/*
 * test of the four modes ON,OFF,ENABLE,DISABLE of Display()
 */

#include <image.h>

#define QUIT if (GetChar()=='x') break

int main(int argc, char* argv[])
{
  int i;
  Image p = NewImg(512, 512, 255);
  p.set(0);
  Show(ON, p); // announce visualization

  OpenAlpha("zack");

  // main loop
  while (1)
    {
      ClearAlpha();

      Printf("Filling Image... \n");

      for (i = 0; i < 512; i++)
        for (int j = 511; j >= 0; j--)
          PutVal(p, i, j, j % 255);

      Printf("Ready. Hit a key to switch display ON\n");
      QUIT;
      Display(ON);

      Printf("Hit a key to disable display\n");
      QUIT;

      Display(OFF);
    } // main loop

  Show(OFF, p);
  return 0;
}


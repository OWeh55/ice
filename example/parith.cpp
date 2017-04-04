/**************************************************************/
/* Test Bildarithmetik                                        */
/**************************************************************/

#include <stdio.h>
#include <image.h>

string modes[3] = {"MD_NORMALIZE", "MD_LIMIT", "MD_MODULO"};
string smodes[3] = {"SMD_ABSOLUTE", "SMD_POSITIVE", "SMD_SHIFT"};

int main(int argc, char* argv[])
{
  Image p1, p2, p3;
  int mode, smode;

  int x, y;
  OpenAlpha("Arithmetiktest");
  Alpha(ON);
  ClearAlpha();
  Display(ON);
  p1 = NewImg(256, 256, 255);
//  Show(ON,p1);

  for (int y=0;y<p1.ysize;y++)
    for (int x=0;x<p1.xsize;x++)
      PutVal(p1, x, y, x);

  p2 = NewImg(256, 256, 255);
//  Show(ON,p2);

  for (int y=0;y<p2.ysize;y++)
    for (int x=0;x<p2.xsize;x++)

    PutVal(p2, x, y, y);

  p3 = NewImg(256, 256, 127);
  Show(ON, p3);

//  p4=NewImg(256,256,1000000);
//  Show(ON,p4);

  for (mode = MD_NORMALIZE; mode <= MD_MODULO; mode++)
    {
      Print("Add " + modes[mode - 1] + "\n");
      AddImg(p1, p2, p3, mode);
      GetChar();
    }

// ----------------------------------

  for (smode = SMD_ABSOLUTE; smode <= SMD_SHIFT; smode++)
    for (mode = MD_NORMALIZE; mode <= MD_MODULO; mode++)
      {
        Print("Sub " + smodes[smode - 1] + " " + modes[mode - 1] + "\n");
        SubImg(p1, p2, p3, smode, mode);
        GetChar();
      }

  Display(OFF);

  return 0;

}

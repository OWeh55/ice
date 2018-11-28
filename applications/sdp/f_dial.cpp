#include <image.h>
#include "sdp.h"
#include "f_dial.h"

bool handleDials()
{
  static int current = 0;
  Alpha(ON);

  bool continue_ = true;
  while (continue_)
    {
      ClearAlpha();
      Print("4 <-> 6 = change value \n");
      Print("8 <-> 2 = select dial \n");
      Print("ESC - abort\n\n");
      for (int i = 0; i < (int)dials.size(); ++i)
        {
          Dial& cdd = *(dials[i]);
          if (i == current)
            Print("*");
          else
            Print(" ");
          Print(cdd.getName());
          Printf(" %d   %d ... %d  step:%d", cdd.value, cdd.min, cdd.max, cdd.step);
          Print("\n");
        }
      Dial& cd = *(dials[current]);
      int key = GetChar();

      switch (key)
        {
        case K_Left:
        case '4':
          cd.down();
          continue_ = false;
          break;
        case '6':
        case K_Right:
          cd.up();
          continue_ = false;
          break;
        case K_Up:
        case '8':
          current--;
          if (current < 0)
            current = 0;
          break;
        case K_Down:
        case '2':
          current++;
          if (current >= (int)dials.size())
            current--;
          break;
        case '+':
          cd.coarse();
          break;
        case '-':
          cd.fine();
          break;
        case 27:
          return false;
        default:
          Print(string("Unknown Key ") + to_string(key));
        }

    }
  return true;
}

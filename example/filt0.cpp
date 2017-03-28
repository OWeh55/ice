/**************************************************************/
/* Test Grundfunktionen lokale Filter                         */
/**************************************************************/

#include <stdio.h>
#include <image.h>
#include <string.h>

#define path "./"

#define TIMES 500

int main(int argc, char* argv[])
{
  string fn = "test.jpg";

  Image p1, p2;
#define KSIZE 13

  int sum = 0;
  vector<int> mask;
  int min = 0;
  int max = 0;

  for (unsigned int i = 0; i < KSIZE; i++)
    {
      int val = i - KSIZE / 2;

      if (val < 0) val = -1;
      else val = 1;

      mask.push_back(val);

      if (val > 0) max += val;

      if (val < 0) min += val;
    }

  cout << min << ".." << max << "  "  << endl;

  sum = max - min;
  int offset = -min * 255 / sum;

  cout << "\"sum\": " << sum << " \"offset\": " << offset << endl;

  if (argc > 1) fn = argv[1];

  p1 = ReadImg(fn);

  Show(ON, p1, "original");
  p2 = NewImg(p1);

  Show(ON, p2, "gefiltert");
  Print("Horizontal gefiltert\n");
  LSIHImg(p1, p2, mask, sum, offset);
  GetChar();
  Print("Vertikal gefiltert\n");
  LSIVImg(p1, p2, mask, sum, offset);
  GetChar();
  Print("Vertikal und horizontal gefiltert\n");
  LSIVImg(p1, p2, mask, sum, offset);
  LSIHImg(p2, p2, mask, sum, offset);
  GetChar();
  return 0;
}

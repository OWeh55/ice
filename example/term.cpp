#include <image.h>

int main(int arc, char* argv[])
{
  int c = 'A';
  OpenAlpha("Terminalartiger Test");
  Alpha(ON);
  ClearAlpha();
  Print("Das ist ein einfacher Test der Console\n");
  Print("Er funktioniert wie eine Schreibmaschine.\n");
  Print("Man kann ihn durch Drücken von 'Q' abbrechen.\n\n");

  while (c != 'Q')
    {
      c = GetChar();
      cout << c << endl;
      PutChar(c);
    }

  return OK;
}

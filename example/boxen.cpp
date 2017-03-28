#include <image.h>

using namespace ice;

char banner[] = "Das ist ein Box-Test. ";

int main(int argc, char* argv[])
{
  int i, j, l;
  OpenAlpha("Box-Test");

  do
    {
      SetAttribute(C_BLACK, C_CYAN, 0, 0);
      ClearAlpha();
      l = strlen(banner);

      for (i = 0; i < AlphaSizeY(); i++)
        {
          SetAlphaCursor(0, i);

          for (j = 0; j < AlphaSizeX() - 1; j++)
            Printf("%c", banner[(j + i + i) % l]);
        }

      PushAlpha();
      SetAttribute(C_WHITE, C_BLUE, 0, 0);
      MessageBox("ICE enthaelt Message-Boxen!", B_LEFT | B_TOP);
      Enter("Dies ist eine Quittungs-Box.\nBitte mit <ENTER> bestätigen!");
      PopAlpha();
      PushAlpha();
      SetAttribute(C_WHITE, C_BLUE, 0, 0);
      MessageBox("Boxen in allen Ecken", B_LEFT | B_TOP);
      MessageBox("Boxen in allen Ecken", B_RIGHT | B_TOP);
      MessageBox("Boxen in allen Ecken", B_LEFT | B_BOTTOM);
      MessageBox("Boxen in allen Ecken", B_RIGHT | B_BOTTOM);
      Enter("Schon alles gesehen ?\nBitte mit <ENTER> bestätigen!");
      PopAlpha();
      PushAlpha();
      SetAttribute(C_WHITE, C_BLUE, 0, 0);
      MessageBox("Von links bis rechts", B_LEFT | B_RIGHT | B_TOP);
      MessageBox("Von links bis rechts", B_LEFT | B_RIGHT | B_CENTER);
      MessageBox("Von links bis rechts", B_LEFT | B_RIGHT | B_BOTTOM);
      Enter("Schon alles gesehen ?\nBitte mit <ENTER> bestätigen!");
      PopAlpha();
      PushAlpha();
      SetAttribute(C_WHITE, C_BLUE, 0, 0);
      MessageBox("Von oben bis unten", B_LEFT | B_TOP | B_BOTTOM);
      MessageBox("Von oben bis unten", B_CENTER | B_TOP | B_BOTTOM);
      MessageBox("Von oben bis unten", B_RIGHT | B_TOP | B_BOTTOM);
      Enter("Schon alles gesehen ?\nBitte mit <ENTER> bestätigen!");
      PopAlpha();
      PushAlpha();
      SetAttribute(C_YELLOW, C_RED, 0, 1);
      MessageBox("Auch für Fehlermeldungen !!", B_CENTER);
      GetChar();
      PopAlpha();

    }
  while (!Ask("Abort", "jn"));

  return 0;
}

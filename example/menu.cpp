#include <stdio.h>
#include <wchar.h>
#include <image.h>
/*
#include <menu.h>
#include <terminal.h>
#include <dirfunc.h>
#include <screen.h>
*/

int main(int argc, char* argv[])
{
  int x;
  vector<string> men;
  OpenAlpha("Menü-Test");
  /*
  SetTextAttributes(TextAttribs::FgColorWhite);
  SetTextAttributes(TextAttribs::BgColorBlue);
  */
  ClearAlpha();

  // Bildschirm mit Text füllen

  SetAlphaCursor(0, 3);

  for (int i = 0; i < 15; i++)
    Print("Das ist die "
          + NumberString(i + 3)
          + ". Zeile des alphanumerischen Bildschirms\n");

  SetAlphaCursor(0, 0);
  Print("\020Test für einfache Menufunktion\n");
  men.push_back("Eins");
  men.push_back("Z~wei");
  men.push_back("~gruen");
  men.push_back("g~elb");
  men.push_back("#R~rot");
  men.push_back("#B~blau");
  men.push_back("#I~gruen");
  men.push_back("#Ig~elb");
  men.push_back("#I~rot");
  men.push_back("#I~blau");
  men.push_back("Sehr langer Menüpunkt");
  x = Menu(men, 5, 5, 24, 18, true);

  SetAlphaCursor(0, 20);
  Printf("Ausgewaehlt: %d          \n", x);
  GetChar();

  SetAlphaCursor(0, 0);
  Print("\020Test für langes Menue (DIR-Funktion) \n");
  men.clear();
  Directory(men, "*.cpp");

  if (!men.empty())
    {
      x = Menu(men, 45, 5, 60, 18, true);
      SetAlphaCursor(0, 20);

      if (x >= 0)
        {
          Printf("Ausgewaehlt: %d \n", x);
          Print("Filename " + men[x]);
        }
      else
        Print("Abgebrochen");

      GetChar();
    }

  SetAlphaCursor(0, 0);
  Print("\020Test fuer einfache Dateiauswahl \n");
  SetAlphaCursor(0, 22);
  string filename;
  string path;
  SelFile("*.cpp", filename, path, DIR_SELPATH | DIR_FILE, "Bitte Datei wählen", 3, 3, 65, 22);
  Print("Filesel:" + path + "/" + filename + "\n");

  GetChar();
  return 0;
}

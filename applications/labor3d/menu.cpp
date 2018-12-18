#include <image.h>

#include "menu.h"

const char *qmenu[] = { "b0bin-Darstellung ein/aus",
                        "l0Binarisierungspegel festlegen",
                        "c0Kursor ein/aus",
                        "C0Farbbilddarstellung ein/aus",
                        "X0X erhöhen",
                        "x0X erniedrigen",
                        "Y0Y erhöhen",
                        "y0Y erniedrigen",
                        "Z0Z erhöhen",
                        "z0Z erniedrigen",
                        "+0Binarisierungspegel erhöhen",
                        "-0Binarisierungspegel erniedrigen",
                        "r0Bild als Referenz festlegen",
                        "d0Delete Image",
                        "i0Einlesen eines 3d-Bildes",
                        "q0Quit",

                        "b1Binärbild erzeugen",
                        "i1Invertiertes Bild",
                        "k1DoB-Filter",
                        "n1Normalisierung",
                        "s1Differenzbild",
                        "L1Lokale Segmentierung",
                        "S1Mittelwert-Filterung",
                        "m1Maximum zweier Bilder",
                        "h1Lokale Maxima",

                        "f2Fouriertransformation",
                        "s2Verschobenes Bild",
                        "d2Deconvolution",
                        "b2Deconvolution-Parameter",
                        "p2SDR-Parameter festlegen",
                        "o2Verschiebungsbild (SDR)",
                        "S2Lokale Verschiebungsdetektion",

                        "t3Tiefenbild als Gebirge",
                        "T3Einzelobjekt als Gebirge",
                        "o3Fremde Objekte löschen",
                        "m3Marching cubes + Darstellung",

                        "k4Konfokal-Simulation",
                        "r4Rekonstruktion aus Konfokal",

                        "" // Ende-Kennung Leerstring
                      };

class QMenuItem
{
public:
  char key;
  unsigned char submenu;
  string title;
  QMenuItem(const char *s): key(s[0]), submenu(s[1] - '0'), title(s + 2) {}
  void print() const
  {
    cout << key << " " << (int)submenu << ": " << title << endl;
  }
};

vector<QMenuItem> QMenu;

void mkQMenu()
{
  int i = 0;
  while (strlen(qmenu[i]) > 0)
    {
      //      cout << qmenu[i] << endl;
      QMenu.push_back(QMenuItem(qmenu[i]));
      i++;
    }
}

void mkQMenu(int idx, vector<string> &items, vector<char> &key)
{
  items.clear();
  key.clear();
  for (int i = 0; i < (int)QMenu.size(); i++)
    {
      //      QMenu[i].print();
      if (QMenu[i].submenu == idx)
        {
          items.push_back(QMenu[i].title);
          key.push_back(QMenu[i].key);
        }
    }
}

void DrawQMenu(const vector<string> &items,
               const vector<char> &key)
{
  // 2,2,37,20,    41,2,77,20
  SetAttribute(C_YELLOW, C_RED, 0, 0);
  Box(2, 4, 38, 20, true);
  Box(41, 4, 77, 20, true);
  int s2 = (items.size() + 1) / 2;
  for (int i = 0; i < s2; i++)
    {
      int x = 4;
      int y = i + 5;
      SetAlphaCursor(x, y);
      SetAttribute(C_YELLOW, C_RED, 0, 1);
      Print(string("") + key[i]);
      x += 2;
      SetAlphaCursor(x, y);
      SetAttribute(C_YELLOW, C_RED, 0, 0);
      Print(items[i]);
    }
  for (int i = s2; i < (int)items.size(); i++)
    {
      int x = 43;
      int y = i - s2 + 5;
      SetAlphaCursor(x, y);
      SetAttribute(C_YELLOW, C_RED, 0, 1);
      Print(string("") + key[i]);
      x += 2;
      SetAlphaCursor(x, y);
      SetAttribute(C_YELLOW, C_RED, 0, 0);
      Print(items[i]);
    }
  SetAlphaCursor(3, 19);
  Print("0 - globale Tasten");
  SetAlphaCursor(43, 19);
  Print("1, .., 9 - Wechsel des Menüs");
  SetAttribute(C_WHITE, C_BLUE, 0, 0);
  SetAlphaCursor(0, 23);
}

/**************************************************************/
/* Testprogramm zur Anforderung eines Bildes, Bildpunktsetzen */
/* und Bildpunktlesen                                         */
/* TEST1.C                                                    */
/**************************************************************/

#include <image.h>
#include <stdio.h>

#define sx 2500
#define sy 2560

int main(int argc, char* argv[])
{
  Image ptr;
  ptr.create(sx, sy, 255);

  cout << "Bild mit Testmuster beschreiben" << endl;

  for (int y = 0; y < sy; y++)
    for (int x = 0; x < sx; x++)
      ptr.setPixel(x, y, (x + y) & 255);

  cout << "Vergleichen mit Vorgabe" << endl;

  int ct = 0;

  WindowWalker ww(ptr);
  for (ww.init(); !ww.ready(); ww.next())
    if (ptr.getPixel(ww) != ((ww.x + ww.y) & 255))
      {
        cout << "Fehler an Position " << ww.x << "," << ww.y << " ";
        cout << ptr.getPixel(ww) << " != " << ((ww.x + ww.y) & 255);
        ct++;
      }

  if (ct == 0)
    cout << "Vergleich OK" << endl;
  ptr.destroy();

  return 0;
}

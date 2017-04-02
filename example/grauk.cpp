/* Erzeugung eines Graukeiles und Binarisierung */
#include <stdio.h>
#include <image.h>              /* ICE-Headerfile */

int main(int argc, char* argv[])
{
#define SX 200                /* Bildgroesse in x-Richtung */
#define SY 200                /* Bildgroesse in y-Richtung */
#define MAXV 255              /* maximal einzutragender Datenwert */
  Image img;                    /* Deklaration eines Bildes */
  int x, y, val, thr = 100;
  OpenAlpha("ICE Text Server"); /*Textfenster öffen */
  SetAttribute(0, 7, 0, 0);     /* Attribute setzen */
  ClearAlpha();                 /* initialisieren */
  img = NewImg(SX, SY, MAXV);   /* Bild anfordern */
  Show(ON, img);                /* Bild zur Darstellung anmelden */
  Display(ON);                  /* Aktivierung der Grafik */
  Printf("Graukeil\n");         /* Ausgabe in Alphanumerikfenster */

  for (x = 0; x < img->xsize; x++) /* Graukeil erzeugen */
    for (y = 0; y < img->ysize; y++)
      PutVal(img, x, y, x);     /* Grauwert schreiben */

  GetChar();                    /* Zeicheneingabe in Text- oder Bildfenster abwarten */
  Printf("Binarisierung mit Schwellwert %d\n", thr);
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      {
        val = GetVal(img, x, y);    /* Grauwert lesen */

        if (val < thr) PutVal(img, x, y, 0);
        else PutVal(img, x, y, img->maxval);
      }
  GetChar();
  FreeImg(img);                 /* Freigabe des Bildes */
  return 0;
}

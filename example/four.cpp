/**************************************************************/
/* Test Fouriertransformation                                 */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <image.h>
#include <string.h>

#define GETCHAR() GetChar()

Image p1, p2; // Bild im Ortsraum
Image p3, p4; // Bild im Frequenzraum
ImageD d1, d2;

void toimag()
{
  CopyImg(p1, p2);
  p1.set(p1->maxval / 2);
}

void toreal()
{
  CopyImg(p2, p1);
  p2.set(p2->maxval / 2);
}

void todouble()
{
  ConvImgImgD(p1, d1);
  ConvImgImgD(p2, d2);
}

void toint()
{
  ConvImgDImg(d1, p3, ADAPTIVE);
  ConvImgDImg(d2, p4, ADAPTIVE);
}

int main(int argc, char* argv[])
{
  Image p0;// Originalbild

  int dx = 1024, dy = 1024;
  string filename = "test_rgb.jpg";

  int rc;

  while ((rc = getopt(argc, argv, "x:y:f:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          dx = atol(optarg);
          break;
        case 'y':
          dy = atol(optarg);
          break;
        case 'f':
          filename = optarg;
          break;
        }
    }

  ClearAlpha();
  Print(filename + "\n");

  p0 = ReadImg(filename);

  Printf("Originale Bildgröße %d %d\n", p0->xsize, p0->ysize);
  Printf("Bildgroesse fuer Fouriertransformation: %d %d\n", dx, dy);

  Printf("Konvertiere Original\n");
  p1 = NewImg(dx, dy, 255);
  renormImg(p0, p1);

  Printf("Lege Imaginärteil an\n");
  p2 = NewImg(dx, dy, 255);
  p2.set(p2->maxval / 2);

  p3 = NewImg(dx, dy, 255);
  p4 = NewImg(dx, dy, 255);

  d1 = NewImgD(dx, dy);
  d2 = NewImgD(dx, dy);

  // CopyImg(p0,p1);
  Show(ON, p1, "Original, real");
  Show(ON, p2, "Original, imag");
  Show(ON, p3, "transformiert, Realteil");
  Show(ON, p4, "transformiert, Imaginaerteil");

  Printf("FT\n");
  todouble();
  FourierImgD(d1, d2, NORMAL, d1, d2);
  //  PutValD(d1,(dx+1)/2,(dy+1)/2,0.0);
  //  PutValD(d2,(dx+1)/2,(dy+1)/2,0.0);
  toint();
  GETCHAR();

  Printf("FT (gegebenes Bild als Imaginärteil)\n");
  toimag();
  todouble();
  FourierImgD(d1, d2, NORMAL, d1, d2);
  toint();
  GETCHAR();

  Printf("FT und inverse FT\n");
  toreal();
  todouble();
  FourierImgD(d1, d2, NORMAL, d1, d2);
  FourierImgD(d1, d2, INVERS, d1, d2);
  toint();
  GETCHAR();

  Printf("FT und inverse FT mit Imaginaer-Teil\n");
  toimag();
  todouble();
  FourierImgD(d1, d2, NORMAL, d1, d2);
  FourierImgD(d1, d2, INVERS, d1, d2);
  toint();
  GETCHAR();

  Printf("zweimal FT\n");
  toreal();
  todouble();
  FourierImgD(d1, d2, NORMAL, d1, d2);
  FourierImgD(d1, d2, NORMAL, d1, d2);
  toint();
  GETCHAR();

  Printf("logarithmiertes Amplitudenspektrum\n");
  todouble();
  PowerSpectrumImgD(d1, d1, MD_LOG);
  toint();
  GETCHAR();

  Printf("Cepstrum\n");
  todouble();
  CepstrumImgD(d1, d1);
//  LogImgD(d1,d1);
  toint();
  GETCHAR();

  Printf("Amplituden/Phasen-Spektrum\n");
  ConvImgImgD(p1, d1);
  MPSpectrumImgD(d1, d1, d2);
  toint();
  GETCHAR();

  return 0;
}

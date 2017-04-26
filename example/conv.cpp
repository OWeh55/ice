// test der Faltung von ICE
#include <image.h>
//#include <conv.h>

#define SIZE 512
#define NOISE 50

#define DX 22
#define DY -37

#define BETA1 0.001
#define BETA2 1.0

int main(int arc, char* argv[])
{
  Image i1, i2, i3, i4, i5, i6;

  i1 = NewImg(SIZE, SIZE, 255);
  i2 = NewImg(SIZE, SIZE, 255);
  i3 = NewImg(SIZE, SIZE, 255);
  i4 = NewImg(SIZE, SIZE, 255);
  i5 = NewImg(SIZE, SIZE, 255);
  i6 = NewImg(SIZE, SIZE, 255);

  Alpha(ON);
  ClearAlpha();
  Printf("Erzeugen und Darstellen zweier Testbilder\n");
  Show(ON, i1, "Bild 1");
  Show(ON, i2, "Bild 2");
  Show(ON, i3, string("beta=") + to_string(BETA1));
  Show(ON, i4, string("beta=") + to_string(BETA2));
  Show(ON, i5, "phasecorrelation");
  Show(ON, i6, "crosscorrelation");
#if 1
  setImg(i1, 128);
  setImg(i2, 128);
  // unabhängiges Untergrund-Rauschen
  WindowWalker ww(i1);
  for (ww.init(); !ww.ready(); ww.next())
    {
      double g = Random(NOISE) - NOISE / 2 + 128;
      i1.setPixel(ww, g);
      g = Random(NOISE) - NOISE / 2 + 128;
      i2.setPixel(ww, g);
    }
  // Bild-"Inhalt" : Verschobener Text
  Text("Text", 25, 55, 128 + 64, 2, i1);
  Text("Text", 25 + DX, 55 + DY, 128 + 64, 2, i2);
#else
  Image orig;
  orig.read("test_gray.jpg");

  int x1 = 200, y1 = 0;
  int x2 = x1 - DX, y2 = y1 - DY;

  CopyImg(orig(Window(x1, y1, x1 + SIZE - 1, y1 + SIZE - 1)), i1);
  CopyImg(orig(Window(x2, y2, x2 + SIZE - 1, y2 + SIZE - 1)), i2);

  WindowWalker w(i1);
  for (w.init(); !w.ready(); w.next())
    {
      int val = i1.getPixel(w);
      val = (val * (100 - NOISE) + Random(NOISE)) / 100;
      i1.setPixel(w, val);
      val = i2.getPixel(w);
      val = (val * (100 - NOISE) + Random(NOISE)) / 100;
      i2.setPixel(w, val);
    }

#endif

  // SDR mit zwei verschiedenen betas
  Printf("Verschiebungsbestimmung\n");
  InvConvolutionImg(i1, i2, i3, 0.0, BETA1, MD_IGNORE_BIAS);
  InvConvolutionImg(i1, i2, i4, 0.0, BETA2, MD_IGNORE_BIAS);
  // phase correlation
  PhaseCorrelationImg(i1, i2, i5);
  // approximation of cross correlation
  InvConvolutionImg(i1, i2, i6, 0.0, 1e102, MD_IGNORE_BIAS);
  double xf, yf;
  Printf("Verschiebung ist: %i, %i\n", (int)DX, (int)DY);
  double val = PeakValuation(i3, Image(), xf, yf);
  Printf("Ermittelt (beta=%f): %f,%f (%g)\n",
         BETA1, xf - SIZE / 2, yf - SIZE / 2, val);
  val = PeakValuation(i4, Image(), xf, yf);
  Printf("Ermittelt (beta=%f): %f,%f (%g)\n",
         BETA2, xf - SIZE / 2, yf - SIZE / 2, val);
  val = PeakValuation(i5, Image(), xf, yf);
  Printf("Ermittelt (PhaseCorrelation): %f,%f (%g)\n",
         xf - SIZE / 2, yf - SIZE / 2, val);
  val = PeakValuation(i6, Image(), xf, yf);
  Printf("Ermittelt (CrossCorrelation): %f,%f (%g)\n",
         xf - SIZE / 2, yf - SIZE / 2, val);
//  int p[2];
//  SelPoint(1,i3,p);
  GetChar();
  return 0;
}

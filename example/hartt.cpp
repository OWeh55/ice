#include <image.h>

static int bcount = 0;

void ShowImg(ImageD d, Image i)
{
  ConvImgDImg(d, i);
}

void ShowLog(ImageD d, Image i)
{
  ImageD log = NewImgD(d);
  LogImgD(d, log);
  ShowImg(log, i);
  FreeImgD(log);
}

void ShowDiff(ImageD d1, ImageD d2, Image i)
{
  int x, y;
  ImageD diff;
  diff.create(d1.xsize, d1.ysize);
  for (int y = 0; y < diff.ysize; y++)
    for (int x = 0; x < diff.xsize; x++)
      PutValD(diff, x, y, GetValD(d1, x, y) - GetValD(d2, x, y));
  ShowImg(diff, i);
}

void PrintRange(string p, ImageD d)
{
  UpdateLimitImgD(d);
  Printf((p + ": %g .. %g\n").c_str(), d.minval, d.maxval);
}

void PrintDiffRange(string p, ImageD d1, ImageD d2)
{
  int x, y;
  ImageD diff = NewImgD(d1.xsize, d1.ysize);
  for (int y = 0; y < diff.ysize; y++)
    for (int x = 0; x < diff.xsize; x++)
      PutValD(diff, x, y, GetValD(d1, x, y) - GetValD(d2, x, y));
  UpdateLimitImgD(diff);
  Printf((p + ": %g .. %g\n").c_str(), diff.minval, diff.maxval);
  FreeImgD(diff);
}

void Enter(void)
{
  Printf("Press Enter");

  if (bcount > 0) bcount--;
  else GetChar();

  PutChar(0x0d);
  PutChar(16);
}

#define imgsize 512

int main(int argc, char* argv[])
{
  Image links, mitte, rechts;
  //  ImageD d1,d2,d3,d4,d5,d6,d7,d8, null;
  ImageD original, maske, resultat1, resultat2, s1, s2, s3, s4, s5, s6, h, null;
  int x, y;

  ClearAlpha();

  links = NewImg(imgsize, imgsize, 255);
  mitte = NewImg(imgsize, imgsize, 255);
  rechts = NewImg(imgsize, imgsize, 255);

  original = NewImgD(imgsize, imgsize, 0, 255);
  maske = NewImgD(imgsize, imgsize, 0, 255);
  resultat1 = NewImgD(imgsize, imgsize, 0, 255);
  resultat2 = NewImgD(imgsize, imgsize, 0, 255);
  s1 = NewImgD(imgsize, imgsize, 0, 255);
  s2 = NewImgD(imgsize, imgsize, 0, 255);
  s3 = NewImgD(imgsize, imgsize, 0, 255);
  s4 = NewImgD(imgsize, imgsize, 0, 255);
  s5 = NewImgD(imgsize, imgsize, 0, 255);
  s6 = NewImgD(imgsize, imgsize, 0, 255);
  h = NewImgD(imgsize, imgsize, 0, 255);

  null = NewImgD(imgsize, imgsize, 0, 255);
  SetImgD(null, 0);

  links = NewImg(imgsize, imgsize, 255);

  if (argc > 1)
    ReadImg(argv[1], links);
  else
    ReadImg("test_rgb.jpg", links);

  Display(ON);
  Show(ON, links);
  Show(ON, mitte);
  Show(ON, rechts);

  Printf("Hartley-Trafo zweidimensional\n");
  ConvImgImgD(links, original);
  PrintRange("Original", original);
  HartleyImgD(original, s1);
  ShowImg(s1, mitte);
  HartleyImgD(s1, resultat1);
  PrintRange("Rücktransformiert", resultat1);
  ShowImg(resultat1, rechts);
  Enter();

  Printf("Hartley-Trafo nur in X-Richtung\n");
  HartleyImgD(original, s1, X_ONLY);
  ShowImg(s1, mitte);
  HartleyImgD(s1, resultat1, X_ONLY);
  PrintRange("Rücktransformiert", resultat1);
  ShowImg(resultat1, rechts);
  Enter();

  Printf("Hartley-Trafo nur in Y-Richtung\n");
  HartleyImgD(original, s1, Y_ONLY);
  ShowImg(s1, mitte);
  HartleyImgD(s1, resultat1, Y_ONLY);
  PrintRange("Rücktransformiert", resultat1);
  ShowImg(resultat1, rechts);
  Enter();

  Printf("Vergleich: \n - Hartley direkt\n - Hartley als Differenz Real- und Imaginärteil der Fourier-Trafo\n");

  HartleyImgD(original, s1);
  PrintRange("Hartley", s1);
  ShowImg(s1, mitte);
  FourierImgD(original, null, NORMAL, s2, s3);
  wloop(s2, x, y) PutValD(s2, x, y, GetValD(s2, x, y) - GetValD(s3, x, y));
  PrintRange("\"Fourier\"", s2);
  ShowImg(s2, rechts);
  Enter();
  ShowDiff(s1, s2, mitte);
  PrintDiffRange("Hartley-Fourier", s1, s2);
  Enter();

  Printf("(logarithmiertes) Leistungs-Spektrum über Hartley und Fourier im Vergleich\n");
  HartleyImgD(original, s1);
  PowerSpectrumHImgD(s1, resultat1, MD_LOG);
  PrintRange("Hartley", resultat1);
  ShowImg(resultat1, mitte);
  FourierImgD(original, null, NORMAL, s1, s2);
  PowerSpectrumFImgD(s1, s2, resultat2, MD_LOG);
  ShowImg(resultat2, rechts);
  PrintRange("Fourier", resultat2);
  Enter();
  ShowDiff(resultat1, resultat2, mitte);
  PrintDiffRange("Hartley - Fourier", resultat1, resultat2);
  Enter();

  //  PowerSpectrumImgD(original,resultat1);
  //  ShowImg(resultat1,mitte);
  //  Enter();

  Printf("Betrags/Phasen-Darstellung\n");
  FourierImgD(original, null, NORMAL, s1, s2);
  MPSpectrumFImgD(s1, s2, s1, s2);
  ShowImg(s1, mitte);
  ShowImg(s2, rechts);
  Printf("Betrag und Phase aus dem Fourier-Spektrum\n");
  Enter();
  HartleyImgD(original, s3);
  MPSpectrumHImgD(s3, s3, s4);
  ShowImg(s3, mitte);
  ShowImg(s4, rechts);
  Printf("Betrag und Phase aus dem Hartley-Spektrum\n");
  Enter();
  Printf("Differenz von Betrag und Phase aus Fourier und Hartley-Spektrum\n");
  ShowDiff(s1, s3, mitte);
  PrintDiffRange("Fourier-Hartley (Betrag)", s1, s3);
  ShowDiff(s2, s4, rechts);
  PrintDiffRange("Fourier-Hartley (Phase)", s2, s4);
  Enter();

  Printf("Bild -> Fourier -> MP -> Hartley -> Bild\n");
  FourierImgD(original, null, NORMAL, s1, s2);
  MPSpectrumFImgD(s1, s2, s3, s4);
  HartleyMPImgD(s3, s4, s1);
  HartleyImgD(s1, resultat1);
  ShowImg(resultat1, mitte);
  ShowDiff(original, resultat1, rechts);
  PrintDiffRange("Original-Rücktransformiert", original, resultat1);
  Enter();

  Printf("Bild -> Hartley -> MP -> Fourier -> Bild\n");
  HartleyImgD(original, s1);
  MPSpectrumHImgD(s1, s1, s2);
  FourierMPImgD(s1, s2, s3, s4);
  FourierImgD(s3, s4, INVERS, resultat1, resultat2);
  ShowImg(resultat1, mitte);
  ShowDiff(original, resultat1, rechts);
  PrintDiffRange("Original-Rücktransformiert", original, resultat1);
  Enter();

  Printf("Faltung: Hartley/Fourier\n");
  HartleyImgD(original, s1);
  SetImgD(maske, 0);
  PutValD(maske, imgsize / 2, imgsize / 2, 1); //Faltungsmaske
  PutValD(maske, imgsize / 2 + 10, imgsize / 2 + 20, -0.9);
  PutValD(maske, imgsize / 2 - 20, imgsize / 2 + 15, 0.8);
  PutValD(maske, imgsize / 2 + 15, imgsize / 2 - 20, 0.9);
  PutValD(maske, imgsize / 2 - 25, imgsize / 2 - 15, -0.8);
  HartleyImgD(maske, s2);
  ConvolutionHImgD(s1, s2, s3);
  HartleyImgD(s3, resultat1);
  ShowImg(maske, mitte);
  ShowImg(resultat1, rechts);
  Printf("Hartley\n");
  Enter();
  FourierImgD(original, null, NORMAL, s1, s2);
  FourierImgD(maske, null, NORMAL, s3, s4);
  ConvolutionFImgD(s1, s2, s3, s4, s3, s4);
  FourierImgD(s3, s4, INVERS, resultat2, s4);
  ShowImg(resultat2, rechts);
  Printf("Fourier\n");
  Enter();
  ShowDiff(resultat1, resultat2, mitte);
  PrintDiffRange("Differenz Hartley-Fourier", resultat1, resultat2);
  Enter();

  Printf("Inverse Faltung (Hartley)\n");
  HartleyImgD(original, s1);
  HartleyImgD(maske, s2);
  ConvolutionHImgD(s1, s2, s3);
  HartleyImgD(s3, resultat1);
  ShowImg(resultat1, mitte);
#ifdef MASKE
  InvConvolutionHImgD(s1, s3, s4, 0.0001);
#else
  InvConvolutionHImgD(s2, s3, s4, 0.0001);
#endif
  HartleyImgD(s4, resultat2);
  //  PutValD(resultat2,imgsize/2,imgsize/2,0);
  ShowImg(resultat2, rechts);
  Enter();
  Printf("Inverse Faltung (Fourier)\n");
  FourierImgD(original, null, NORMAL, s1, s2);
  FourierImgD(maske, null, NORMAL, s3, s4);
  ConvolutionFImgD(s1, s2, s3, s4, s5, s6);
  FourierImgD(s5, s6, INVERS, resultat1, h);
  ShowImg(resultat1, mitte);
#ifdef MASKE
  InvConvolutionFImgD(s1, s2, s5, s6, s1, s2, 0.0001);
#else
  InvConvolutionFImgD(s3, s4, s5, s6, s1, s2, 0.0001);
#endif
  FourierImgD(s1, s2, INVERS, resultat2, h);
  //  PutValD(resultat2,imgsize/2,imgsize/2,0);
  ShowImg(resultat2, rechts);
  Enter();

  Printf("CrossCorrelation (Hartley)\n");
  HartleyImgD(original, s1);
  SetImgD(maske, 0);
  PutValD(maske, imgsize / 2 - 50, imgsize / 2 - 30, 1); //Faltungsmaske
  PutValD(maske, imgsize / 2 - 30, imgsize / 2 - 50, 1);
  HartleyImgD(maske, s2);
  ConvolutionHImgD(s1, s2, s3);
  HartleyImgD(s3, h); // gefaltetes Bild als 2.Bild für Kreuz-Korr.
  ShowImg(h, mitte);
  CrossCorrelationHImgD(s1, s3, s4);
  HartleyImgD(s4, resultat1);
  ShowImg(resultat1, rechts);
  Enter();

  Printf("CrossCorrelation (Fourier)\n");
  FourierImgD(original, null, NORMAL, s1, s2);
  FourierImgD(h, null, NORMAL, s3, s4);
  CrossCorrelationFImgD(s1, s2, s3, s4, s5, s6);
  FourierImgD(s5, s6, INVERS, resultat2, s6);
  ShowImg(resultat2, rechts);
  Enter();

  ShowDiff(resultat1, resultat2, mitte);
  PrintDiffRange("Hartley-Fourier", resultat1, resultat2);
  Enter();

  Printf("CrossCorrelation\n");
  CrossCorrelationImgD(original, h, resultat1);
  ShowImg(resultat1, mitte);
  Enter();

  Printf("Whitening : direkt=Hartley, Fourier\n");
  WhiteningImgD(original, resultat1, 1);
  ShowImg(resultat1, mitte);
  mitte.write("lennaWhite.jpg");
  FourierImgD(original, null, NORMAL, s1, s2);
  WhiteningFImgD(s1, s2, s3, s4, 1);
  FourierImgD(s3, s4, INVERS, resultat2, s6);
  ShowImg(resultat2, rechts);
  Enter();
  ShowDiff(resultat1, resultat2, mitte);
  PrintDiffRange("Hartley-Fourier", resultat1, resultat2);
  Enter();
  return 0;
}

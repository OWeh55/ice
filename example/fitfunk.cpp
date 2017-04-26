// test for fitting functions
#include <image.h>
#include <fitfn.h>

double myRand(double noise)
{
  return (RandomD() - 0.5) * noise;
}

int main(int argc, char* argv[])
{
  double a, b, c, d, e, f;
  double ar, br, cr, dr, er, fr;
  double noise = 0;

  if (argc > 1)
    {
      noise = atof(argv[1]);
      double time = fmod(TimeD(), 1.0);
//    cout << time << endl;
      int seed = (int)(time * 1000);
//    cout << seed<< endl;
      srand(seed);
    }

  Matrix m;
  int i, j;
  Alpha(ON);
  ClearAlpha();
  Printf("Fitting-Test\n");
  Printf("Test eindimensionaler Funktionen (einzelne Werte)\n");
  a = 2.7;
  b = -5;
  Printf("Vorgabe:  %g * x + %g = y\n", a, b);
  Polynom1o f1;
  f1.FitInit();

  for (i = 0; i < 99; i++)
    f1.FitVal(i, a * i + b + myRand(noise));

  f1.FitFinish();

  f1.getCoefficient(ar, br);
  Printf("Ergebnis: %g * x + %g = y\n", br, ar);
  Printf("Varianz: %6.2f\n", f1.Variance());

  a = 2.7;
  b = -5;
  c = 17;
  Printf("Vorgabe:  %g * x^2 + %g * x + %g = y\n", a, b, c);
  Polynom2o f2;
  f2.FitInit();

  for (i = 0; i < 99; i++)
    f2.FitVal(i, a * i * i + b * i + c + myRand(noise));
  f2.FitFinish();

  f2.getCoefficient(ar, br, cr);
  Printf("Ergebnis: %g * x^2 + %g * x + %g = y\n", cr, br, ar);
  Printf("Varianz: %6.2f\n", f2.Variance());

  Printf("Test eindimensionaler Funktionen (Liste)\n");
  a = 0.72;
  b = 5;
  Printf("Vorgabe:  %g * x + %g = y\n", a, b);
  Polynom1o f1a;
  m = Matrix(0, 2);

  for (i = 0; i < 99; i++)
    m = m && Vector(i, a * i + b + myRand(noise));

  f1a.Fit(m);

  f1a.getCoefficient(ar, br);
  Printf("Ergebnis: %g * x + %g = y\n", br, ar);
  Printf("Varianz: %6.2f\n", f1a.Variance());

  a = 2.7;
  b = -5;
  c = 17;
  Printf("Vorgabe:  %g * x^2 + %g * x + %g = y\n", a, b, c);
  Polynom2o f2a;
  m = Matrix(0, 2);

  for (i = 0; i < 99; i++)
    m = m && Vector(i, a * i * i + b * i + c + myRand(noise));

  f2a.Fit(m);
  f2a.getCoefficient(ar, br, cr);
  Printf("Ergebnis: %g * x^2 + %g * x + %g = y\n", cr, br, ar);
  Printf("Varianz: %6.2f\n", f2a.Variance());

  Printf("Test zweidimensionaler Funktionen\n");
  a = 2.7;
  b = -5;
  c = 4;
  Printf("Vorgabe:  %g * x + %g * y + %g = y\n", a, b, c);
  Polynom2d1o f3;
  f3.FitInit();

  for (i = 0; i < 9; i++)
    for (j = 0; j < 9; j++)
      f3.FitVal(i, j, a * i + b * j + c + myRand(noise));

  f3.FitFinish();

  f3.getCoefficient(ar, br, cr);
  Printf("Ergebnis: %g * x + %g * y + %g = y\n", br, cr, ar);
  Printf("Varianz: %6.2f\n", f3.Variance());

  a = 2.7;
  b = -5;
  c = 17;
  d = 1.2;
  e = -0.7;
  f = 0.1;
  Printf("Vorgabe:  %g*x^2+%g*x*y+%g*y^2+%g*x+%g*y+%g=y\n", a, b, c, d, e, f);
  Polynom2d2o f4;
  f4.FitInit();

  for (i = 0; i < 9; i++)
    for (j = 0; j < 9; j++)
      f4.FitVal(i, j, a * i * i + b * i * j + c * j * j + d * i + e * j + f + myRand(noise));

  f4.FitFinish();

  f4.getCoefficient(ar, br, cr, dr, er, fr);
  Printf("Ergebnis: %g*x^2+%g*x*y+%g*y^2+%g*x+%g*y+%g=y\n", dr, er, fr, br, cr, ar);
  Printf("Varianz: %6.2f\n", f4.Variance());
  GetChar();

  a = 3;
  b = 4;
  c = 5;
  d = 6;
  Printf("Allgemeines Polynom\n");
  Printf("Vorgabe:  %g*x^3+%g*x^2+%g*x+%g=y\n", a, b, c, d);
  Polynom f5;
  f5.FitInit(3);

  for (i = 0; i < 99; i++)
    f5.FitVal(i, a * i * i * i + b * i * i + c * i + d + myRand(noise));

  f5.FitFinish();

  vector<double> res;
  f5.getCoefficient(res);
  Printf("Ergebnis: %g*x^3+%g*x^2+%g*x+%g=y\n", res[3], res[2], res[1], res[0]);
  Printf("Varianz: %6.2f\n", f5.Variance());
  GetChar();

  Image img;
  img = NewImg(511, 511, 255);
  Display(ON);
  Show(ON, img);

  f4.FitInit();
  f4.FitVal(0, 0, 0);
  f4.FitVal(511, 511, 0);
  f4.FitVal(0, 511, 255);
  f4.FitVal(511, 0, 255);
  f4.FitVal(255, 255, 127);
  f4.FitVal(0, 255, 255);

  f4.FitFinish();

  setImg(img, f4);
  GetChar();

  f3.FitInit();
  f3.FitVal(0, 0, 0);
  f3.FitVal(511, 511, 255);
  f3.FitVal(0, 511, 255);
  f3.FitVal(511, 0, 255);
  f3.FitVal(255, 255, 127);
  f3.FitVal(0, 255, 255);
  f3.FitFinish();

  setImg(img, f3);
  GetChar();
  return 0;
}

#include <image.h>

int main(int argc, char* argv[])
{
  const int nCols=3;
  const int nPoints=nCols*nCols;
  const double noise = 4;

  Matrix p1(nPoints, 2), p2(nPoints, 2), p3(nPoints, 2);
  Vector w(nPoints);

  Trafo tr1(2, 2);
  Trafo tr2, tr3;

  Image img;
  img.create(511, 511, 255);

  double circle[3];

  Display(ON);
  Show(OVERLAY, Image(), img);

  // eine Punktliste vorgeben
  for (int i = 0; i < nPoints; i++)
    {
      p1[i][0] = (i % nCols) * 40 + 50;
      p1[i][1] = (i / nCols) * 40 + 50;
      w[i] = 1;
    }

  // eine transformation konstruieren
  tr1.shift(-255, -255);
  tr1.rotate(0, 0, 0.01);
  tr1.shift(255, 255);

  // die punktliste transformieren
  transformList(tr1, p1, p2);

  // zufällige fehler einbauen
  for (int i = 0; i < nPoints; i++)
    {
      p2[i][0] += (drand48()-0.5) * noise;
    }

  // Transformationen schätzen
  // .. kleinste Beträge
  tr2 = MatchPointlistsLinOpt(p1, p2, TRM_AFFINE, w);
  // .. kleinste Quadrate
  tr3 = MatchPointlists(p1, p2, TRM_AFFINE, w);

  // Matrizen ausgeben
  cout << tr1.getMatrix(); // soll
  cout << tr2.getMatrix(); // ist1
  cout << tr3.getMatrix(); // ist2

  // Punktliste 2 darstellen
  for (int i = 0; i < nPoints; i++)
    {
      circle[0] = p2[i][0];
      circle[1] = p2[i][1];
      circle[2] = 2;
      drawCircle(circle, 255, 255, DEFAULT, img);
    }

  // transformierte Punktlisten darstellen
  transformList(tr2, p1, p3);

  for (int i = 0; i < nPoints; i++)
    {
      circle[0] = p3[i][0];
      circle[1] = p3[i][1];
      circle[2] = 3;
      drawCircle(circle, 1, 1, NOFILL, img);
    }

  transformList(tr3, p1, p3);

  for (int i = 0; i < nPoints; i++)
    {
      circle[0] = p3[i][0];
      circle[1] = p3[i][1];
      circle[2] = 4;
      drawCircle(circle, 2, 2, NOFILL, img);
    }

  GetChar();
  return OK;
}

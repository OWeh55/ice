#include <image.h>

int main(int argc, char* argv[])
{
  Matrix p1(100, 2), p2(100, 2), p3(100, 2);
  Vector w(100);
  Trafo tr1(2, 2);
  Trafo tr2, tr3;

  Image img = NewImg(511, 511, 255);
  double circle[3];

  Display(ON);
  Show(OVERLAY, Image(), img);

  int i;

  // eine Punktliste vorgeben
  for (i = 0; i < 100; i++)
    {
      p1[i][0] = (i % 10) * 40 + 50;
      p1[i][1] = (i / 10) * 40 + 50;
      w[i] = 1;
    }

  // eine transformation konstruieren
  tr1.Shift(-255, -255);
  tr1.Rotate(0, 0, 66);
  tr1.Shift(255, 255);

  // die punktliste transformieren
  TransformList(tr1, p1, p2);

  // ein paar fehler einbauen
  // jeden dritten Punkt verschieben
  for (i = 0; i < 100; i += 3)
    {
      p2[i][0] += Random(16) - 8;
      //      p2[i][1]-=15;
    }

  // Transformationen schätzen
  // .. kleinste Beträge
  tr2 = MatchPointlistsLinOpt(p1, p2, TRM_AFFINE, w);
  // .. kleinste Quadrate
  tr3 = MatchPointlists(p1, p2, TRM_AFFINE, w);

  // Matrizen ausgeben
  cout << tr1.Tmatrix(); // soll
  cout << tr2.Tmatrix(); // ist1
  cout << tr3.Tmatrix(); // ist2

  // Punktliste 2 darstellen
  for (i = 0; i < 100; i++)
    {
      circle[0] = p2[i][0];
      circle[1] = p2[i][1];
      circle[2] = 2;
      drawCircle(circle, 255, 255, DEFAULT, img);
    }

  // transformierte Punktlisten darstellen
  TransformList(tr2, p1, p3);

  for (i = 0; i < 100; i++)
    {
      circle[0] = p3[i][0];
      circle[1] = p3[i][1];
      circle[2] = 3;
      drawCircle(circle, 1, 1, NOFILL, img);
    }

  TransformList(tr3, p1, p3);

  for (i = 0; i < 100; i++)
    {
      circle[0] = p3[i][0];
      circle[1] = p3[i][1];
      circle[2] = 4;
      drawCircle(circle, 2, 2, NOFILL, img);
    }

  GetChar();
  return OK;
}

#include <image.h>

int main(int argc, char* argv [])
{
  Matrix pl1 =
    Vector(0, 0, 0) &&
    Vector(1, 0, 0) &&
    Vector(0, 1, 0) &&
    Vector(0, 0, 1) &&
    Vector(1, 1, 0) &&
    Vector(1, 0, 1) &&
    Vector(0, 1, 1) &&
    Vector(1, 1, 1);

  Matrix pl2 =
    Vector(0, 0, 1) &&
    Vector(1, 0, 1) &&
    Vector(0, 1, 1) &&
    Vector(0, 0, 2) &&
    Vector(1, 1, 1) &&
    Vector(1, 0, 2) &&
    Vector(0, 1, 2) &&
    Vector(1, 1, 2);

  Vector weight = Vector(0) && 1 && 1 && 1 && 1 && 1 && 1 && 1 && 1;

  int i;
  cout.flags(cout.flags() | ios::fixed);
  cout.precision(2);

  Trafo t1(3, 3);
  t1.Rotate(Vector3d(0.5, 0.5, 0.5), Vector3d(0, 1, 0), 90);
  t1.Scale(Vector(0.5, 0.5, 0.5), 0.5);

  for (i = 0; i < pl1.rows(); i++)
    {
      pl2[i] = t1 * pl1[i];
    }

  Trafo tr = MatchPointlists(pl1, pl2, TRM_PROJECTIVE, weight);

  cout << t1.getMatrix();
  cout << tr.getMatrix();

  for (i = 0; i < pl1.rows(); i++)
    {
      cout << (tr * pl1[i]) << " - " << pl2[i] ;
      cout << " = " << (tr * pl1[i] - pl2[i]).Length() << endl;
    }

  GetChar();
  return 0;
  Display(ON);
  ClearAlpha();

  Image i1, i2, i3, m1, m2;
//    double time1;
//    i1=ReadImg("dcp00538.jpg",NULL);
//    //  i1=ReadImg("dcp_2727.jpg",NULL);
//    i2=CopyImg(i1);
//    Show(ON,i1);
//    Show(ON,i2);
//    t1=Trafo(2,2);
//    t1.Rotate(i1->xsize/2,i1->ysize/2,17);

//    //  SetWindowImg(i1,5,5,233,233);
//    //  SetWindowImg(i2,5,5,233,233);

//    time1=TimeD();
//    Transform(t1,i2,i1,DEFAULT);
//    Printf("\n\nZeit: %4.2f\n",TimeD()-time1);
//    time1=TimeD();
//    t1.Invert();
//    Transform(t1,i2,i1,INTERPOL);
//    Printf("\n\nZeit: %4.2f\n",TimeD()-time1);
//    GetChar();

//    FreeImg(i1);
//    FreeImg(i2);

  i1 = ReadImg("test_gray.jpg");
  i2 = ReadImg("test_gray.jpg");

  pl1 = Matrix(0, 2);
  pl2 = Matrix(0, 2);

  for (i = 0; i < 5; i++)
    {
      Printf("Punkt %d:\nLinks\n", i);
      Show(ON, i1);
      pl1 = pl1 && (Vector)SelVector(DEFAULT, i1);
      Show(OFF, i1);
      Show(ON, i2);
      Printf("Rechts\n");
      pl2 = pl2 && (Vector)SelVector(DEFAULT, i2);
      Show(OFF, i2);

    }

  cout << pl1 << pl2 ;

  t1 = MatchPointlists(pl1, pl2, TRM_PROJECTIVE);
  cout << t1.Tmatrix();
  i3 = NewImg(i1);
  CopyImg(i1, i3);
  Show(ON, i3);
  Transform(t1, i1, i3);
  SubImg(i3, i2, 3, i3);
  GetChar();
  Show(OFF, i3);

  m1 = NewImg(i1->xsize, i1->ysize, 255);
  ClearImg(m1);
  m2 = NewImg(i2->xsize, i2->ysize, 255);
  ClearImg(m2);

  Show(OVERLAY, i1, m1);
  Contur c1, c2;
  c1 = SelContur(m1);
  c2 = Transform(t1, c1);
  Show(OFF, i1);
  Show(OVERLAY, i2, m2);
  MarkContur(c2, 3, m2);
  GetChar();
  FreeImg(i1);
  FreeImg(i2);
  FreeImg(m1);
  FreeImg(m2);
  return 0;
}

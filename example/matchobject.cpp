#include <image.h>

int main(int argc, char** argv)
{
  Image img;
  img.create(1000, 800, 255);
  img.set(0);

  Show(ON, img);
#if 0
  Contur c1 = SelContur(img, true);
#else
  Contur c1;
  c1.SetStart(100, 100);
  c1.Add(800, 200);
  c1.Add(700, 777);
  c1.Add(200, 800);
  c1.Close();
#endif
  Matrix tm(3, 3, 1);
  tm[0][0] = 0.9;
  tm[0][1] = 0.2;
  tm[0][2] = 50;
  tm[1][0] = -0.2;
  tm[1][1] = 0.9;
  tm[2][0] = 0.0001;
  tm[2][1] = 0.0001;
  Trafo tro(tm);

  cout << tro.Tmatrix() << endl;
  Contur c2 = Transform(tro, c1);
  // SelContur(img,true);
  MarkContur(c2, 200, img);
  Trafo tr = MatchObject(c1, c2, TRM_PROJECTIVE_NOR);
  //Trafo tr=MatchObject(c1,c2,TRM_AFFINE);
  cout << tr.Tmatrix() << endl;
  Contur c3 = Transform(tr, c1);
  MarkContur(c3, 127, img);
  GetChar();
  return 0;
}

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
  c1.setStart(100, 100);
  c1.add(800, 200);
  c1.add(700, 777);
  c1.add(200, 800);
  c1.close();
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

  cout << tro.getMatrix() << endl;
  Contur c2 = transform(tro, c1);
  // SelContur(img,true);
  MarkContur(c2, 200, img);
  Trafo tr = MatchObject(c1, c2, TRM_PROJECTIVE_NOR);
  //Trafo tr=MatchObject(c1,c2,TRM_AFFINE);
  cout << tr.getMatrix() << endl;
  Contur c3 = transform(tr, c1);
  MarkContur(c3, 127, img);
  GetChar();
  return 0;
}

#ifndef HF_H
#define HF_H

#include <vector>
#include "point.h"

class heightfield: public point
{
public:
  heightfield(const string &namep, vect *posp, vect *dirxp = NULL, vect *diryp = NULL, vect *dirzp = NULL):
    point(posp), dirx(dirxp), diry(diryp), dirz(dirzp), tex(NULL)
  {
    InitHF(namep);
  }

  heightfield(const string &namep, vect *posp, vect *dirxp, vect *diryp, vect *dirzp, texture *texp):
    point(posp), dirx(dirxp), diry(diryp), dirz(dirzp), tex(texp)
  {
    InitHF(namep);
  }
  virtual ~heightfield() {}

private:
  void InitHF(const string &n);
  void CalcV3D() const;
  void Vertex(int x, int y) const;

protected:
  virtual void MyOptimize();
  virtual void myDraw() const;

  std::vector<std::vector<int> > h;
  int xsize, ysize, maxval;

  mutable vector<vector<vector3> > v3d;
  mutable vector<vector<vector3> > n3d;

  vector3 DirX() const
  {
    if (dirx != NULL) return dirx->Value();
    else return vector3(0, 0, 1);
  }

  vector3 DirY() const
  {
    if (diry != NULL) return diry->Value();
    else return vector3(1, 0, 0);
  }

  vector3 DirZ() const
  {
    if (dirz != NULL) return dirz->Value();
    vector3 vz = DirX().cross(DirY());
    vz.Normalize();
    return vz;
  }

  vect *dirx;
  vect *diry;
  vect *dirz;
  texture *tex;
};

#endif

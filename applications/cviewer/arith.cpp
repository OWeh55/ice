#include "cviewer.h"
#include "arith.h"
#include <math.h>


const vector3 &vnum3::Value() const
{
  // Komposition eines Vektors aus 3 Floats
  return temp = vector3(val1->Value(), val2->Value(), val3->Value());
}

const vector3 &spline::Value(float idx) const
{
// berechnung eines Spline-Wertes
  int i0, i1, i2, i3; // indizees der benachbarten Werte
  int s = size();
  while (idx < 0) idx += s; // index in Bereich 0..size bringen
  idx = fmod(idx, s);

  i1 = (int)floor(idx); // linker Nachbar
  float fract = idx - i1;
  i2 = i1 + 1;
  if (i2 == s) i2 = 0; // rechter Nachbar

  switch (typ)
    {
    case 1:
      // lineare Interpolation
      temp = vectlist::Value(i1) * (1 - fract) + vectlist::Value(i2) * fract;
      break;
    case 3:
    {
      // spezielle kubische Interpolation:
      // = lineare Interpolation zwischen zwei quadratischen Funktionen
      i3 = i2 + 1;
      if (i3 == s) i3 = 0;
      i0 = i1 - 1;
      if (i0 < 0) i0 = s - 1;

      vector3 v0 = vectlist::Value(i0);
      vector3 v1 = vectlist::Value(i1);
      vector3 v2 = vectlist::Value(i2);
      vector3 v3 = vectlist::Value(i3);

      // angepasste quadr. Funktion an i0,i1,i2
      vector3 a1 = v0 * 0.5 - v1 + v2 * 0.5;
      vector3 b1 = -v0 * 1.5 + v1 * 2 - v2 * 0.5;
      vector3 c1 = v0;

      // angepasste quadr. Funktion an i1,i2,i3
      vector3 a2 = v1 * 0.5 - v2 + v3 * 0.5;
      vector3 b2 = -v1 * 2.5 + v2 * 4 - v3 * 1.5;
      vector3 c2 = v1 * 3 - v2 * 3 + v3;

      float idxh = fract + 1;
      // lineare Interpolation der Funktionswerte von f1 und f2
      vector3 f1 = a1 * idxh * idxh + b1 * idxh + c1;
      vector3 f2 = a2 * idxh * idxh + b2 * idxh + c2;

      temp = f1 * (1 - fract) + f2 * fract;
    }
    }
  return temp;
}

void vectlist::Invert()
{
  for (unsigned int i = 0; i < vlist.size() / 2; i++)
    {
      int ir = vlist.size() - 1 - i;
      vectp h = vlist[i];
      vlist[i] = vlist[ir];
      vlist[ir] = h;
    }
}

const vector3 &splineaccess::Value() const
{
// berechnung eines Spline-Wertes
  float idx = index->Value();
  return val->Value(idx);
}

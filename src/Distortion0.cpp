/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <sstream>

#include "Distortion0.h"

using namespace std;

namespace ice
{
  /*
   * Distortion 0
   * r' = r * (1.0 + d2 * r^2)
   */

#define FNAME "Distortion0::Distortion0"
  // constructor
  Distortion0::Distortion0(): DistortionR2(), d2(0.0) {}

  //constructor
  Distortion0::Distortion0(double x0p, double y0p, double d2p): DistortionR2(x0p, y0p), d2(d2p) {}

  Distortion0::Distortion0(double x0p, double y0p, double d2p, int dimx, int dimy):
    DistortionR2(x0p, y0p, dimx, dimy)
  {
    int maxdim = dimx > dimy ? dimx : dimy;
    d2 = d2p / maxdim / maxdim;
  }

  Distortion0& Distortion0::operator =(const Distortion0& d)
  {
    x0 = d.x0;
    y0 = d.y0;
    d2 = d.d2;
    return *this;
  }

  // constructor for constructing distortion instances from references
  Distortion0::Distortion0(const Matrix& mark, const Matrix& orig,
                           Trafo& tr, const Vector& ImageCenter)
  {
    Calc(mark, orig, tr, ImageCenter[0], ImageCenter[1]);
  }

  Distortion0::Distortion0(const Matrix& mark, const Matrix& orig,
                           Trafo& tr)
  {
    Calc(mark, orig, tr);
  }

  Distortion0::Distortion0(const Matrix& mark, const Matrix& orig, const Vector& ImageCenter)
  {
    Calc(mark, orig, ImageCenter[0], ImageCenter[1]);
  }

  Distortion0::Distortion0(const Matrix& mark, const Matrix& orig)
  {
    Calc(mark, orig);
  }
#undef FNAME
#define FNAME "Distortion0::ffr"
  double Distortion0::ffr2(double r2) const
  {
    //    cout << "0 ";
    return 1.0 + d2 * r2;
  }
#undef FNAME
#define FNAME "Distortion0::frs"
  double Distortion0::frs(double r) const
  {
    double r2 = r * r;
    return r = 1.0 + 3.0 * d2 * r2;
  }
#undef FNAME

  Vector Distortion0::MakeVector() const
  {
    return Vector(x0, y0, d2);
  }

#define FNAME "Distortion0::Set"
  void Distortion0::Set(const Vector& v)
  {
    if (v.size() != 3)
      throw IceException(FNAME, M_WRONG_DIM);

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
  }
#undef FNAME

#define FNAME "Distortion0::toString"
  string Distortion0::toString() const
  {
    ostringstream os;
    os << "x0: " << x0 << " y0: " << y0 << " d2: " << d2 ;
    return os.str();
  }
#undef FNAME
#define FNAME "Distortion0::Set"
  void Distortion0::Set(const string& parastr)
  {
    istringstream is(parastr);

    if (! ReadPara(is, "x0", x0) || ! ReadPara(is, "y0", y0) || ! ReadPara(is, "d2", d2))
      throw IceException(FNAME, M_WRONG_FORMAT);
  }
#undef FNAME
}

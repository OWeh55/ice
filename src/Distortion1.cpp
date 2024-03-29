/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <sstream>

#include "strtool.h"
#include "Trafo.h"
#include "geo.h"
#include "lmdif.h"
#include "macro.h"
#include "geo.h"

#include "Distortion1.h"

using namespace std;

namespace ice
{
  /*************************************************************
   * distortion model r' = r * ( 1 + d2 * r^2 + d4 * r^4 )
   *************************************************************/

#define FNAME "Distortion1::ffr2"
  double Distortion1::ffr2(double r2) const
  {
    return 1.0 + (d4 * r2 + d2) * r2;
  }
#undef FNAME
#define FNAME "Distortion1::frs"
  double Distortion1::frs(double r) const
  {
    double r2 = r * r;
    double r4 = r2 * r2;
    return 1.0 + 3.0 * d2 * r2 + 5.0 * d4 * r4;
  }
#undef FNAME

#define FNAME "Distortion1::Distortion1"
  // constructor
  Distortion1::Distortion1(): DistortionR2()
  {
    d2 = 0.0;
    d4 = 0.0;
  }

  //constructor
  Distortion1::Distortion1(double x0p, double y0p, double d2p, double d4p): DistortionR2(x0p, y0p)
  {
    d2 = d2p;
    d4 = d4p;
  }

  Distortion1::Distortion1(double x0p, double y0p, double d2p, double d4p, int dimx, int dimy):
    DistortionR2(x0p, y0p, dimx, dimy)
  {
    int maxdim = dimx > dimy ? dimx : dimy;
    d2 = d2p / maxdim / maxdim;
    d4 = d4p / maxdim / maxdim / maxdim;
  }

  Distortion1& Distortion1::operator =(const Distortion1& d)
  {
    x0 = d.x0;
    y0 = d.y0;
    d2 = d.d2;
    d4 = d.d4;
    return *this;
  }

  // constructor for constructing distortion instances from references
  Distortion1::Distortion1(const Matrix& mark, const Matrix& orig,
                           Trafo& tr, const Vector& ImageCenter)
  {
    calculate(mark, orig, tr, ImageCenter[0], ImageCenter[1]);
  }

  Distortion1::Distortion1(const Matrix& mark, const Matrix& orig,
                           Trafo& tr)
  {
    calculate(mark, orig, tr);
  }

  Distortion1::Distortion1(const Matrix& mark, const Matrix& orig, const Vector& ImageCenter)
  {
    calculate(mark, orig, ImageCenter[0], ImageCenter[1]);
  }

  Distortion1::Distortion1(const Matrix& mark, const Matrix& orig)
  {
    calculate(mark, orig);
  }
#undef FNAME

#define FNAME "Distortion1::toString"
  string Distortion1::toString() const
  {
    ostringstream os;
    os << "x0: " << x0 << " y0: " << y0 << " d2: " << d2 << " d4: " << d4 ;
    return os.str();
  }
#undef FNAME

#define FNAME "Distortion1::set"
  void Distortion1::set(const string& parastr)
  {
    istringstream is(parastr);

    if (! readPara(is, "x0", x0) || ! readPara(is, "y0", y0) ||
        ! readPara(is, "d2", d2) || ! readPara(is, "d4", d4))
      throw IceException(FNAME, M_WRONG_FILEFORMAT);
  }
#undef FNAME

  vector<double> Distortion1::makeVectorDouble() const
  {
    return {x0, y0, d2, d4};
  }

#define FNAME "Distortion1::set"
  void Distortion1::set(const Vector& v)
  {
    if (v.size() != 4)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
    d4 = v[3];
  }

  void Distortion1::set(const std::vector<double>& v)
  {
    if (v.size() < 4)
      throw IceException(FNAME, M_WRONG_DIMENSION);

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
    d4 = v[3];
  }
#undef FNAME
}

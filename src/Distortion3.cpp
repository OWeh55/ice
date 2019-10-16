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

#include <fstream>
#include <sstream>

#include "strtool.h"
#include "Trafo.h"
#include "geo.h"
#include "lmdif.h"
#include "macro.h"
#include "geo.h"
//#include "bairstow.h"

#include "Distortion3.h"

using namespace std;

namespace ice
{
  /*************************************************************
   * Klasse Modell 3
   *************************************************************/

#define FNAME "Distortion3::ffr"
  double Distortion3::ffr2(double r2) const
  {
    return 1.0 + ((d6 * r2 + d4) * r2 + d2) * r2;
  }
#undef FNAME
#define FNAME "Distortion3::frs"
  double Distortion3::frs(double r) const
  {
    double r2 = r * r;
    return r  = 1.0 + ((7.0 * d6 * r2 + 5.0 * d4) * r2 + 3.0 * d2) * r2;
  }
#undef FNAME

#define FNAME "Distortion::Distortion"
  // constructor
  Distortion3::Distortion3(): DistortionR2()
  {
    this->d2 = 0.0;
    this->d4 = 0.0;
    this->d6 = 0.0;
  }

  //constructor
  Distortion3::Distortion3(double x0, double y0, double d2, double d4, double d6): DistortionR2(x0, y0)
  {
    this->d2 = d2;
    this->d4 = d4;
    this->d6 = d6;
  }

  Distortion3::Distortion3(double x0p, double y0p, double d2p, double d4p, double d6p,
                           int dimx, int dimy):
    DistortionR2(x0p, y0p, dimx, dimy)
  {
    int maxdim = dimx > dimy ? dimx : dimy;
    double maxdim2 = maxdim * maxdim;
    double maxdim4 = maxdim2 * maxdim2;
    double maxdim6 = maxdim2 * maxdim4;

    d2 = d2p / maxdim2;
    d4 = d4p / maxdim4;
    d6 = d6p / maxdim6;
  }

  Distortion3& Distortion3::operator =(const Distortion3& d)
  {
    x0 = d.x0;
    y0 = d.y0;
    d2 = d.d2;
    d4 = d.d4;
    d6 = d.d6;
    return *this;
  }

  Distortion3::Distortion3(const Matrix& mark, const Matrix& orig,
                           Trafo& tr, const Vector& ImageCenter)
  {
    calculate(mark, orig, tr, ImageCenter[0], ImageCenter[1]);
  }

  Distortion3::Distortion3(const Matrix& mark, const Matrix& orig,
                           Trafo& tr)
  {
    calculate(mark, orig, tr);
  }

  Distortion3::Distortion3(const Matrix& mark, const Matrix& orig,
                           const Vector& ImageCenter)
  {
    calculate(mark, orig, ImageCenter[0], ImageCenter[1]);
  }

  Distortion3::Distortion3(const Matrix& mark, const Matrix& orig)
  {
    calculate(mark, orig);
  }
#undef FNAME
  string Distortion3::toString() const
  {
    ostringstream os;
    os << "x0: " << x0 << " y0: " << y0 ;
    os << " d2: " << d2 << " d4: " << d4 << " d6: " << d6 ;
    return os.str();
  }

#define FNAME "Distortion3::Set"
  void Distortion3::set(const string& parastr)
  {
    istringstream is(parastr);

    if (! readPara(is, "x0", x0) || ! readPara(is, "y0", y0) ||
        ! readPara(is, "d2", d2) || ! readPara(is, "d4", d4) || ! readPara(is, "d6", d6))
      throw IceException(FNAME, M_WRONG_FORMAT);
  }
#undef FNAME

  vector<double> Distortion3::makeVectorDouble() const
  {
    return {x0, y0, d2, d4, d6};
  }

#define FNAME "Distortion3::set"
  void Distortion3::set(const Vector& v)
  {
    if (v.size() != 5)
      throw IceException(FNAME, M_WRONG_DIM);

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
    d4 = v[3];
    d6 = v[4];
  }
  void Distortion3::set(const std::vector<double>& v)
  {
    if (v.size() < 5)
      throw IceException(FNAME, M_WRONG_DIM);

    x0 = v[0];
    y0 = v[1];
    d2 = v[2];
    d4 = v[3];
    d6 = v[4];
  }
#undef FNAME
}

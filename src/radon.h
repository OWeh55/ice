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
/*
   radon.h,
   W.Ortmann, 2011
*/
#ifndef _RADON_H
#define _RADON_H

#include "based.h"
#include "Vector.h"
#include "Matrix.h"

namespace ice
{

  void RadonImg(Image& srcimg, Image& radonimg);
  void InvRadonImg(Image& radonimg, Image& resimg, int fmax = -1, int lines = -1, bool filtered = true);

  class RadonCTrafo
  {
    // Klasse die von Radon-Koordinaten (im radonbild)
    // auf geradenparameter und auf Koordinaten im Ursprungsbild umrechnet
  private:
    const unsigned int xs, ys;
    const int xr, yr;
    const double diag;
    const double alphafac;
    const double pfac;
    const double p0;
    const double xm;
    const double ym;
    double sina;
    double cosa;
    double x0;
    double y0;
  public:
    RadonCTrafo(int xsp, int ysp, // Größe Quellbild (nicht radon)
                int xrp, int yrp // Größe Radonbild
               ):
      xs(xsp), ys(ysp), xr(xrp), yr(yrp),
      diag(Distance(xsp, ysp)), alphafac(M_PI / yr), pfac(diag / xr),
      p0(-0.5 * diag), xm(0.5 * xsp), ym(0.5 * ysp)
    {
    }

    void setYR(int y)
    {
      double alpha = y * alphafac;
      sina = sin(alpha);
      cosa = cos(alpha);
    }

    void setXR(int x)
    {
      double p = p0 + x * pfac;
      x0 = xm - sina * p;
      y0 = ym + cosa * p;
    }

    inline bool getXYS(double p, int& xt, int& yt) const
    {
      xt = RoundInt(x0 + p * cosa);
      yt = RoundInt(y0 + p * sina);
      return ((unsigned int)xt < xs && (unsigned int)yt < ys);
    }

    inline void getXY0(double& xt, double& yt) const
    {
      xt = x0  - 0.5 * diag * cosa;
      yt = y0  - 0.5 * diag * sina;
    }

    inline void getDXY(double step, double& dx, double& dy) const
    {
      dx = step * cosa;
      dy = step * sina;
    }

    int getXR(double xs, double ys) const
    {
      double p = - (xs - xm) * sina + (ys - ym) * cosa;
      return RoundInt((p - p0) / pfac);
    }

    double getStepNumber(double sres) const
    {
      return diag / sres;
    }

    double getMinPar() const
    {
      return -diag;
    }
    double getMaxPar() const
    {
      return  diag;
    }
  };

}
#endif

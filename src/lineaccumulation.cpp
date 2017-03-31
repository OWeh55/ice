/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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

#include "lineseg.h"
#include "Accumulator2.h"

using namespace std;

namespace ice
{
#define FNAME "DetectLine"

  LineSeg detectLine(const std::vector<Point>& pl)
  {
    if (pl.size() < 2)
      {
        throw logic_error("detectLine - not enough points");
      }

    double pmax2 = 0;
    for (unsigned int i = 0; i < pl.size(); i++)
      {
        double p = pl[i].r2();

        if (p > pmax2)
          {
            pmax2 = p;
          }
      }

    int pmax = ceil(sqrt(pmax2));

    Accumulator2 lineakku(1800, -M_PI, M_PI, true, 5,
                          pmax, 0, pmax, false, 5);

    for (unsigned int i = 0; i < pl.size() - 1; i++)
      for (unsigned int j = i + 1; j < pl.size(); j++)
        {
          LineSeg aline(pl[i], pl[j], LineSeg::line);

          if (aline.Length2() > 1.0)
            {
              double p, phi;
              aline.CalcHesse(p, phi);
              lineakku.add(phi, p);
            }
        }

    double p, phi;
    lineakku.getMax(phi, p);
    return LineSeg(p, phi);
  }

  LineSeg detectLine(const std::vector<Point>& pl, int nPairs)
  {
    int nPoints = pl.size();

    if (nPoints < 2)
      {
        throw logic_error("detectLine - not enough points");
      }

    if (nPairs < 0)
      {
        throw logic_error("detectLine - wrong number of pairs");
      }

    if (nPairs == 0 || nPairs > nPoints * nPoints)   // no limitation
      {
        return detectLine(pl);
      }

    // find max distance to origin
    double pmax2 = 0;

    for (int i = 0; i < nPoints; i++)
      {
        double p = pl[i].r2();
        if (p > pmax2)
          {
            pmax2 = p;
          }
      }

    int pmax = ceil(sqrt(pmax2));

    Accumulator2 lineakku(1800, -M_PI, M_PI, true, 5,
                          pmax, 0, pmax, false, 5);

    for (int i = 0; i < nPairs; i++)
      {
        int i1 = Random(nPoints - 1);
        int i2 = Random(nPoints - 1);

        while (i1 == i2)
          {
            i2 = Random(nPoints - 1);
          }

        LineSeg aline(pl[i1], pl[i2], LineSeg::line);

        if (aline.Length2() > 1.0)
          {
            double p, phi;
            aline.CalcHesse(p, phi);
            lineakku.add(phi, p);
          }
      }

    double p, phi;
    lineakku.getMax(phi, p);
    return LineSeg(p, phi);
  }
#undef FNAME
}

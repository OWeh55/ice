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

#include "macro.h"

#include "WindowWalker.h"
#include "imatch.h"

namespace ice
{

#define FNAME "CorrCoefficient"
  static int momente(const Image& img1, const Image& img2,
                     int& n, int& a, int& b, int& aa, int& ab, int& bb)
  {
    n = 0;
    a = 0;
    b = 0;
    aa = 0;
    ab = 0;
    bb = 0;

    ReturnErrorIfFailed(MatchImg(img1, img2));

    WindowWalker w(img1);
    for (w.init(); !w.ready(); w.next())
      {
        int va = img1.getPixel(w);
        int vb = img2.getPixel(w);
        n++;
        a += va;
        b += vb;
        aa += va * va;
        ab += va * vb;
        bb += vb * vb;
      }

    return 0;
  }
#undef FNAME

#define FNAME "Correlation"
  double CorrCoefficient(const Image& img1, const Image& img2)
  {
    int n, a, b, aa, ab, bb;
    ReturnErrorIfFailed(momente(img1, img2, n, a, b, aa, ab, bb));
    double dn = n, da = a, db = b, daa = aa, dab = ab, dbb = bb;

    double z = dab - da * db / dn;
    double na = daa - da * da / dn;
    double nb = dbb - db * db / dn;
    return z / sqrt(na * nb);
  }
#undef FNAME
}

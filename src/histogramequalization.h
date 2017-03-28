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
/*********************************************************************
  HistogramEqualization
**********************************************************************/

#ifndef _ICE_HISTOGRAMEQUALIZATION
#define _ICE_HISTOGRAMEQUALIZATION

#include <vector>
#include "base.h"
#include "histogram.h"

namespace ice
{
  /**********************************************************************

    Histogrammausgleich

  ***********************************************************************/

  int HistogramEqualizationLUT(const Histogram& h, std::vector<int>& lut,
                               int maxv,
                               double reg);
  int HistogramEqualizationLUT(const Image& img,
                               std::vector<int>& lut,
                               int maxval = -1,
                               double reg = 0.0);
  int HistogramEqualization(const Image& imgs, const Image& imgt, double reg = 0.0);
  int HistogramEqualization(const Image& img, double reg = 0.0);
}
#endif

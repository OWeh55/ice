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
//
// Bestimmung von Schwellen für Kontur- Kanten- und Liniensuche
// (Methode von Otsu)
//  m. schubert 2/97
//  Ortmann 1999...2015
//
#ifndef _THRESHLD_H
#define _THRESHLD_H

#include <vector>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "hist.h"
#include "histogram.h"

namespace ice
{
  double CalcThreshold(const Hist& h);
  double CalcThreshold(const Hist& h, double& discmax);
  int CalcThreshold(const Histogram& h);
  int CalcThreshold(const Histogram& h, double& discmax);

  int CalcThreshold(const Image& img);
  int CalcThreshold(const Image& img, double& discmax, int diff = 1);

  double CalcThreshold(const std::vector<double>& value, double& discmax);
  int CalcThreshold(const std::vector<int>& value, double& discmax);

  int GradThreshold(const Image& img);
  int RidgeThreshold(const Image& img);
}
#endif

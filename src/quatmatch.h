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
 * Author: Alexander Lärz, 2005
 */

#ifndef QUATMATCH_H
#define QUATMATCH_H

#include "trafodualquaternion.h"
#include "rotquaternion.h"
#include "Matrix.h"

using namespace std;

namespace ice
{
  TrafoDualQuaternion estimateTransformation(const Matrix& orig, const Matrix& trans);

  TrafoDualQuaternion estimateTransformationWeighted(const Matrix& orig, const Matrix& trans, const Matrix& weights);
}
#endif

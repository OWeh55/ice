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
#include <stdlib.h>
#include <math.h>
#include <fstream>

#include "defs.h"
#include "message.h"
#include "macro.h"
#include "matrix_function.h"
#include "numbase.h"

#include "Matrix.h"
#include "matrixtemplate.h"

#define PRECISION 1e-200

#define ERR(f,m,r,ret) { Message(f,m,r); return ret; }

using namespace std;

namespace ice
{

#define FNAME "Inverse"
  Matrix Inverse(const Matrix& m)
  {
    int dim = m.cols();

    Matrix h(m);

    if (dim != m.rows())
      {
        Message(FNAME, M_NO_SQUARE, WRONG_PARAM);
        return h;
      }

    double max, s, q, pivot;
    vector<int> p_k(dim);

    for (int k = 0; k < dim; k++)
      {
        max = 0;
        p_k[k] = 0;

        for (int i = k; i < dim; i++)
          {
            s = 0;

            for (int j = k; j < dim; j++)
              {
                s = s + fabs(h[i][j]);
              }

            if (s != 0)
              {
                q = fabs(h[i][k]) / s;
              }
            else
              {
                q = 0;
              }

            if (q > max)
              {
                max = q;
                p_k[k] = i;
              }
          }

        if (max == 0)
          {
            Message(FNAME, M_NO_INVERSE, NO_SOLUTION);
            return h;
          }

        if (p_k[k] != k)
          {
            for (int j = 0; j < dim; j++)
              {
                std::swap(h[k][j], h[p_k[k]][j]);
              }
          }

        pivot = h[k][k];

        for (int j = 0; j < dim; j++)
          {
            if (j != k)
              {
                h[k][j] = (-h[k][j]) / pivot;

                for (int i = 0; i < dim; i++)
                  {
                    if (i != k)
                      {
                        h[i][j] = h[i][j] + (h[i][k] * h[k][j]);
                      }
                  }
              }
          }

        for (int i = 0; i < dim; i++)
          {
            h[i][k] = h[i][k] / pivot;
          }

        h[k][k] = 1 / pivot;
      }

    for (int k = (dim - 2); k >= 0; k--)
      {
        if (p_k[k] != k)
          {
            for (int i = 0; i < dim; i++)
              {
                std::swap(h[i][k], h[i][p_k[k]]);
              }
          }
      }

    return h;
  }


#define FNAME "Inverse"
  matrix<double> Inverse(const matrix<double>& m)
  {
    int dim = m.cols();

    matrix<double> h(m);

    if (dim != m.rows())
      {
        Message(FNAME, M_NO_SQUARE, WRONG_PARAM);
        return h;
      }

    double max, s, q, pivot;
    vector<int> p_k(dim);

    for (int k = 0; k < dim; k++)
      {
        max = 0;
        p_k[k] = 0;

        for (int i = k; i < dim; i++)
          {
            s = 0;

            for (int j = k; j < dim; j++)
              {
                s = s + fabs(h[i][j]);
              }

            if (s != 0)
              {
                q = fabs(h[i][k]) / s;
              }
            else
              {
                q = 0;
              }

            if (q > max)
              {
                max = q;
                p_k[k] = i;
              }
          }

        if (max == 0)
          {
            Message(FNAME, M_NO_INVERSE, NO_SOLUTION);
            return h;
          }

        if (p_k[k] != k)
          {
            for (int j = 0; j < dim; j++)
              {
                std::swap(h[k][j], h[p_k[k]][j]);
              }
          }

        pivot = h[k][k];

        for (int j = 0; j < dim; j++)
          {
            if (j != k)
              {
                h[k][j] = (-h[k][j]) / pivot;

                for (int i = 0; i < dim; i++)
                  {
                    if (i != k)
                      {
                        h[i][j] = h[i][j] + (h[i][k] * h[k][j]);
                      }
                  }
              }
          }

        for (int i = 0; i < dim; i++)
          {
            h[i][k] = h[i][k] / pivot;
          }

        h[k][k] = 1 / pivot;
      }

    for (int k = (dim - 2); k >= 0; k--)
      {
        if (p_k[k] != k)
          {
            for (int i = 0; i < dim; i++)
              {
                std::swap(h[i][k], h[i][p_k[k]]);
              }
          }
      }

    return h;
  }
#undef FNAME


}

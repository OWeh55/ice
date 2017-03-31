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

/*
 * Author: Alexander Lärz, 2005
 */


#include "quaternion.h"
#include "qft.h"

#include "Vector.h"
#include "Matrix.h"
#include "IceException.h"
#include "defs.h"
#include "base.h"
#include "macro.h"
#include "fourier.h"


namespace ice
{
#define FNAME "QFourier(QuatMatrix &input, QuatMatrix &output, int option)"
  int QFourier(QuatMatrix& input, QuatMatrix& output, int option)
  {
    int i, j;

    int M = input.getColumns();
    int N = input.getRows();

    QuatMatrix o;
    o = QuatMatrix(N, M);

    Vector re1 = Vector(M);

    Vector im1 = Vector(M);

    Vector re2 = Vector(M);

    Vector im2 = Vector(M);

    Matrix mre1 = Matrix(N, M);

    Matrix mim1 = Matrix(N, M);

    Matrix mre2 = Matrix(N, M);

    Matrix mim2 = Matrix(N, M);

    for (i = 0; i < N; i++)
      {
        for (j = 0; j < M; j++)
          {
            re1[j] = input[i][j].getReal();
            im1[j] = input[i][j].getI();

            re2[j] = input[i][j].getJ();
            im2[j] = input[i][j].getK();
          }

        Fourier(re1, im1, option);

        Fourier(re2, im2, option);

        mre1[i] = re1;
        mim1[i] = im1;

        mre2[i] = re2;
        mim2[i] = im2;
      }

    Vector re3 = Vector(N);

    Vector im3 = Vector(N);

    Vector re4 = Vector(N);

    Vector im4 = Vector(N);

    for (i = 0; i < M; i++)
      {
        for (j = 0; j < N; j++)
          {
            re3[j] = mre1[j][i];
            im3[j] = mre2[j][i];

            re4[j] = mim1[j][i];
            im4[j] = mim2[j][i];
          }


        Fourier(re3, im3, option);

        Fourier(re4, im4, option);

        for (j = 0; j < N; j++)
          {
            o[j][i].setReal(re3[j]);
            o[j][i].setI(re4[j]);
            o[j][i].setJ(im3[j]);
            o[j][i].setK(im4[j]);
          }
      }

    o = o * sqrt(double(M * N));

    if (option == INVERS)
      {
        double f = 1.0 / (M * N);

        o = o * f;
      }


    output = o;

    return OK;
  }
#undef FNAME

#define FNAME "PowerSpektrumQFT(QuatMatrix &input, Image &output, int type, int mode)"
  int PowerSpektrumQFT(QuatMatrix& input, Image& output, int type, int mode)
  {
    if (!IsImg(output))
      {
        throw IceException(FNAME, M_INVALID, INVALID);
      }

    if (int(input.getColumns()) != output->xsize || int(input.getRows()) != output->ysize)
      {
        throw IceException(FNAME, M_WRONG_IMGSIZE, WRONG_PARAM);
      }

    if (!(type == LOG || type == POWER || type == NORM))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }


    if (!(mode == CENTER || mode == NOCENTER))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }

    int maxcolor = output->maxval;
    int i, j;
    int X = input.getColumns();
    int Y = input.getRows();

    double maxnorm = 0;
    double norm, snorm, help;
    int xs, ys;

    if (type == LOG)
      {
        for (i = 0; i < Y; i++)
          {
            for (int j = 0; j < X; j++)
              {
                if (input[i][j].getNorm() > maxnorm)
                  {
                    maxnorm = input[i][j].getNorm();
                  }
              }
          }
      }

    for (i = 0; i < Y; i++)
      {
        for (j = 0; j < X; j++)
          {
            if (mode == CENTER)
              {
                if (j < X / 2)
                  {
                    xs = X / 2 + j;
                  }
                else
                  {
                    xs = j - X / 2 + X % 2;
                  }

                if (i < Y / 2)
                  {
                    ys = Y / 2 + i;
                  }
                else
                  {
                    ys = i - Y / 2 + Y % 2;
                  }
              }
            else
              {
                xs = j;
                ys = i;
              }

            norm = input[i][j].getNorm();
            snorm = norm * norm;

            if (type == POWER)
              {
                help = fmod(snorm / sqrt(double(X * Y)), maxcolor);

                PutVal(output, xs, ys, int(help));
              }

            if (type == NORM)
              {
                help = fmod(norm / sqrt(double(X * Y)), maxcolor);

                PutVal(output, xs, ys, int(help));
              }

            if (type == LOG)
              {
                if (snorm != 0)
                  {
                    help = log10(1.0 + snorm) / logf(1 + maxnorm * maxnorm) * (maxcolor - 1);
                  }
                else
                  {
                    help = 0;
                  }

                PutVal(output, xs, ys, int(help));
              }
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "EigenwinkelSpektrumQFT(QuatMatrix &input, Image &r, Image &g, Image &b, int mode)"
  int EigenwinkelSpektrumQFT(QuatMatrix& input, Image& r, Image& g, Image& b, int mode)
  {
    if (!(IsImg(r) && IsImg(g) && IsImg(g)))
      {
        throw IceException(FNAME, M_INVALID, INVALID);
        return ERROR;
      }

    if (int(input.getColumns()) != r->xsize || int(input.getRows()) != r->ysize ||
        int(input.getColumns()) != g->xsize || int(input.getRows()) != g->ysize ||
        int(input.getColumns()) != g->xsize || int(input.getRows()) != b->ysize)
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return ERROR;
      }

    if (r->maxval != g->maxval || r->maxval != b->maxval)
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return ERROR;
      }

    if (!(mode == CENTER || mode == NOCENTER))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ERROR;
      }

    int maxcolor = r->maxval;
    int i, j;
    int X = input.getColumns();
    int Y = input.getRows();
    int xs, ys;
    double I = (maxcolor - 1) / 3.0;
    double ew, red, green, blue;


    for (i = 0; i < Y; i++)
      {
        for (j = 0; j < X; j++)
          {
            if (mode == CENTER)
              {
                if (j < X / 2)
                  {
                    xs = X / 2 + j;
                  }
                else
                  {
                    xs = j - X / 2 + X % 2;
                  }

                if (i < Y / 2)
                  {
                    ys = Y / 2 + i;
                  }
                else
                  {
                    ys = i - Y / 2 + Y % 2;
                  }
              }
            else
              {
                xs = j;
                ys = i;
              }

            bool ok = true;
            IF_FAILED(ew = input[i][j].getEigenwinkel())
            {
              red = 255;
              green = 255;
              blue = 255;
              ok = false;
            }

            if (ok)
              {
                if (ew < (2 * M_PI / 3))
                  {
                    blue = 0;
                    red = I * (1.0 + (cos(ew) / cos((M_PI / 3.0) - ew)));
                    green = 3.0 * I - (red + blue);
                  }
                else
                  {
                    red = 0;
                    green = I * (1.0 + (cos(ew - (2.0 * M_PI / 3.0)) / cos(M_PI - ew)));
                    blue = 3 * I - (red + green);
                  }
              }

            PutVal(r, xs, ys, int(red));
            PutVal(g, xs, ys, int(green));
            PutVal(b, xs, ys, int(blue));
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "EigenachsenSpektrumQFT(QuatMatrix &input, Image &r, Image &g, Image &b, int mode)"
  int EigenachsenSpektrumQFT(QuatMatrix& input, Image& r, Image& g, Image& b, int mode)
  {
    if (!(IsImg(r) && IsImg(g) && IsImg(g)))
      {
        throw IceException(FNAME, M_INVALID, INVALID);
        return ERROR;
      }

    if (int(input.getColumns()) != r->xsize || int(input.getRows()) != r->ysize ||
        int(input.getColumns()) != g->xsize || int(input.getRows()) != g->ysize ||
        int(input.getColumns()) != g->xsize || int(input.getRows()) != b->ysize)
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return ERROR;
      }

    if (r->maxval != g->maxval || r->maxval != b->maxval)
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return ERROR;
      }

    if (!(mode == CENTER || mode == NOCENTER))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ERROR;
      }

    int maxcolor = r->maxval;
    int i, j;
    int X = input.getColumns();
    int Y = input.getRows();
    int xs, ys;
    double red, green, blue;
    Quaternion mu;


    for (i = 0; i < Y; i++)
      {
        for (j = 0; j < X; j++)
          {
            if (mode == CENTER)
              {
                if (j < X / 2)
                  {
                    xs = X / 2 + j;
                  }
                else
                  {
                    xs = j - X / 2 + X % 2;
                  }

                if (i < Y / 2)
                  {
                    ys = Y / 2 + i;
                  }
                else
                  {
                    ys = i - Y / 2 + Y % 2;
                  }
              }
            else
              {
                xs = j;
                ys = i;
              }

            bool ok = true;
            IF_FAILED(mu = input[i][j].getEigenachse())
            {
              red = maxcolor;
              green = maxcolor;
              blue = maxcolor;
              ok = false;
            }

            if (ok)
              {
                red = maxcolor / 2 + (maxcolor / 2) * mu.getI();
                green = maxcolor / 2 + (maxcolor / 2) * mu.getJ();
                blue = maxcolor / 2 + (maxcolor / 2) * mu.getK();
              }

            PutVal(r, xs, ys, int(red));
            PutVal(g, xs, ys, int(green));
            PutVal(b, xs, ys, int(blue));
          }
      }

    return OK;
  }
#undef FNAME

#define FNAME "PhasenSpektrumQFT(QuatMatrix &input, Image &alpha, Image &beta, Image &delta, int mode)"
  int PhasenSpektrumQFT(QuatMatrix& input, Image& alpha, Image& beta, Image& delta, int mode)
  {
    if (!(IsImg(alpha) && IsImg(beta) && IsImg(delta)))
      {
        throw IceException(FNAME, M_INVALID, INVALID);
        return ERROR;
      }

    if (int(input.getColumns()) != alpha->xsize || int(input.getRows()) != alpha->ysize ||
        int(input.getColumns()) != beta->xsize || int(input.getRows()) != beta->ysize ||
        int(input.getColumns()) != delta->xsize || int(input.getRows()) != delta->ysize)
      {
        throw IceException(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
        return ERROR;
      }

    /*
        if(r->maxval != g->maxval || r->maxval != b->maxval)
        {
    throw IceException(FNAME,M_SIZES_DIFFER,WRONG_PARAM);
          return ERROR;
        }*/

    if (!(mode == CENTER || mode == NOCENTER))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ERROR;
      }

    int i, j;
    int X = input.getColumns();
    int Y = input.getRows();
    int xs, ys;
    double a, b, d;
    Vector3d phases;

    for (i = 0; i < Y; i++)
      {
        for (j = 0; j < X; j++)
          {
            if (mode == CENTER)
              {
                if (j < X / 2)
                  {
                    xs = X / 2 + j;
                  }
                else
                  {
                    xs = j - X / 2 + X % 2;
                  }

                if (i < Y / 2)
                  {
                    ys = Y / 2 + i;
                  }
                else
                  {
                    ys = i - Y / 2 + Y % 2;
                  }
              }
            else
              {
                xs = j;
                ys = i;
              }


            bool ok = true;
            IF_FAILED(phases = input[i][j].getPhases())
            {
              a = alpha->maxval;
              b = beta->maxval;
              d = delta->maxval;
              ok = false;
            }

            if (ok)
              {
                a = alpha->maxval / 2 + phases.x * (alpha->maxval) / (2 * M_PI);
                b = beta->maxval / 2 + phases.y * (beta->maxval) / M_PI;
                d = (delta->maxval - 1) / 2 + phases.z * 4 * ((delta->maxval - 1) / 2) / M_PI;
              }

            PutVal(alpha, xs, ys, int(a));
            PutVal(beta, xs, ys, int(b));
            PutVal(delta, xs, ys, int(d));
          }
      }

    return OK;
  }
#undef FNAME
}

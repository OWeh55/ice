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
#include <math.h>
#include <stdio.h>
#include <malloc.h>

#include "IceException.h"
#include "macro.h"
#include "mateigen.h"
#include "matdef.h"

namespace ice
{
#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
                          a[k][l]=h+s*(g-h*tau);

  void Eigsrt(double* d, double** v, int n);

#define FNAME "Mateigen"
  MatrixStruct Mateigen(MatrixStruct A, double* d, MatrixStruct evect)
  {
    MatrixStruct Amat;
    double** a, **v;
    int nrot, n;
    int j, iq, ip, i;
    double tresh, theta, tau, t, sm, s, h, g, c, *b, *z;

    /* Testung der Parameter */
    if (IsMatrix(A) == false)
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    Amat = MoveMat(A, nullptr);
    n = A->rsize;

    if (evect == nullptr)
      {
        evect = NewMatrix(0, n, n);
      }
    else
      {
        if (IsMatrix(evect) == false)
          throw IceException(FNAME, M_WRONG_MATRIX);

        if (evect->csize != n || evect->rsize != n || evect->type != MAT_DOUBLE)
          throw IceException(FNAME, M_MAT_NO_COMPAT);
      }

    // pointers to data
    a = Amat->data;
    v = evect->data;
    //
    b = (double*)malloc(n * sizeof(double));
    z = (double*)malloc(n * sizeof(double));

    if (b == nullptr || z == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    //
    for (ip = 0; ip < n; ip++)
      {
        for (iq = 0; iq < n; iq++)
          {
            v[ip][iq] = 0.0;
          }

        v[ip][ip] = 1.0;
      }

    for (ip = 0; ip < n; ip++)
      {
        b[ip] = d[ip] = a[ip][ip];
        z[ip] = 0.0;
      }

    nrot = 0;

    for (i = 1; i <= 50; i++)
      {
        sm = 0.0;

        for (ip = 0; ip < n - 1; ip++)
          {
            for (iq = ip + 1; iq < n; iq++)
              {
                sm += fabs(a[ip][iq]);
              }
          }

        if (sm == 0.0)
          {
            free(z);
            free(b);
            FreeMatrix(Amat);
            Eigsrt(d, evect->data, n);
            return (evect);
          }

        if (i < 4)
          {
            tresh = 0.2 * sm / (n * n);
          }
        else
          {
            tresh = 0.0;
          }

        for (ip = 0; ip < n - 1; ip++)
          {
            for (iq = ip + 1; iq < n; iq++)
              {
                g = 100.0 * fabs(a[ip][iq]);

                if (i > 4 && fabs(d[ip]) + g == fabs(d[ip])
                    && fabs(d[iq]) + g == fabs(d[iq]))
                  {
                    a[ip][iq] = 0.0;
                  }
                else if (fabs(a[ip][iq]) > tresh)
                  {
                    h = d[iq] - d[ip];

                    if (fabs(h) + g == fabs(h))
                      {
                        t = (a[ip][iq]) / h;
                      }
                    else
                      {
                        theta = 0.5 * h / (a[ip][iq]);
                        t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));

                        if (theta < 0.0)
                          {
                            t = -t;
                          }
                      }

                    c = 1.0 / sqrt(1 + t * t);
                    s = t * c;
                    tau = s / (1.0 + c);
                    h = t * a[ip][iq];
                    z[ip] -= h;
                    z[iq] += h;
                    d[ip] -= h;
                    d[iq] += h;
                    a[ip][iq] = 0.0;

                    for (j = 0; j <= ip - 1; j++)
                      {
                        ROTATE(a, j, ip, j, iq)
                      }

                    for (j = ip + 1 ; j <= iq - 1; j++)
                      {
                        ROTATE(a, ip, j, j, iq)
                      }

                    for (j = iq + 1 ; j < n; j++)
                      {
                        ROTATE(a, ip, j, iq, j)
                      }

                    for (j = 0; j < n; j++)
                      {
                        ROTATE(v, j, ip, j, iq)
                      }

                    ++nrot;
                  }
              }
          }

        for (ip = 0; ip < n; ip++)
          {
            b[ip] += z[ip];
            d[ip] = b[ip];
            z[ip] = 0.0;
          }

        //
      }

    FreeMatrix(Amat);
    free(b);
    free(z);
    throw IceException(FNAME, M_NO_SOLUTION);
  }

  /* interne Funktion: Sortierung der Eigenwerte, Eigenvektoren nach Betrag */
  void Eigsrt(double d[], double** v, int n)
  {
    int k, j, i;
    double p;

    for (i = 0; i < n - 1; i++)
      {
        p = d[k = i];

        for (j = i + 1; j < n; j++)
          if (fabs(d[j]) >= fabs(p))
            {
              p = d[k = j];
            }

        if (k != i)
          {
            d[k] = d[i];
            d[i] = p;

            for (j = 0; j < n; j++)
              {
                p = v[j][i];
                v[j][i] = v[j][k];
                v[j][k] = p;
              }
          }
      }
  }
#undef ROTATE
#undef FNAME

#define SVDCMP
#define FNAME "SingularValueDcmp"

  static double at, bt, ct;
#define PYTHAG(a,b) ((at=fabs(a)) > (bt=fabs(b)) ? \
(ct=bt/at,at*sqrt(1.0+ct*ct)) : (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0))

  static double maxarg1, maxarg2;
#define MAX_NR(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
   (maxarg1) : (maxarg2))
#define SIGN_NR(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

  int SingularValueDcmp(MatrixStruct A, MatrixStruct* U, MatrixStruct* W, MatrixStruct* V)
  {
    double** a, *w, **v;
    int m, n;
    int flag, i, its, j, jj, k, l, nm;
    double c, f, h, s1, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;
    double* rv1;
    l = 0;
    nm = 0;

    /* Testung der Parameter */
    if (IsMatrix(A) == false)
      throw IceException(FNAME, M_WRONG_MATRIX);

    if (A->type != MAT_DOUBLE)
      throw IceException(FNAME, M_WRONG_MATRIXTYPE);

    m = A->rsize;
    n = A->csize;

    if (m < n)
      throw IceException(FNAME, M_MATRIXFORMAT);

    /* Matrizen U,W,V anfordern */
    /* Matrix U */
    if (*U == nullptr)
      {
        *U = NewMatrix(MAT_DOUBLE, m, n);
      }
    else
      {
        if (IsMatrix(*U) == false)
          throw IceException(FNAME, M_WRONG_MATRIX);

        if ((*U)->rsize != m || (*U)->type != MAT_DOUBLE || (*U)->csize != n)
          throw IceException(FNAME, M_MAT_NO_COMPAT);
      }

    /* W - Die Singulaerwerte */
    if (*W == nullptr)
      {
        *W = NewMatrix(0, n, n);
      }
    else
      {
        if (IsMatrix(*W) == false)
          throw IceException(FNAME, M_WRONG_MATRIX);

        if ((*W)->rsize != n || (*W)->type != MAT_DOUBLE || (*W)->csize != n)
          throw IceException(FNAME, M_MAT_NO_COMPAT);
      }

    for (i = 1; i < n; i++)
      for (j = 0; j < n; j++)
        {
          (*W)->data[i][j] = 0.0;
        }

    /* Matrix V */
    if (*V == nullptr)
      {
        *V = NewMatrix(0, n, n);
      }
    else
      {
        if (! IsMatrix(*V))
          throw IceException(FNAME, M_WRONG_MATRIX);

        if ((*V)->rsize != n || (*V)->type != MAT_DOUBLE || (*V)->csize != n)
          throw IceException(FNAME, M_MAT_NO_COMPAT);
      }

    /* Zeigeruebergabe */
    *U = MoveMat(A, *U);
    a = (*U)->data;
    w = (*W)->data[0];
    v = (*V)->data;

    /* der eigentliche Algorithmus */
    rv1 = (double*) malloc(n * sizeof(double));

    for (i = 0; i < n; i++)
      {
        l = i + 1;
        rv1[i] = scale * g;
        g = s1 = scale = 0.0;

        if (i < m)
          {
            for (k = i; k < m; k++)
              {
                scale += fabs(a[k][i]);
              }

            if (scale)
              {
                for (k = i; k < m; k++)
                  {
                    a[k][i] /= scale;
                    s1 += a[k][i] * a[k][i];
                  }

                f = a[i][i];
                g = -SIGN_NR(sqrt(s1), f);
                h = f * g - s1;
                a[i][i] = f - g;

                if (i != n - 1)
                  {
                    for (j = l; j < n; j++)
                      {
                        for (s1 = 0.0, k = i; k < m; k++)
                          {
                            s1 += a[k][i] * a[k][j];
                          }

                        f = s1 / h;

                        for (k = i; k < m; k++)
                          {
                            a[k][j] += f * a[k][i];
                          }
                      }
                  }

                for (k = i; k < m; k++)
                  {
                    a[k][i] *= scale;
                  }
              }
          }

        w[i] = scale * g;
        g = s1 = scale = 0.0;

        if (i < m && i != n - 1)
          {
            for (k = l; k < n; k++)
              {
                scale += fabs(a[i][k]);
              }

            if (scale)
              {
                for (k = l; k < n; k++)
                  {
                    a[i][k] /= scale;
                    s1 += a[i][k] * a[i][k];
                  }

                f = a[i][l];
                g = -SIGN_NR(sqrt(s1), f);
                h = f * g - s1;
                a[i][l] = f - g;

                for (k = l; k < n; k++)
                  {
                    rv1[k] = a[i][k] / h;
                  }

                if (i != m - 1)
                  {
                    for (j = l; j < m; j++)
                      {
                        for (s1 = 0.0, k = l; k < n; k++)
                          {
                            s1 += a[j][k] * a[i][k];
                          }

                        for (k = l; k < n; k++)
                          {
                            a[j][k] += s1 * rv1[k];
                          }
                      }
                  }

                for (k = l; k < n; k++)
                  {
                    a[i][k] *= scale;
                  }
              }
          }

        anorm = MAX_NR(anorm, (fabs(w[i]) + fabs(rv1[i])));
      }

    for (i = n - 1; i >= 0; i--)
      {
        if (i < n - 1)
          {
            if (g)
              {
                for (j = l; j < n; j++)
                  {
                    v[j][i] = (a[i][j] / a[i][l]) / g;
                  }

                for (j = l; j < n; j++)
                  {
                    for (s1 = 0.0, k = l; k < n; k++)
                      {
                        s1 += a[i][k] * v[k][j];
                      }

                    for (k = l; k < n; k++)
                      {
                        v[k][j] += s1 * v[k][i];
                      }
                  }
              }

            for (j = l; j < n; j++)
              {
                v[i][j] = v[j][i] = 0.0;
              }
          }

        v[i][i] = 1.0;
        g = rv1[i];
        l = i;
      }

    for (i = n - 1; i >= 0; i--)
      {
        l = i + 1;
        g = w[i];

        if (i < n - 1)
          for (j = l; j < n; j++)
            {
              a[i][j] = 0.0;
            }

        if (g)
          {
            g = 1.0 / g;

            if (i != n - 1)
              {
                for (j = l; j < n; j++)
                  {
                    for (s1 = 0.0, k = l; k < m; k++)
                      {
                        s1 += a[k][i] * a[k][j];
                      }

                    f = (s1 / a[i][i]) * g;

                    for (k = i; k < m; k++)
                      {
                        a[k][j] += f * a[k][i];
                      }
                  }
              }

            for (j = i; j < m; j++)
              {
                a[j][i] *= g;
              }
          }
        else
          {
            for (j = i; j < m; j++)
              {
                a[j][i] = 0.0;
              }
          }

        ++a[i][i];
      }

    for (k = n - 1; k >= 0; k--)
      {
        for (its = 1; its <= 30; its++)
          {
            flag = 1;

            for (l = k; l >= 0; l--)
              {
                nm = l - 1;

                if (fabs(rv1[l]) + anorm == anorm)
                  {
                    flag = 0;
                    break;
                  }

                if (fabs(w[nm]) + anorm == anorm)
                  {
                    break;
                  }
              }

            if (flag)
              {
                c = 0.0;
                s1 = 1.0;

                for (i = l; i <= k; i++)
                  {
                    f = s1 * rv1[i];

                    if (fabs(f) + anorm != anorm)
                      {
                        g = w[i];
                        h = PYTHAG(f, g);
                        w[i] = h;
                        h = 1.0 / h;
                        c = g * h;
                        s1 = (-f * h);

                        for (j = 0; j < m; j++)
                          {
                            y = a[j][nm];
                            z = a[j][i];
                            a[j][nm] = y * c + z * s1;
                            a[j][i] = z * c - y * s1;
                          }
                      }
                  }
              }

            z = w[k];

            if (l == k)
              {
                if (z < 0.0)
                  {
                    w[k] = -z;

                    for (j = 0; j < n; j++)
                      {
                        v[j][k] = (-v[j][k]);
                      }
                  }

                break;
              }

            if (its == 40)
              throw IceException(FNAME, M_NO_SOLUTION);

            x = w[l];
            nm = k - 1;
            y = w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = PYTHAG(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + SIGN_NR(g, f))) - h)) / x;
            c = s1 = 1.0;

            for (j = l; j <= nm; j++)
              {
                i = j + 1;
                g = rv1[i];
                y = w[i];
                h = s1 * g;
                g = c * g;
                z = PYTHAG(f, h);
                rv1[j] = z;
                c = f / z;
                s1 = h / z;
                f = x * c + g * s1;
                g = g * c - x * s1;
                h = y * s1;
                y = y * c;

                for (jj = 0; jj < n; jj++)
                  {
                    x = v[jj][j];
                    z = v[jj][i];
                    v[jj][j] = x * c + z * s1;
                    v[jj][i] = z * c - x * s1;
                  }

                z = PYTHAG(f, h);
                w[j] = z;

                if (z)
                  {
                    z = 1.0 / z;
                    c = f * z;
                    s1 = h * z;
                  }

                f = (c * g) + (s1 * y);
                x = (c * y) - (s1 * g);

                for (jj = 0; jj < m; jj++)
                  {
                    y = a[jj][j];
                    z = a[jj][i];
                    a[jj][j] = y * c + z * s1;
                    a[jj][i] = z * c - y * s1;
                  }
              }

            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = x;
          }
      }

    free(rv1);

    /* Umschreiben der Loesung */
    for (i = 1; i < n; i++)
      {
        (*W)->data[i][i] = (*W)->data[0][i];
        (*W)->data[0][i] = 0.0;
      }

    return OK;
  }
#undef SIGN_NR
#undef FNAME
}

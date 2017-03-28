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

#include <utility>

#include "message.h"
#include "macro.h"
#include "mateigen.h"
#include "Matrix.h"

namespace ice
{
#define ROTATE(a,i,j,k,l) g=a.at(i).at(j);h=a.at(k).at(l);  \
  a.at(i).at(j)=g-s*(h+g*tau);          \
  a.at(k).at(l)=h+s*(g-h*tau);


  void EigenvalueSort(Vector& d, Matrix& v);

#define FNAME "Eigenvalue"
  int Eigenvalue(const Matrix& A, Matrix& W, Matrix& v, double eps, int maxIt)
  {
    int rc;
    Vector w;

    rc = Eigenvalue(A, w, v);

    if (rc == OK)
      {
        W = Matrix(w.size(), w.size());

        /* Umschreiben der L"osung */
        for (unsigned int i = 0; i < w.size(); i++)
          {
            W[i][i] = w[i];
          }
      }

    return rc;
  }

  int Eigenvalue(const Matrix& A, Vector& eval, Matrix& evect, double eps, int maxIt)
  {
    Matrix Amat = A;

    int nrot;

    double tresh, theta, t, sm, s, h, g, c;

    /* Testung der Parameter */

    int n = A.cols();

    if (n != A.rows())
      {
        Message(FNAME, M_NO_SQUARE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // Matrix symmetrisieren
    for (int ip = 0; ip < n; ip++)
      for (int iq = 0; iq < n; iq++)
        Amat.at(ip).at(iq) = Amat.at(iq).at(ip) =
                               0.5 * (Amat.at(ip).at(iq) + Amat.at(iq).at(ip));

    evect = Matrix(n, n);
    eval.Resize(n);

    Vector b(n);
    Vector z(n);

    // evect mit Einheitsmatrix belegen
    for (int ip = 0; ip < n; ip++)
      {
        for (int iq = 0; iq < n; iq++)
          evect.at(ip).at(iq) = 0.0;

        evect.at(ip).at(ip) = 1.0;
      }

    for (int ip = 0; ip < n; ip++)
      {
        b.at(ip) = eval.at(ip) = Amat.at(ip).at(ip);
        z.at(ip) = 0.0;
      }

    nrot = 0;

    for (int i = 1; i <= maxIt; i++)
      {
        sm = 0.0;

        for (int ip = 0; ip < n - 1; ip++)
          {
            for (int iq = ip + 1; iq < n; iq++)
              sm += fabs(Amat.at(ip).at(iq));
          }

        //    Printf("%d: %f\n",i,sm);
        if (sm < eps)
          {
            EigenvalueSort(eval, evect);
            return OK;
          }

        if (i < 4)
          tresh = 0.2 * sm / (n * n);
        else
          tresh = 0.0;

        for (int ip = 0; ip < n - 1; ip++)
          {
            for (int iq = ip + 1; iq < n; iq++)
              {
                g = 100.0 * fabs(Amat.at(ip).at(iq));

                if (i > 4 && fabs(eval.at(ip)) + g == fabs(eval.at(ip))
                    && fabs(eval.at(iq)) + g == fabs(eval.at(iq)))
                  Amat.at(ip).at(iq) = 0.0;
                else if (fabs(Amat.at(ip).at(iq)) > tresh)
                  {
                    h = eval.at(iq) - eval.at(ip);

                    if (fabs(h) + g == fabs(h))
                      t = (Amat.at(ip).at(iq)) / h;
                    else
                      {
                        theta = 0.5 * h / (Amat.at(ip).at(iq));
                        t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));

                        if (theta < 0.0)
                          t = -t;
                      }

                    c = 1.0 / sqrt(1 + t * t);
                    s = t * c;
                    double tau = s / (1.0 + c);
                    h = t * Amat.at(ip).at(iq);
                    z[ip] -= h;
                    z[iq] += h;
                    eval[ip] -= h;
                    eval[iq] += h;
                    Amat.at(ip).at(iq) = 0.0;

                    for (int j = 0; j <= ip - 1; j++)
                      {
                        ROTATE(Amat, j, ip, j, iq)
                      }

                    for (int j = ip + 1 ; j <= iq - 1; j++)
                      {
                        ROTATE(Amat, ip, j, j, iq)
                      }

                    for (int j = iq + 1 ; j < n; j++)
                      {
                        ROTATE(Amat, ip, j, iq, j)
                      }

                    for (int j = 0; j < n; j++)
                      {
                        ROTATE(evect, j, ip, j, iq)
                      }

                    ++nrot;
                  }
              }
          }

        for (int ip = 0; ip < n; ip++)
          {
            b[ip] += z[ip];
            eval[ip] = b[ip];
            z[ip] = 0.0;
          }

        //
      }

    Message(FNAME, "Too many iterations", NO_SOLUTION);
    return ERROR;
  }
#undef ROTATE

  void rotate(matrix<double>& a, int i, int j, int k, int l, double s, double tau)
  {
    double g = a[i][j];
    double h = a[k][l];
    a[i][j] = g - s * (h + g * tau);
    a[k][l] = h + s * (g - h * tau);
  }

  void EigenvalueSort(std::vector<double>& d,
                      matrix<double>& mv,
                      std::vector<std::vector<double> >& ev)
  {
    // sort eigenvalues and convert to vector of vectors

    ev.clear();
    int n = d.size();

    for (int i = 0; i < n - 1; i++)
      {
        // max. Betrag suchen
        int idx_max = i;
        double p = fabs(d[i]);

        for (int j = i + 1; j < n; j++)
          {
            double pa = fabs(d[j]);

            if (pa > p)
              {
                p = pa;
                idx_max = j;
              }
          }

        if (idx_max != i) // Tausch nötig ?
          {
            // tauschen
            std::swap(d[idx_max], d[i]);

            // Spalten der Matrix tauschen
            for (int j = 0; j < n; j++)
              {
                std::swap(mv[j][i], mv[j][idx_max]);
              }
          }

        // eigenvektor erzeugen
        std::vector<double> v;
        for (int k = 0; k < n; ++k)
          v.push_back(mv[k][i]);
        ev.push_back(v);
      }

    std::vector<double> v;
    for (int k = 0; k < n; ++k)
      v.push_back(mv[k][n - 1]);
    ev.push_back(v);
  }

  int Eigenvalue(const matrix<double>& A, std::vector<double>& eval,
                 std::vector<std::vector<double>>& eVectors, double eps, int maxIt)
  {
    matrix<double> Amat = A;

    // requires square matrix
    int n = A.cols();
    if (n != A.rows())
      {
        Message(FNAME, M_NO_SQUARE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    // symmetrize matrix
    for (int ip = 0; ip < n; ip++)
      for (int iq = 0; iq < ip; iq++)
        Amat[ip][iq] = Amat[iq][ip] = 0.5 * (Amat[ip][iq] + Amat[iq][ip]);

    matrix<double> evect(n, n);
    eval.resize(n);

    std::vector<double> b(n);
    std::vector<double> z(n);

    // evect mit Einheitsmatrix belegen
    for (int ip = 0; ip < n; ip++)
      {
        for (int iq = 0; iq < n; iq++)
          evect[ip][iq] = 0.0;

        evect[ip][ip] = 1.0;
      }

    for (int ip = 0; ip < n; ip++)
      {
        b[ip] = eval[ip] = Amat[ip][ip];
        z[ip] = 0.0;
      }

    int nrot = 0;
    bool ready = false;

    for (int i = 0; !ready && i < maxIt; ++i)
      {
        double sm = 0.0;

        for (int ip = 0; ip < n - 1; ip++)
          {
            for (int iq = ip + 1; iq < n; iq++)
              sm += fabs(Amat[ip][iq]);
          }

        //    Printf("%d: %f\n",i,sm);
        ready = sm < eps;
        if (! ready)
          {
            double thresh = i < 4 ? 0.2 * sm / (n * n) : 0.0;

            for (int ip = 0; ip < n - 1; ++ip)
              {
                for (int iq = ip + 1; iq < n; ++iq)
                  {
                    double g = 100.0 * fabs(Amat[ip][iq]);

                    if (i > 4 && fabs(eval[ip]) + g == fabs(eval[ip])
                        && fabs(eval[iq]) + g == fabs(eval[iq]))
                      Amat[ip][iq] = 0.0;
                    else if (fabs(Amat[ip][iq]) > thresh)
                      {
                        double h = eval[iq] - eval[ip];
                        double t;
                        if (fabs(h) + g == fabs(h))
                          t = Amat[ip][iq] / h;
                        else
                          {
                            double theta = 0.5 * h / Amat[ip][iq];
                            t = 1.0 / (fabs(theta) + sqrt(1.0 + theta * theta));

                            if (theta < 0.0)
                              t = -t;
                          }

                        double c = 1.0 / sqrt(1 + t * t);
                        double s = t * c;
                        double tau = s / (1.0 + c);
                        h = t * Amat[ip][iq];
                        z[ip] -= h;
                        z[iq] += h;
                        eval[ip] -= h;
                        eval[iq] += h;
                        Amat[ip][iq] = 0.0;

                        for (int j = 0; j <= ip - 1; j++)
                          {
                            rotate(Amat, j, ip, j, iq, s, tau);
                          }

                        for (int j = ip + 1 ; j <= iq - 1; j++)
                          {
                            rotate(Amat, ip, j, j, iq, s, tau);
                          }

                        for (int j = iq + 1 ; j < n; j++)
                          {
                            rotate(Amat, ip, j, iq, j, s, tau);
                          }

                        for (int j = 0; j < n; j++)
                          {
                            rotate(evect, j, ip, j, iq, s, tau);
                          }

                        ++nrot;
                      }
                  }
              }

            for (int ip = 0; ip < n; ip++)
              {
                b[ip] += z[ip];
                eval[ip] = b[ip];
                z[ip] = 0.0;
              }

            //
          }
      }

    if (ready)
      {
        EigenvalueSort(eval, evect, eVectors);
        return OK;
      }
    else
      {
        Message(FNAME, "Too many iterations", NO_SOLUTION);
        return ERROR;
      }
  }

  /* interne Funktion: Sortierung der Eigenwerte, Eigenvektoren nach Betrag */
  void EigenvalueSort(Vector& d, Matrix& v)
  {
    //    std::cout << d << std::endl;
    //    std::cout << v << std::endl;

    int n = d.size();

    for (int i = 0; i < n - 1; i++)
      {
        // max. Betrag suchen
        int idx_max = i;
        double p = fabs(d[i]);

        for (int j = i + 1; j < n; j++)
          {
            double pa = fabs(d.at(j));

            if (pa > p)
              {
                p = pa;
                idx_max = j;
              }
          }

        if (idx_max != i) // Tausch nötig ?
          {
            // tauschen
            double p = d.at(idx_max);
            d.at(idx_max) = d.at(i);
            d.at(i) = p;

            for (int j = 0; j < n; j++)
              {
                // Spalten der Matrix tauschen
                double p = v.at(j).at(i);
                v.at(j).at(i) = v.at(j).at(idx_max);
                v.at(j).at(idx_max) = p;
              }
          }
      }
  }
#undef FNAME

#define FNAME "SingularValueDcmp"

  static inline double pythag(double a, double b)
  {
    double at, bt, ct;
    at = fabs(a);
    bt = fabs(b);

    if (at > bt)
      {
        ct = bt / at;
        return at * sqrt(1.0 + ct * ct);
      }
    else
      {
        if (bt != 0.0)
          {
            ct = at / bt;
            return bt * sqrt(1.0 + ct * ct);
          }
        else return 0.0;
      }
  }

  static inline double sign_nr(double a, double b)
  {
    if (b >= 0.0) return fabs(a);
    else return -fabs(a);
  }

  int SingularValueDcmp(const Matrix& A, Matrix& a, Vector& w, Matrix& v)
  {
    int m, n;
    int its;
    double c, f, h, s1, x, y, z;

    double anorm = 0.0, g = 0.0, scale = 0.0;

    Vector rv1;

    int l = 0;
    int nm = 0;
    /* Testung der Parameter */

    m = A.rows();
    n = A.cols();

    if (m < n)
      {
        Message(FNAME, M_MATRIXFORMAT, WRONG_PARAM);
        return WRONG_PARAM;
      }

    /* Matrizen U,W,V anlegen */
    /* Matrix U als Kopie von A */
    a = A;

    /* w - Die Singulaerwerte */
    w = Vector(n);

    /* Matrix V */
    v = Matrix(n, n);
    v.Set(0.0);

    /* der eigentliche Algorithmus */
    rv1 = Vector(n);

    for (int i = 0; i < n; i++)
      {
        l = i + 1;
        rv1.at(i) = scale * g;
        g = s1 = scale = 0.0;

        if (i < m)
          {
            for (int k = i; k < m; k++)
              scale += fabs(a.at(k).at(i));

            if (scale)
              {
                for (int k = i; k < m; k++)
                  {
                    a.at(k).at(i) /= scale;
                    s1 += a.at(k).at(i) * a.at(k).at(i);
                  }

                f = a.at(i).at(i);
                g = -sign_nr(sqrt(s1), f);
                h = f * g - s1;
                a.at(i).at(i) = f - g;

                if (i != n - 1)
                  {
                    for (int j = l; j < n; j++)
                      {
                        s1 = 0.0;

                        for (int k = i; k < m; k++)
                          s1 += a.at(k).at(i) * a.at(k).at(j);

                        f = s1 / h;

                        for (int k = i; k < m; k++)
                          a.at(k).at(j) += f * a.at(k).at(i);
                      }
                  }

                for (int k = i; k < m; k++)
                  a.at(k).at(i) *= scale;
              }
          }

        w.at(i) = scale * g;
        g = s1 = scale = 0.0;

        if (i < m && i != n - 1)
          {
            for (int k = l; k < n; k++)
              scale += fabs(a.at(i).at(k));

            if (scale)
              {
                for (int k = l; k < n; k++)
                  {
                    a.at(i).at(k) /= scale;
                    s1 += a.at(i).at(k) * a.at(i).at(k);
                  }

                f = a.at(i).at(l);
                g = -sign_nr(sqrt(s1), f);
                h = f * g - s1;
                a.at(i).at(l) = f - g;

                for (int k = l; k < n; k++)
                  rv1.at(k) = a.at(i).at(k) / h;

                if (i != m - 1)
                  {
                    for (int j = l; j < m; j++)
                      {
                        s1 = 0.0;

                        for (int k = l; k < n; k++)
                          s1 += a.at(j).at(k) * a.at(i).at(k);

                        for (int k = l; k < n; k++)
                          a.at(j).at(k) += s1 * rv1.at(k);
                      }
                  }

                for (int k = l; k < n; k++)
                  a.at(i).at(k) *= scale;
              }
          }

        anorm = Max(anorm, (fabs(w.at(i)) + fabs(rv1.at(i))));
      }

    for (int i = n - 1; i >= 0; i--)
      {
        if (i < n - 1)
          {
            if (g)
              {
                for (int j = l; j < n; j++)
                  v.at(j).at(i) = (a.at(i).at(j) / a.at(i).at(l)) / g;

                for (int j = l; j < n; j++)
                  {
                    s1 = 0.0;

                    for (int k = l; k < n; k++)
                      s1 += a.at(i).at(k) * v.at(k).at(j);

                    for (int k = l; k < n; k++)
                      v.at(k).at(j) += s1 * v.at(k).at(i);
                  }
              }

            for (int j = l; j < n; j++)
              v.at(i).at(j) = v.at(j).at(i) = 0.0;
          }

        v.at(i).at(i) = 1.0;
        g = rv1.at(i);
        l = i;
      }

    for (int i = n - 1; i >= 0; i--)
      {
        l = i + 1;
        g = w.at(i);

        if (i < n - 1)
          for (int j = l; j < n; j++)
            a.at(i).at(j) = 0.0;

        if (g != 0)
          {
            g = 1.0 / g;

            if (i != n - 1)
              {
                for (int j = l; j < n; j++)
                  {
                    s1 = 0.0;

                    for (int k = l; k < m; k++)
                      s1 += a.at(k).at(i) * a.at(k).at(j);

                    f = (s1 / a.at(i).at(i)) * g;

                    for (int k = i; k < m; k++)
                      a.at(k).at(j) += f * a.at(k).at(i);
                  }
              }

            for (int j = i; j < m; j++)
              a.at(j).at(i) *= g;
          }
        else
          {
            for (int j = i; j < m; j++)
              a.at(j).at(i) = 0.0;
          }

        ++a.at(i).at(i);
      }

    //    anorm = 1e-16;

    for (int k = n - 1; k >= 0; k--)
      {
        for (its = 1; its <= 40; its++)
          {
            bool flag = true;

            for (l = k; l >= 0; l--)
              {
                nm = l - 1;

                if (fabs(rv1.at(l)) + anorm == anorm)
                  {
                    //        cout << "++++ " ;
                    flag = false;
                    break;
                  }

                if (fabs(w.at(nm)) + anorm == anorm)
                  {
                    //        cout << "---- ";
                    break;
                  }
              }

            //      cout << l << " " << k << " " << flag << endl;

            if (flag)
              {
                c = 0.0;
                s1 = 1.0;

                for (int i = l; i <= k; i++)
                  {
                    f = s1 * rv1.at(i);

                    if (fabs(f) + anorm != anorm)
                      {
                        g = w.at(i);
                        h = pythag(f, g);
                        w.at(i) = h;
                        h = 1.0 / h;
                        c = g * h;
                        s1 = -f * h;

                        for (int j = 0; j < m; j++)
                          {
                            y = a.at(j).at(nm);
                            z = a.at(j).at(i);
                            a.at(j).at(nm) = y * c + z * s1;
                            a.at(j).at(i) = z * c - y * s1;
                          }
                      }
                  }
              }

            z = w.at(k);

            if (l == k)
              {
                //    cout << k << ": " << z << endl;
                if (z < 0.0)
                  {
                    w.at(k) = -z;

                    for (int j = 0; j < n; j++)
                      v.at(j).at(k) = (-v.at(j).at(k));
                  }

                break;
              }

            if (its == 40)
              {
                Message(FNAME, M_NO_SOLUTION, NO_SOLUTION);
                return NO_SOLUTION;
              }

            x = w.at(l);
            nm = k - 1;
            y = w.at(nm);
            g = rv1.at(nm);
            h = rv1.at(k);
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = pythag(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + sign_nr(g, f))) - h)) / x;
            c = s1 = 1.0;

            for (int j = l; j <= nm; j++)
              {
                int i = j + 1;
                g = rv1.at(i);
                y = w.at(i);
                h = s1 * g;
                g = c * g;
                z = pythag(f, h);
                rv1.at(j) = z;
                c = f / z;
                s1 = h / z;
                f = x * c + g * s1;
                g = g * c - x * s1;
                h = y * s1;
                y = y * c;

                for (int jj = 0; jj < n; jj++)
                  {
                    x = v.at(jj).at(j);
                    z = v.at(jj).at(i);
                    v.at(jj).at(j) = x * c + z * s1;
                    v.at(jj).at(i) = z * c - x * s1;
                  }

                z = pythag(f, h);
                w.at(j) = z;

                if (z)
                  {
                    z = 1.0 / z;
                    c = f * z;
                    s1 = h * z;
                  }

                f = (c * g) + (s1 * y);
                x = (c * y) - (s1 * g);

                for (int jj = 0; jj < m; jj++)
                  {
                    y = a.at(jj).at(j);
                    z = a.at(jj).at(i);
                    a.at(jj).at(j) = y * c + z * s1;
                    a.at(jj).at(i) = z * c - y * s1;
                  }
              }

            rv1.at(l) = 0.0;
            rv1.at(k) = f;
            w.at(k) = x;
          }
      }

    return OK;
  }

  int SingularValueDcmp(const Matrix& A, Matrix& a, Matrix& W, Matrix& v)
  {
    int rc;
    Vector w;

    rc = SingularValueDcmp(A, a, w, v);

    if (rc == OK)
      {
        W = Matrix(w.size(), w.size());

        /* Umschreiben der L"osung */
        for (unsigned int i = 0; i < w.size(); i++)
          {
            W.at(i).at(i) = w.at(i);
          }
      }

    return rc;
  }

#undef FNAME
}

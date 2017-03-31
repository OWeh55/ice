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
// **************************************************************************
// Lösung des Zuordnungsproblems mit der U n g a r i s c h e n Methode:
// Gegeben m Quellenpunkte und n Zielpunkte
// sowie eine Kostenmatrix, die  Kosten enthält.
// Kosten werden jeder Kante von einem Quellenpunkt zu einem Zielpunkt
// zugeordnet. Sind keine Kosten sinnvoll, so ist eine Null zuzuordnen.
// Es wird die optimale Zuordnung mit minimaler Kostensumme
// nach der "ungarischen Methode" bestimmt
// Es werden höchstens min(m,n) Zuordnungspaare ermittelt !!!!
// Wurden weniger als min(m,n) Zuordnungspaare ermittelt,
// dann können die restlichen Paare frei zugeordnet werden,
// da es für die restlichen Paare keine eindeutige Zuordnung gibt !!
//
// RC: 0       Alles O.K.
// RC: 1       Alles O.K., aber Referenzpaare < min(m,n)
// *************************************************************************


#include <limits.h>
#include <limits>

#include <stdio.h>
#include <malloc.h>

#include "defs.h"
#include "message.h"
#include "assignment.h"
#include "numbase.h"
#include "Matrix.h"

using namespace std;

namespace ice
{
  typedef int TNum;

#define INFTY 0x7fffffffL
  typedef enum {True, False} TBoolean;

  struct SVertexLeft
  {
    struct SEdge* head;
    struct SVertexRight* mate;
    TNum u;
    TBoolean f;
  };

  typedef struct SVertexLeft TVertexLeft;

  struct SVertexRight
  {
    struct SVertexLeft* mate;
    TNum v;
    TBoolean f;
    TNum pi;
    struct SVertexLeft* parent;
  };

  typedef struct SVertexRight TVertexRight;

  struct SEdge
  {
    struct SVertexRight* right;
    struct SEdge* next;
    TNum weight;
  };
  typedef struct SEdge TEdge;

  static void matching(int m, TVertexLeft* s, int n, TVertexRight* t, TNum w);
  static int input_graph(int m, int n, int k, int* indizes, TVertexLeft** ps, TVertexRight** pt, TEdge** pe, TNum* pw);

#define FNAME "Hungarian"
  int Hungarian(const Matrix& cost, IMatrix& reference_pairs, double& min_cost, int mode)
  {
    int m; // Zeilen der Kostenmatrix
    int n; // Spalten der Kostenmatrix
    int anz; // Anzahl der Elemente der Kostenmatrix
    double max; // Maximale Kosten

    TVertexLeft* s;
    TVertexRight* t;
    TEdge* e;
    TNum w;

    //    int i,j;
    int* indices;
    int anz_pairs; // Anzahl der ermittelten Referenzpaare
    int rc; // Rueckkehrkode
    m = cost.rows();
    n = cost.cols();

    if ((m <= 0) || (n <= 0))
      {
        Message(FNAME, M_WRONG_MATRIX, WRONG_MATRIX);
        return WRONG_MATRIX;
      }

    anz = m * n;

    indices = new int[3 * anz];

    if (indices == nullptr)
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return ERROR;
      }

    if (mode == 0)
      {
        max = 0;

        for (int i = 0; i < m; ++i)
          for (int j = 0; j < n; ++j)
            {
              double c = cost.at(i).at(j);

              if (c < 0.0)
                {
                  Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
                  return WRONG_PARAM;
                }

              if (c > max)
                {
                  max = c;
                }
            }


        double fak = ((double)INT_MAX) / max / 1000;

        int k = 0;

        for (int i = 0; i < m; i++)
          for (int j = 0; j < n; j++)
            {
              indices[k] = i + 1;
              indices[k + 1] = j + 1;
              indices[k + 2] = RoundInt((max - cost.at(i).at(j)) * fak);
              k = k + 3;
            }
      }
    else if (mode == 1)
      {
        double minc = numeric_limits<double>::max(), minc2 = minc, maxc = 0;

        for (int i = 0; i < (int)cost.rows(); i++)
          {
            for (int j = 0; j < (int)cost.cols(); j++)
              {
                if (cost[i][j] > maxc)
                  {
                    maxc = cost[i][j];
                  }

                if (cost[i][j] < minc)
                  {
                    minc2 = minc;
                    minc = cost[i][j];
                  }

                if (cost[i][j] > minc && cost[i][j] < minc2)
                  {
                    minc2 = cost[i][j];
                  }
              }
          }

        double new_maxval = numeric_limits<int>::max() / 1000.0;

        double max_cost = new_maxval * (minc2 - minc);

        double fak = new_maxval / max_cost;


        int k = 0;

        for (int i = 0; i < (int)cost.rows(); i++)
          {
            for (int j = 0; j < (int)cost.cols(); j++)
              {
                double cost_rescaled = (cost[i][j] - minc) * fak;

                if (cost_rescaled > new_maxval)
                  {
                    cost_rescaled = new_maxval;
                  }

                indices[k] = i + 1;
                indices[k + 1] = j + 1;
                indices[k + 2] = RoundInt(cost_rescaled);
                k = k + 3;
              }
          }
      }

    rc = input_graph(m, n, anz, indices, &s, &t, &e, &w);

    if (rc == 1)
      {
        Message(FNAME, M_NO_MEM, NO_MEM);
        return ERROR;
      }

    matching(m, s, n, t, w);

    anz_pairs = 0;

    for (int i = 0; i < m; i++)
      {
        if (s[i].mate != nullptr)
          {
            anz_pairs++;
          }
      }

    reference_pairs = IMatrix(anz_pairs, 2); // Anlegen der Referenzpaare-Matrix

    min_cost = 0;

    int k = 0;

    for (int i = 0; i < m; i++)
      {
        if (s[i].mate != nullptr)
          {
            int i2 = s[i].mate - t;
//  min_cost +=max-((s[i].u + s[i].mate -> v)/fak);
            min_cost += cost.at(i).at(i2);
            reference_pairs[k][0] = i;
            reference_pairs[k][1] = i2;
            k++;
          }
      }

    delete [] indices;
    free(s);
    free(t);
    free(e);

    if (Min(m, n) < anz_pairs)
      {
        return 1;
      }

    return 0;
  }

  int Hungarian(const Matrix& cost, IMatrix& reference_pairs, int mode)
  {
    double dummy;
    return Hungarian(cost, reference_pairs, dummy, mode);
  }

#undef FNAME

// ************************************************************************
  int input_graph(int m, int n, int k,
                  int* indizes, TVertexLeft** ps, TVertexRight** pt,
                  TEdge** pe, TNum* pw)
  {
    int  kk, i, a, b;
    TNum v, w;
    TVertexLeft* s;
    TVertexRight* t;
    TEdge* e;

    s = (TVertexLeft*)malloc(m * sizeof(TVertexLeft));

    if (s == nullptr)
      {
        return 1;
      }

    t = (TVertexRight*)malloc(n * sizeof(TVertexRight));

    if (t == nullptr)
      {
        return 1;
      }

    e = (TEdge*)malloc(k * sizeof(TEdge));

    if (e == nullptr)
      {
        return 1;
      }

    for (i = 0; i < m; ++i)
      {
        s[i].head = nullptr;
      }

    w = 0;
    kk = 0;

    for (i = 0; i < k; ++i)
      {
        a = indizes[kk];
        b = indizes[kk + 1];
        v = indizes[kk + 2];
        --a;
        --b;
        e[i].right = t + b;
        e[i].next = s[a].head;
        s[a].head = e + i;
        e[i].weight = v;

        if (v > w)
          {
            w = v;
          }

        kk = kk + 3;
      }

    *ps = s;
    *pt = t;
    *pe = e;
    *pw = w;
    return 0;
  }
// ******************************************************************
// *******************************************************************
  void matching(int m, TVertexLeft* s, int n, TVertexRight* t, TNum w)
  {
    int i, j;
    TNum d1, d2, z;
    TEdge* p;
    TVertexLeft* a;
    TVertexRight* b, *c;
    struct SStack
    {
      void* v;
      enum {Left, Right} lr;
    } *st;
    int sp;

#define INSERT(x, y) { st[sp].v = x; st[sp].lr = y; ++sp; }

    for (i = 0; i < m; ++i)
      {
        s[i].mate = nullptr;
      }

    for (j = 0; j < n; ++j)
      {
        t[j].mate = nullptr;
      }

    for (i = 0; i < m; ++i)
      {
        s[i].u = w;
      }

    for (j = 0; j < n; ++j)
      {
        t[j].v = 0;
      }

    st = (struct SStack*)malloc((m + n) * sizeof(struct SStack));

Label_A:

    sp = 0;

    for (i = 0; i < m; ++i)
      {
        if (s[i].mate == nullptr)
          {
            s[i].f = True;
            INSERT(&s[i], SStack::Left);
          }
        else
          {
            s[i].f = False;
          }
      }

    for (j = 0; j < n; ++j)
      {
        t[j].f = False;
        t[j].pi = + INFTY;
      }

Label_B:

    while (sp > 0)
      {
        --sp;

        if (st[sp].lr == SStack::Left)
          {
            a = (TVertexLeft*) st[sp].v;

            for (p = a -> head; p != nullptr; p = p -> next)
              {
                b = p -> right;

                if ((b -> f) == False)
                  {
                    z = (a -> u) + (b -> v) - (p -> weight);

                    if (z < (b -> pi))
                      {
                        b -> pi = z;
                        b -> parent = a;

                        if (z == 0)
                          {
                            b -> f = True;
                            INSERT(b, SStack:: Right);
                          }
                      }
                  }
              }
          }
        else
          {
            b = (TVertexRight*) st[sp].v;

            if (b -> mate != nullptr)
              {
                a = b -> mate;

                if (a -> f == False)
                  {
                    a -> f = True;
                    INSERT(a, SStack:: Left);
                  }
              }
            else
              {
                a = b -> parent;
                b -> mate = a;

                while (a -> mate != nullptr)
                  {
                    c = a -> mate;
                    a -> mate = b;
                    b = c;
                    a = b -> parent;
                    b -> mate = a;
                  }

                a -> mate = b;
                goto Label_A;
              }
          }
      }

    z = + INFTY;

    for (i = 0; i < m; ++i)
      if (s[i].mate == nullptr)
        {
          z = s[i].u;
          break;
        }

    d1 = z;
    z = + INFTY;

    for (j = 0; j < n; ++j)
      if (t[j].f == False && t[j].pi < z)
        {
          z = t[j].pi;
        }

    d2 = z;

    if (d1 <= d2)
      {
        z = d1;
      }
    else
      {
        z = d2;
      }

    for (i = 0; i < m; ++i) if (s[i].f == True)
        {
          s[i].u -= z;
        }

    for (j = 0; j < n; ++j) if (t[j].f == True)
        {
          t[j].v += z;
        }

    if (d1 <= d2)
      {
        free(st);
        return;
      }

    for (j = 0; j < n; ++j) if (t[j].f == False && t[j].pi < + INFTY)
        {
          t[j].pi -= z;

          if (t[j].pi == 0)
            {
              t[j].f = True;
              INSERT(&t[j], SStack:: Right);
            }
        }

    goto Label_B;
#undef INSERT
  }
}

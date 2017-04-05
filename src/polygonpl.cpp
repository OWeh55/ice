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
//   functions for the work with polygons with PointLists
//
//   (C) m.schubert 1/96
//       Ortmann 10/99, 10/06, 02/09
//
//   All new functions should use datatype Matrix as list of points, not
//   the type PointList

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "macro.h"
#include "numbase.h"
#include "analygeo.h"
#include "fit.h"
#include "drawline.h"
#include "Contur.h"
#include "polygonim.h"
#include "simplex.h"
#include "distance.h"

namespace ice
{

  /******************************************************/
  /***interne Datenstruktur (verkettete Punktliste)******/
  /******************************************************/
  typedef struct p_clip_
  {
    double p[2];
    int valid;
    struct p_clip_* prev, *next;
  }* p_clip;
  /******************************************************/

  inline void first(p_clip& p)
  {
    while (p->prev != NULL)
      {
        p = p->prev;
      }
  }

  inline p_clip next(p_clip& p)
  {
    p_clip h = p;

    if (h->next != NULL)
      {
        return h->next;
      }

    first(p);
    return p;
  }

  /******************************************************/
  /***interne Funktionen*********************************/
  /******************************************************/

// Hilfsfunktion für insidePolygon
  bool handleOneEdge(double xp, double yp,
                     double x1, double y1, double x2, double y2,
                     int& lastdir, int& c)
  {
    if (yp == y1)
      {
        if (xp == x1)
          {
            return true;
          }

        if (y1 > y2)
          {
            if (lastdir == -1) // weiter fallend
              if (x1 < xp)
                {
                  c++;
                }
          }

        if (y1 < y2)
          {
            if (lastdir == 1) // weiter steigend
              if (x1 < xp)
                {
                  c++;
                }
          }

        if (y1 == y2)
          {
            if ((x1 <= xp) && (x2 >= xp))
              {
                return true;
              }
          }
      }

    if ((y1 > yp && y2 < yp) || (y1 < yp && y2 > yp))
      {
        double xz = (yp - y1) * (x2 - x1) / (y2 - y1) + x1;

        /* Punkt auf dem Polygonzug? */
        if (xz == xp)
          {
            return true;
          }

        /* Polygonzug schneidet Scanlinie links vom Punkt? */
        if (xz < xp)
          {
            c++;
          }
      }

    if (y2 > y1)
      {
        lastdir = 1;
      }

    if (y2 < y1)
      {
        lastdir = -1;
      }

    return false;
  }

  int p_PointInPolygon(double p[2], p_clip cl)
  {
    p_clip c0, c1;
    int c = 0;
    int lastdir = 0;

    first(cl);

    for (c0 = cl, c1 = next(cl); c0 != NULL; c0 = c0->next, c1 = next(c1))
      {
        if (c0->p[1] < c1->p[1])
          {
            lastdir = 1;
          }

        if (c0->p[1] > c1->p[1])
          {
            lastdir = -1;
          }
      }

    for (c0 = cl, c1 = next(cl); c0 != NULL; c0 = c0->next, c1 = next(c1))
      {
        if (handleOneEdge(p[0], p[1],
                          c0->p[0], c0->p[1], c1->p[0], c1->p[1],
                          lastdir, c))
          {
            return 0;
          }

      }

    if ((c & 1) == 1)
      {
        return 1;
      }

    return -1;
  }
#undef FNAME
  /******************************************************/
  /* neuen Listenpunkt erstellen*/
  p_clip p_New(double x, double y)
  {
    p_clip c;
    c = (p_clip)malloc(sizeof(struct p_clip_));
    c->p[0] = x;
    c->p[1] = y;
    c->valid = true;
    c->prev = nullptr;
    c->next = nullptr;
    return c;
  }
  /******************************************************/
  /* Listenpunkt hinter akt. Punkt einfügen */
  void p_Insert(p_clip c, p_clip ci)
  {
    ci->prev = c;
    ci->next = c->next;
    c->next = ci;

    if (ci->next != nullptr)
      {
        ci->next->prev = ci;
      }

    return;
  }
  /******************************************************/
  /* Listenpunkt löschen */
  p_clip p_Delete(p_clip cd)
  {
    p_clip c;

    /* Rueckgabe des im Zyklus folgenden Punktes*/
    if (cd == nullptr)
      {
        return nullptr;
      }

    if (cd->next != nullptr)
      {
        c = cd->next;
      }
    else
      {
        c = cd;
        first(c);

        if (c == cd)
          {
            c = nullptr;
          }
      }

    if (cd->prev != nullptr)
      {
        cd->prev->next = cd->next;
      }

    if (cd->next != nullptr)
      {
        cd->next->prev = cd->prev;
      }

    free(cd);
    return c;
  }
  /******************************************************/
  void p_Free(p_clip cl)
  {
    while (cl != nullptr)
      {
        cl = p_Delete(cl);
      }
  }
  /******************************************************/
  /* verkettete Liste aus Punktliste erzeugen*/
  p_clip p_ClipList(PointList pl)
  {
    p_clip c0, c;
    int i, j;
    double a;

    /* Polygonfläche berechnen */
    for (i = 0, j = 1, a = 0; i < pl->lng; i++, j++)
      {
        if (j >= pl->lng)
          {
            j = 0;
          }

        a += pl->xptr[i] * pl->yptr[j] - pl->xptr[j] * pl->yptr[i];
      }

    c = c0 = p_New(pl->xptr[0], pl->yptr[0]);

    /* Fläche positiv: Umlaufsinn beibehalten! */
    if (a > 0)
      {
        for (i = 1; i < pl->lng; i++)
          {
            p_Insert(c, p_New(pl->xptr[i], pl->yptr[i]));
            c = c->next;
          }

        return c0;
      }

    /* Flaeche negativ: Umlaufsinn umkehren */
    for (i = pl->lng - 1; i > 0; i--)
      {
        p_Insert(c, p_New(pl->xptr[i], pl->yptr[i]));
        c = c->next;
      }

    return c0;
  }
  /******************************************************/
  /* naechsten Schnittpunkt mit Polygon suchen */
  p_clip p_SearchCross(p_clip c, p_clip cl)
  {
    double l, lmin = 1;
    p_clip cn, c0, c1, cr = nullptr;
    first(cl);

    if (c->next != nullptr)
      {
        cn = c->next;

        if (!cn->valid)
          {
            return nullptr;
          }
      }
    else
      {
        cn = c;
        first(cl);
      }

    for (c0 = cl, c1 = next(cl); c0 != nullptr; c0 = c0->next, c1 = next(c1))
      {
        if (Intersect(c->p, cn->p, c0->p, c1->p, &l))
          {
            if (l < lmin)
              {
                lmin = l;
                cr = c0;
              }
          }
      }

    if (lmin < 1)
      {
        p_Insert(c, p_New(c->p[0] + lmin * (cn->p[0] - c->p[0]),
                          c->p[1] + lmin * (cn->p[1] - c->p[1])));
        p_Insert(cr, p_New(c->p[0] + lmin * (cn->p[0] - c->p[0]),
                           c->p[1] + lmin * (cn->p[1] - c->p[1])));
      }

    return cr;
  }
  /******************************************************/
  /* Polygone schneiden */
  p_clip p_ClipPolygon(p_clip c1, p_clip c2)
  {
    p_clip c, ca, cs, cr, ch;
    int h;

    first(c1);
    first(c2);

    ca = c1;

    while ((!ca->valid) && (ca->next != nullptr))
      {
        ca = ca->next;
      }

    if (ca->next == nullptr)
      {
        return nullptr;
      }

    /* erster Punkt im 2. Polygon? --> Startpunkt für neue Liste */
    if ((h = p_PointInPolygon(ca->p, c2)) >= 0)
      {
        cs = p_New(ca->p[0], ca->p[1]);
      }
    /* sonst ersten Schnittpunkt (von außen nach innen) suchen --> Startpunkt */
    else
      {
        for (ca = c1; ca != nullptr; ca = ca->next)
          if ((cr = p_SearchCross(ca, c2)) != nullptr)
            {
              break;
            }

        /* Polygon 1 vollst. außerhalb von Polygon 2  (aber nicht notw. umgekehrt) */
        if (ca == nullptr)
          {
            return nullptr;
          }

        ca->valid = false;
        ca = ca->next;
        cs = p_New(ca->p[0], ca->p[1]);
      }

    cr = p_SearchCross(ca, c2);

    if (cr != nullptr)
      {
        /* nach einem Schnittpunkt wechseln inneres
           und aeusseres Polygon */
        ca->valid = false;
        ca->next->valid = false;
        ca = cr;
        ch = c2;
        c2 = c1;
        c1 = ch;
      }

    /* c2 ist das aeussere Polygon
       ca ist ein Pkt. auf d. inneren Polygon */
    ca->valid = false;
    ca = ca->next;
    c = cs;

    if (ca == nullptr)
      {
        /* Fehler: Polygon 1 ist nur 1 Punkt */
        free(cs);
        return nullptr;
      }

    /* suchen, bis Startpunkt wieder erreicht ist */
    while (fabs(ca->p[0] - cs->p[0]) > EPSILON
           || fabs(ca->p[1] - cs->p[1]) > EPSILON)
      {
        /* naechsten Punkt in neue Liste einfügen */
        p_Insert(c, p_New(ca->p[0], ca->p[1]));
        c = c->next;
        cr = p_SearchCross(ca, c2);
        ca->valid = false;

        if (cr != nullptr)
          {
            /* nach einem Schnittpunkt wechseln
               inneres und aeusseres Polygon */
            ca->next->valid = false;
            ca = cr;
            ch = c2;
            c2 = c1;
            c1 = ch;
          }

        /* naechsten Punkt des inneren Polygons */
        ca = next(ca);
      }

    return cs;
  }
  /******************************************************/
  /* Punktliste aus verketteter Liste erzeugen */
  PointList p_PointList(p_clip c)
  {
    PointList pl;
    int i;
    p_clip c1;
    first(c);
    c1 = c;
    i = 0;

    while (c->next != nullptr)
      {
        c = c->next;
        i++;
      }

    c = c1;
    pl = NewPointList(i + 1);

    if (pl == nullptr)
      {
        return (nullptr);
      }

    for (i = 0; i < pl->lng; i++)
      {
        PutPoint(pl, i, c->p[0], c->p[1], 1.0);
        c = c->next;
      }

    return pl;
  }
  /******************************************************/
  bool p_IsPolygon(p_clip cl)
  {
    /* Es wird getestet, ob sich die Kanten eines Polygons schneiden */
    p_clip c1, c2, c3;
    double l;
    first(cl);

    for (c1 = cl; c1->next != nullptr; c1 = c1->next)
      {
        for (c2 = c1->next; c2 != nullptr; c2 = c2->next)
          {
            c3 = c2->next;

            if (c3 == nullptr)
              {
                c3 = cl;
              }

            if (Intersect(c1->p, c1->next->p, c2->p, c3->p, &l))
              {
                return false;
              }
          }
      }

    return true;
  }
  /******************************************************/
  /***oeffentliche Funktionen****************************/
  /******************************************************/
#define FNAME "ClipPolygon"
  PointList* ClipPolygon(PointList pl1, PointList pl2, int* i)
  {
    PointList* pl;
    p_clip c1, c2, c;
    *i = 0;
    pl = nullptr;

    if ((pl1 == nullptr) || (pl2 == nullptr))
      throw IceException(FNAME, M_WRONG_PARAM);

    if ((pl1->lng < 3) || (pl2->lng < 3))
      throw IceException(FNAME, M_WRONG_PARAM);

    c1 = p_ClipList(pl1);
    c2 = p_ClipList(pl2);

    /*Teilpolygone von c1 in c2 suchen und umgekehrt*/
    while ((c = p_ClipPolygon(c1, c2)) != nullptr)
      {
        if (*i == 0)
          {
            pl = (PointList*)malloc(sizeof(PointList));
          }
        else
          {
            pl = (PointList*)realloc(pl, (*i + 1) * sizeof(PointList));
          }

        if (pl == nullptr)
          {
            return nullptr;
          }

        pl[*i] = p_PointList(c);
        p_Free(c);
        (*i)++;
      }

    while ((c = p_ClipPolygon(c2, c1)) != nullptr)
      {
        if (*i == 0)
          {
            pl = (PointList*)malloc(sizeof(PointList));
          }
        else
          {
            pl = (PointList*)realloc(pl, (*i + 1) * sizeof(PointList));
          }

        if (pl == nullptr)
          {
            return nullptr;
          }

        pl[*i] = p_PointList(c);
        p_Free(c);
        (*i)++;
      }

    p_Free(c1);
    p_Free(c2);
    return pl;
  }
#undef FNAME
  /******************************************************/
#define FNAME "isPolygon"
  bool isPolygon(PointList pl)
  {
    /* Es wird getestet, ob sich die Kanten eines Polygons schneiden */
    p_clip cl;
    bool flag;

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (pl->lng < 2)
      {
        return false;
      }

    cl = p_ClipList(pl);
    flag = p_IsPolygon(cl);
    p_Free(cl);
    return flag;
  }
#undef FNAME

  /******************************************************/
#define FNAME "insidePolygon"
  bool insidePolygon(double p[2], PointList pl)
  {
    int i, j, c = 0;
    double xp = p[0];
    double yp = p[1];
    int lastdir = 0;

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PARAM);

    if (pl->lng < 2)
      throw IceException(FNAME, M_WRONG_PARAM);

    for (i = 0, j = 1; i < pl->lng; i++, j++)
      {
        if (j >= pl->lng)
          {
            j = 0;
          }

        double y1 = pl->yptr[i];
        double y2 = pl->yptr[j];

        if (y2 > y1)
          {
            lastdir = 1;
          }

        if (y2 < y1)
          {
            lastdir = -1;
          }
      }

    if (lastdir == 0)
      {
        // degenerated polygon
        return false;
      }

    for (i = 0, j = 1; i < pl->lng; i++, j++)
      {
        if (j >= pl->lng)
          {
            j = 0;
          }

        if (handleOneEdge(xp, yp,
                          pl->xptr[i], pl->yptr[i], pl->xptr[j], pl->yptr[j],
                          lastdir, c))
          {
            return false;
          }
      }

    if (c & 1)
      {
        return true;
      }

    return false;
  }

#undef FNAME
  /******************************************************/

#define FNAME "PolygonContur"
  Contur PolygonContur(PointList pl)
  {
    int i;
    Contur c;

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PARAM);

    c.SetStart(RoundInt(pl->xptr[0]), RoundInt(pl->yptr[0]));

    for (i = 1; i < pl->lng; i++)
      {
        c.Add(RoundInt(pl->xptr[i]), RoundInt(pl->yptr[i]));
      }

    c.Add(RoundInt(pl->xptr[0]), RoundInt(pl->yptr[0]));
    return c;
  }

#undef FNAME
#define FNAME "DrawPolygon"
  void DrawPolygon(PointList pl, int val, Image& img)
  {
    int i, j;

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    if (pl == nullptr)
      throw IceException(FNAME, M_WRONG_PARAM);

    for (i = 0, j = 1; i < pl->lng; i++, j++)
      {
        if (j >= pl->lng)
          {
            j = 0;
          }

        Line(RoundInt(pl->xptr[i]), RoundInt(pl->yptr[i]),
             RoundInt(pl->xptr[j]), RoundInt(pl->yptr[j]),
             val, 0, img);
      }

    return;
  }
#undef FNAME
}

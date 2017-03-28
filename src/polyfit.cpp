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

/************************************************
 * Kontursegmentierung und Anpassung von        *
 * Liniensegmenten                              *
 * (Die Eckpunkte des Polygonzuges werden durch *
 * rekursive Teilung der Kurve bestimmt)        *
 *                                              *
 *   (C) m.schubert 5.95                        *
 ************************************************/

#include <stdlib.h>
#include <math.h>

#include "macro.h"
#include "message.h"
#include "fit_pointlist.h"
#include "Contur.h"
#include "contools.h"
#include "analygeo.h"
#include "drawline.h"
#include "paint.h"

namespace ice
{
  typedef struct pfit_Corner_
  {
    PointList pl;
    int adr;
    struct pfit_Corner_* prev, *next;
  } pfit_Corner;
  /************************************************/

  int pfit_Split(PointList pl, pfit_Corner* cr1, pfit_Corner* cr2, double mdist, int mlng);

  /************************************************/
  pfit_Corner* pfit_NewCorner(PointList pl, int adr)
  {
    pfit_Corner* c;
    c = (pfit_Corner*)malloc(sizeof(pfit_Corner));
    c->pl = pl;
    c->adr = adr;
    c->prev = c->next = nullptr;
    return (c);
  }
  /************************************************/
  void pfit_InsertCorner(pfit_Corner* cr, pfit_Corner* crp)
  {
    cr->prev = crp;
    cr->next = crp->next;
    cr->prev->next = cr;
    cr->next->prev = cr;
    return;
  }
  /************************************************/
  void pfit_DeleteCorner(pfit_Corner* cr)
  {
    if (cr->prev != nullptr) cr->prev->next = cr->next;

    if (cr->next != nullptr) cr->next->prev = cr->prev;

    free(cr);
    return;
  }
  /************************************************/
  void pfit_FreeCornerList(pfit_Corner* crl)
  {
    pfit_Corner* crf;

    if (crl == nullptr) return;

    while (crl->next != nullptr) crl = crl->next;

    while (crl != nullptr)
      {
        crf = crl;
        crl = crl->prev;
        free(crf);
      }

    return;
  }
  /************************************************/
  void pfit_DrawSeg(Segment segl, int val, Image imgm)
  {
    Segment seg;
    int x1, y1, x2, y2;
    seg = segl;

    while (seg != nullptr)
      {
        x1 = (int)(seg->p0[0]);
        y1 = (int)(seg->p0[1]);
        x2 = (int)(seg->p1[0]);
        y2 = (int)(seg->p1[1]);
        Line(x1, y1, x2, y2, val, 0, imgm);
        Marker(2, x1, y1, 2, 5, imgm);
        Marker(2, x2, y2, 2, 5, imgm);
        seg = seg->next;
      }
  }
  /************************************************/
  void pfit_DrawCorners(pfit_Corner* crl, int val, Image imgm)
  {
    int x, y;

    if (crl == nullptr) return;

    while (crl->prev != nullptr) crl = crl->prev;

    while (crl != nullptr)
      {
        x = (int)(crl->pl->xptr[crl->adr]);
        y = (int)(crl->pl->yptr[crl->adr]);
        Marker(2, x, y, val, 5, imgm);
        crl = crl->next;
      }

    return;
  }
  /************************************************/
  void pfit_MergeCorners(pfit_Corner* crl, int dist)
  {
    int adn, flg;

    while (crl->prev != nullptr) crl = crl->prev;

    while (crl != nullptr)
      {
        flg = true;

        while (crl->next != nullptr && flg)
          {
            if ((adn = crl->next->adr - crl->adr) < dist)
              {
                crl->adr += adn / 2;
                pfit_DeleteCorner(crl->next);
              }
            else flg = false;
          }

        crl = crl->next;
      }
  }
  /************************************************/
  pfit_Corner* pfit_MergeSegments(pfit_Corner* crl, double dist, int closed)
  {
    double d, d1, d2, d3, dmax, cc, ss, *x, *y;
    double p1[2], p2[2], par[2];
    PointList pl;
    pfit_Corner* cre, *cr1, *cr2, *cr3;
    int a1, a2, a3, a;
    cre = nullptr;
    dmax = 1.5 * dist;
    pl = crl->pl;
    x = pl->xptr;
    y = pl->yptr;

    if (closed)
      {
        /*letzter Eckpunkt*/
        cre = crl;

        while (cre->next != nullptr) cre = cre->next;
      }

    /*erster Eckpunkt*/
    cr2 = crl;

    while (cr2->prev != nullptr) cr2 = cr2->prev;

    if (cr2->next == nullptr)return (cr2);

    if (!closed) cr2 = cr2->next;

    while (cr2->next != nullptr)
      {
        if (cr2->prev == nullptr) cr1 = cre;
        else cr1 = cr2->prev;

        cr3 = cr2->next;
        a1 = cr1->adr;
        a2 = cr2->adr;
        a3 = cr3->adr;
        d = sqrt(Sqr(x[a3] - x[a1]) + Sqr(y[a3] - y[a1]));

        if (d > 0.5)
          d = fabs((x[a2] - x[a1]) * (y[a3] - y[a2]) - (x[a3] - x[a2]) * (y[a2] - y[a1])) / 2 / d;
        else d = dmax + 1;

        if (d < dmax)
          {
            d1 = Sqr(x[a1] - x[a2]) + Sqr(y[a1] - y[a2]);
            d2 = Sqr(x[a2] - x[a3]) + Sqr(y[a2] - y[a3]);
            d3 = Sqr(x[a3] - x[a1]) + Sqr(y[a3] - y[a1]);

            if (d3 > d1 && d3 > d2)
              {
                FitLine(pl, a1, a2, 0, par, &d, &a);
                cc = cos(par[1]);
                ss = sin(par[1]);
                d = par[0] - cc * x[a2] - ss * y[a2];
                p1[0] = x[a2] + d * cc;
                p1[1] = y[a2] + d * ss;
                FitLine(pl, a2, a3, 0, par, &d, &a);
                cc = cos(par[1]);
                ss = sin(par[1]);
                d = par[0] - cc * x[a2] - ss * y[a2];
                p2[0] = x[a2] + d * cc;
                p2[1] = y[a2] + d * ss;
                FitLine(pl, a1, a3, 0, par, &d, &a);
                cc = cos(par[1]);
                ss = sin(par[1]);
                d1 = fabs(par[0] - cc * p1[0] - ss * p1[1]);
                d2 = fabs(par[0] - cc * p2[0] - ss * p2[1]);

                if (d1 < dist && d2 < dist) pfit_DeleteCorner(cr2);
              }
          }

        cr2 = cr3;
      }

    while (cr2->prev != nullptr) cr2 = cr2->prev;

    return (cr2);
  }
  /************************************************/
  int pfit_Split(PointList pl, pfit_Corner* cr1, pfit_Corner* cr2, double mdist, int mlng)
  {
    double p1[2], p2[2];
    double p, phi, cc, ss;
    pfit_Corner* cr3;
    int i, a1, a2, admax = 0;
    double d, dmax = 0;

    if ((cr1 == nullptr) || (cr2 == nullptr)) return (ERROR);

    a1 = cr1->adr;
    a2 = cr2->adr;

    if (a1 == a2) return (ERROR);

    if ((a2 - a1) < mlng) return (OK);

    p1[0] = (double)pl->xptr[a1];
    p1[1] = (double)pl->yptr[a1];
    p2[0] = (double)pl->xptr[a2];
    p2[1] = (double)pl->yptr[a2];
    ConvPointHesse(p1, p2, &p, &phi);
    cc = cos(phi);
    ss = sin(phi);

    for (i = a1 + 1; i < a2; i++)
      {
        d = fabs((double)pl->xptr[i] * cc + (double)pl->yptr[i] * ss - p);

        if (d > dmax)
          {
            dmax = d;
            admax = i;
          }
      }

    if (dmax >= mdist)                        /*Ecke gefunden*/
      {
        cr3 = pfit_NewCorner(pl, admax);

        if (cr3 != nullptr)
          {
            pfit_InsertCorner(cr3, cr1);

            if (pfit_Split(pl, cr1, cr3, mdist, mlng) != OK) return (ERROR);

            if (pfit_Split(pl, cr3, cr2, mdist, mlng) != OK) return (ERROR);

            return (OK);
          }
      }

    return (OK);
  }
  /************************************************/
  int pfit_InsertSeg(Segment seg, double p1[2], double p2[2])
  {
    Segment segn;
    int rc;

    if (seg == nullptr) return (ERROR);

    segn = (Segment)malloc(sizeof(struct Segment_));
    OffMessage();
    rc = ConvPointHesse(p1, p2, &segn->par[0], &segn->par[1]);
    OnMessage();

    if (rc != OK)
      {
        free(segn);
        return (ERROR);
      }

    segn->prev = seg;
    segn->next = seg->next;
    seg->next = segn;

    if (segn->next != nullptr) segn->next->prev = segn;

    segn->typ = 1;
    segn->p0[0] = p1[0];
    segn->p0[1] = p1[1];
    segn->p1[0] = p2[0];
    segn->p1[1] = p2[1];
    return (OK);
  }
  /************************************************/
  int pfit_ConnectSeg(Segment seg1, Segment seg2, double x, double y)
  {
    double c1, s1, c2, s2, xs, ys, det, d;
    c1 = cos(seg1->par[1]);
    s1 = sin(seg1->par[1]);
    c2 = cos(seg2->par[1]);
    s2 = sin(seg2->par[1]);
    det = c1 * s2 - c2 * s1;

    /*keine parallelen Segmente schneiden*/
    if (fabs(det) > 0.1)
      {
        xs = (s2 * seg1->par[0] - s1 * seg2->par[0]) / det;
        ys = (c1 * seg2->par[0] - c2 * seg1->par[0]) / det;

        /*Schnittpunkt soll Mindesabstand nicht überschreiten*/
        if ((Sqr(x - xs) + Sqr(y - ys)) < 25)
          {
            seg1->p1[0] = seg2->p0[0] = xs;
            seg1->p1[1] = seg2->p0[1] = ys;
            return (OK);
          }
      }

    d = seg1->par[0] - c1 * x - s1 * y;
    seg1->p1[0] = x + c1 * d;
    seg1->p1[1] = y + s1 * d;
    d = seg2->par[0] - c2 * x - s2 * y;
    seg2->p0[0] = x + c2 * d;
    seg2->p0[1] = y + s2 * d;
    pfit_InsertSeg(seg1, seg1->p1, seg2->p0);
    return (OK);
  }
  /************************************************/
  Segment pfit_Fit(pfit_Corner* crl, int closed)
  {
    double par[2], d, c0, s0, c1, s1;
    int a0, a1, a2;
    Segment segl, seg, seg1, seg2;
    PointList pl;
    int a;

    if (crl == nullptr) return (nullptr);

    segl = nullptr;
    seg = nullptr;
    pl = crl->pl;

    /*zum Listenanfang*/
    while (crl->prev != nullptr) crl = crl->prev;

    a0 = crl->adr;

    /*Geraden an Konturstücke anpassen*/
    while (crl->next != nullptr)
      {
        a1 = crl->adr;
        a2 = crl->next->adr;

        if (a1 != a2)
          {
            FitLine(pl, a1, a2, 0, par, &d, &a);
            seg = (Segment)malloc(sizeof(struct Segment_));
            seg->prev = seg->next = nullptr;
            seg->typ = 1;
            seg->par[0] = par[0];
            seg->par[1] = par[1];
            segl = AddSegment(segl, seg);

            if (seg->prev != nullptr)
              {
                /*Schnittpunkte aufeinanderfolgender Geraden bestimmen*/
                pfit_ConnectSeg(seg->prev, seg, pl->xptr[a1], pl->yptr[a1]);
              }
          }

        crl = crl->next;
      }

    seg1 = seg2 = FirstSegment(segl);

    if (seg1 == nullptr) return (nullptr);

    if (closed)
      {
        /*Sonderbehandlung für geschlossene Konturen*/
        a1 = crl->adr;
        FitLine(pl, a1, a0, 0, par, &d, &a);
        seg = (Segment)malloc(sizeof(struct Segment_));
        seg->prev = seg->next = nullptr;
        seg->typ = 1;
        seg->par[0] = par[0];
        seg->par[1] = par[1];
        segl = AddSegment(segl, seg);

        if (seg->prev != nullptr)
          {
            /*Schnittpunkte aufeinanderfolgender Geraden bestimmen*/
            pfit_ConnectSeg(seg->prev, seg, pl->xptr[a1], pl->yptr[a1]);
          }

        pfit_ConnectSeg(seg, seg2, pl->xptr[a0], pl->yptr[a0]);
      }

    segl = seg1;

    if (!closed)
      {
        /*Sonderbehandlung für Ende*/
        a1 = pl->lng - 1;
        c1 = cos(seg->par[1]);
        s1 = sin(seg->par[1]);
        d = seg->par[0] - pl->xptr[a1] * c1 - pl->yptr[a1] * s1;
        seg->p1[0] = pl->xptr[a1] + d * c1;
        seg->p1[1] = pl->yptr[a1] + d * s1;
        /*Sonderbehandlung für Anfang*/
        c0 = cos(seg1->par[1]);
        s0 = sin(seg1->par[1]);
        d = seg1->par[0] - pl->xptr[0] * c0 - pl->yptr[0] * s0;
        seg1->p0[0] = pl->xptr[0] + d * c0;
        seg1->p0[1] = pl->yptr[0] + d * s0;
      }

    return (segl);
  }
  /************************************************/
#define FNAME "LineSegPointList"
  Segment LineSegPointList(PointList pl, int closed, int mlng, double mdist)
  {
    pfit_Corner* cr1, *cr2, *cr3;
    Segment segl = nullptr;
    int i, admax = 0;
    double d, dmax = 0;

    if (pl == nullptr)
      {
        Message(FNAME, M_WRONG_PTR, WRONG_POINTER);
        return (nullptr);
      }

    cr1 = pfit_NewCorner(pl, 0);
    cr2 = pfit_NewCorner(pl, pl->lng - 1);
    cr1->next = cr2;
    cr2->prev = cr1;

    if (closed)
      {
        for (i = 1; i < cr2->adr; i++)
          {
            d = Sqr(pl->xptr[i] - pl->xptr[0]) + Sqr(pl->yptr[i] - pl->yptr[0]);

            if (d > dmax)
              {
                dmax = d;
                admax = i;
              }
          }

        cr3 = pfit_NewCorner(pl, admax);
        pfit_InsertCorner(cr3, cr1);

        if (pfit_Split(pl, cr1, cr3, mdist, mlng) != OK) return (nullptr);

        if (pfit_Split(pl, cr3, cr2, mdist, mlng) != OK) return (nullptr);

        pfit_DeleteCorner(cr2);   /*identisch mit cr1*/
      }
    else
      {
        if (pfit_Split(pl, cr1, cr2, mdist, mlng) != OK)
          {
            return (nullptr);
          }
      }

    pfit_MergeCorners(cr1, mlng);
    cr1 = cr2 = pfit_MergeSegments(cr1, mdist, closed);

    for (i = 1; cr2->next != nullptr; i++, cr2 = cr2->next);

    if (i == 1)
      {
        pfit_FreeCornerList(cr1);
        return (nullptr);
      }

    segl = pfit_Fit(cr1, closed);
    pfit_FreeCornerList(cr1);
    return (segl);
  }
#undef FNAME
  /************************************************/
#define FNAME "LineSegContur"
  Segment LineSegContur(Contur c, int mlng, double mdist)
  {
    PointList pl;
    Segment segl;
    pl = ConturPointList(c, 1);
    segl = LineSegPointList(pl, c.isClosed(), mlng, mdist);
    FreePointList(pl);
    return segl;
  }
#undef FNAME
//----------------------------------------------
#define FNAME "FitPolygonPointList"
  PointList FitPolygonPointList(PointList pl, int closed, int mlng, double mdist)
  {
    Segment segl, seg, seg1;
    PointList plf;
    int i;
    seg1 = nullptr;
    segl = LineSegPointList(pl, closed, mlng, mdist);
    seg = segl;
    i = 0;

    while (seg != nullptr)
      {
        i++;
        seg = seg->next;
      }

    if (i == 0)
      {
        Message(FNAME, "Can't fit polygon", ERROR);
        return nullptr;
      }

    if (!closed) i++;

    plf = NewPointList(i);
    seg = segl;
    i = 0;

    while (seg != nullptr)
      {
        seg1 = seg;
        PutPoint(plf, i, seg->p0[0], seg->p0[1], 1.0);
        i++;
        seg = seg->next;
      }

    if (!closed) PutPoint(plf, i, seg1->p1[0], seg1->p1[1], 1.0);

    FreeSegmentList(segl);
    return (plf);
  }
#undef FNAME
//---------------------------------------------------------
#define FNAME "FitPolygonContur"
  PointList FitPolygonContur(Contur c, int mlng, double mdist)
  {
    PointList pl, plf;
    pl = ConturPointList(c, 1);
    plf = FitPolygonPointList(pl, c.isClosed(), mlng, mdist);
    FreePointList(pl);
    return plf;
  }
#undef FNAME
}

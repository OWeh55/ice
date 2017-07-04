#include <cassert>
#include "reduced_poly.h"

// helper class reduced_poly

namespace ice
{
  /**
   * Fehler der reduzierten Kante ii1..ii2 und Punkt mit maximalen Abstand.
   * Berechnet den max. Abstand der Zwischenpunkte des
   * Originalpolygons von der Kante ii1 - ii2 der
   * reduzierten PolygonalCurve
   * maxind gibt index des Punktes mit max. Abstand zurück
   */
  double reduced_poly::maxdistedge(int ii1, int ii2, int& maxind) const
  {
    int i1 = idx[ii1]; // Indizes in Original-PolygonalCurve
    int i2 = idx[ii2];
//    std::cout << idx[ii1] << " - " << idx[ii2] << std::endl;
//    std::cout << i1 << " - " << i2 << std::endl;
    LineSeg edge(base[i1], base[i2]); // "reduzierte" Kante

    double emax = 0;

    maxind = i1;

    for (int i = i1 + 1; i < i2; i++)   // alle zwischenpunkte
      {
        double d = edge.Distance(base[i]);

        if (d > emax)
          {
            emax = d;
            maxind = i;
          }
      }

    return emax;
  }

  /**
   * Fehler der reduzierten Kante ii1..ii2.
   */
  double reduced_poly::maxdistedge(int ii1, int ii2) const
  {
    int dummy;
    return maxdistedge(ii1, ii2, dummy);
  }

  /**
   * aktualisiert Fehlermass für die Kante i..i+1.
   */
  void reduced_poly::updatedist1(int i)
  {
    if (i + 1 < (int)idx.size() && i >= 0)
      {
        double dist = maxdistedge(i, i + 1, maxi1[i]);
        maxdist1[i] = dist;

        if (dist > maxdist)
          {
            maxdist = dist;
          }
      }
  }

  void reduced_poly::updatedist2(int i)
  {
    if (i + 2 < (int)idx.size() && i >= 0)
      {
        maxdist2[i] = maxdistedge(i, i + 2);
      }
  }

  /**
   * initiale Distanzberechnung.
   * Beschreibt die felder maxdist1, maxi1 und maxdist2
   * reduzierte Kurve muss mindestens 2 Punkte beinhalten
   */
  void reduced_poly::init_distances()
  {
    maxdist = 0.0;
    int last1 = idx.size() - 1;
    int last2 = idx.size() - 2;

    maxdist1.resize(last1);
    maxi1.resize(last1);
    maxdist2.resize(last2);

    for (unsigned int i = 0; i < maxdist1.size(); i++)
      {
        updatedist1(i);
      }

    for (unsigned int i = 0; i < maxdist2.size(); i++)
      {
        updatedist2(i);
      }
  }

  /**
   * update maxdist
   */
  void reduced_poly::recalcmaxdist()
  {
    maxdist = 0.0;

    for (unsigned int i = 0; i < maxdist1.size(); i++)
      if (maxdist1[i] > maxdist)
        {
          maxdist = maxdist1[i];
        }
  }

  /**
   * insert new corner into reduced polygonalcurve = split
   */
  void reduced_poly::insertcorner(int pos, int baseindex)
  {
    // inserts new corner _after_ pos
    int ipos = pos + 1;
    if (ipos >= (int)idx.size())
      {
        idx.push_back(baseindex);
      }
    else
      {
        idx.insert(idx.begin() + ipos, baseindex);
      }

    if (ipos >= (int) maxdist1.size())
      {
        maxdist1.push_back(0.0);
        maxi1.push_back(0);
      }
    else
      {
        maxdist1.insert(maxdist1.begin() + ipos, 0);
        maxi1.insert(maxi1.begin() + ipos, 0);
      }

    if (ipos >= (int)maxdist2.size())
      {
        maxdist2.push_back(0.0);
      }
    else
      {
        maxdist2.insert(maxdist2.begin() + ipos, 0);
      }

    updatedist1(pos);
    updatedist1(pos + 1);

    updatedist2(pos - 1);
    updatedist2(pos);
    updatedist2(pos + 1);

    // distances normally decrease here
    // we need to determine new maximum
    recalcmaxdist();
  }

  /**
   * delete corner from reduced polygonalcurve = merge
   */

  void reduced_poly::deletecorner(int pos)
  {
    assert(pos > 0);
    assert(pos < (int)idx.size() - 1);

    idx.erase(idx.begin() + pos);

    maxdist1.erase(maxdist1.begin() + pos);
    maxi1.erase(maxi1.begin() + pos);

    if (pos < (int)maxdist2.size())
      {
        maxdist2.erase(maxdist2.begin() + pos);
      }
    else
      {
        maxdist2.pop_back();
      }

    updatedist1(pos - 1);

    updatedist2(pos - 1);
    updatedist2(pos - 2);
    // since distances increase here updatedist1 will update maxdist
  }

  reduced_poly::reduced_poly(const std::vector<Point>& basep): base(basep), maxdist(0.0) {}

  reduced_poly::reduced_poly(const std::vector<Point>& basep,
                             initmode_t initmode):
    base(basep), maxdist(0.0)
  {
    nbasepoint = base.size();

    // std::cout << "Basepoints: " << base.size() << std::endl;

    switch (initmode)
      {
      case all:

        for (int i = 0; i < nbasepoint; i++)
          {
            idx.push_back(i);
          }

        break;
      case two:
        idx.push_back(0);
        idx.push_back(nbasepoint - 1); // Endpunkt
        break;
      }

    init_distances();
  }

  void reduced_poly::split()
  {
    // search segment to split
    double maxdist = 0;
    unsigned int maxind = 0;
    unsigned int maxedge = 0;
    unsigned int last = idx.size() - 1;

    for (unsigned int i = 0; i < last; i++)
      {
        if (maxdist1[i] > maxdist)
          {
            maxdist = maxdist1[i];
            maxind  = maxi1[i];
            maxedge = i;
          }
      }

    // split now
    insertcorner(maxedge, maxind);
  }

  void reduced_poly::merge()
  {
    // search segments to merge
    unsigned int minedge = 0;
    double mindist = maxdist2[0];
    for (unsigned int i = 0; i < maxdist2.size(); i++)
      {
        if (maxdist2[i] < mindist)
          {
            mindist = maxdist2[i];
            minedge = i;
          }
      }

    deletecorner(minedge + 1);
  }

  void reduced_poly::getPolygonalCurve(PolygonalCurve& poly) const
  {
    /*
    for (unsigned int i = 0; i < maxdist2.size(); i++)
      std::cout << i << ":: " << maxdist2[i] << std::endl;
    */

    poly.Reset(base[idx[0]]); // first point
    // following points
    for (unsigned int i = 1; i < idx.size(); i++)
      {
        int ib = idx[i];
        poly.add(base[ib]);
      }
  }

  /*
   * class reduced_poly_cyclic
   */

  double reduced_poly_cyclic::maxdistedge(int ii1, int ii2, int& maxind) const
  {
    int i1 = idx[reducednormal(ii1)]; // Indizes in Original-PolygonalCurve
    int i2 = idx[reducednormal(ii2)];

    if (i2 < i1)
      {
        i2 += base.size();
      }
    /*
    if (idx.size()<8)
      {
      std::cout << reducednormal(ii1) << " - " << reducednormal(ii2) << " (" << idx.size() << ")" << std::endl;
      std::cout << idx[reducednormal(ii1)] << " - " << idx[reducednormal(ii2)] << " (" << base.size() << ")" << std::endl;
      std::cout << i1 << " - " << i2 << std::endl;
      }
    */
    LineSeg edge(base[i1], base[basenormal(i2)]); // "reduzierte" Kante

    double emax = 0;

    maxind = i1;

    for (int i = i1 + 1; i < i2; i++)   // alle zwischenpunkte
      {
        int in = basenormal(i);
        double d = edge.Distance(base[in]);

        if (d > emax)
          {
            emax = d;
            maxind = in;
          }
      }

    return emax;
  }

  /**
   * Fehler der reduzierten Kante ii1..ii2.
   */
  double reduced_poly_cyclic::maxdistedge(int ii1, int ii2) const
  {
    int dummy;
    return maxdistedge(ii1, ii2, dummy);
  }

  /**
   * aktualisiert Fehlermass für die Kante i..i+1.
   */
  void reduced_poly_cyclic::updatedist1(int i)
  {
    i = reducednormal(i);
    double dist = maxdistedge(i, i + 1, maxi1[i]);
    // std::cout << "distance: " << dist << std::endl;
    maxdist1[i] = dist;
    if (dist > maxdist)
      {
        maxdist = dist;
      }
  }

  void reduced_poly_cyclic::updatedist2(int i)
  {
    i = reducednormal(i);
    maxdist2[i] = maxdistedge(i, i + 2);
  }

  /**
   * initiale Distanzberechnung.
   * Beschreibt die felder maxdist1, maxi1 und maxdist2
   * reduzierte Kurve muss mindestens 2 Punkte beinhalten
   */
  void reduced_poly_cyclic::init_distances()
  {
    maxdist = 0.0;
    int n = idx.size();

    maxdist1.resize(n);
    maxi1.resize(n);
    maxdist2.resize(n);

    for (unsigned int i = 0; i < maxdist1.size(); i++)
      {
        updatedist1(i);
      }

    for (unsigned int i = 0; i < maxdist2.size(); i++)
      {
        updatedist2(i);
      }
  }

  /**
   * insert new corner into reduced polygonalcurve = split
   */
  void reduced_poly_cyclic::insertcorner(int pos, int baseindex)
  {
    // inserts new corner _after_ pos
    pos = reducednormal(pos);

    if (pos == (int)idx.size() - 1)
      {
        idx.push_back(baseindex);

        maxdist1.push_back(0.0);
        maxi1.push_back(0);

        maxdist2.push_back(0.0);
      }
    else
      {
        idx.insert(idx.begin() + pos + 1, baseindex);
        maxdist1.insert(maxdist1.begin() + pos + 1, 0);
        maxi1.insert(maxi1.begin() + pos + 1, 0);
        maxdist2.insert(maxdist2.begin() + pos + 1, 0);
      }

    updatedist2(pos - 1);

    updatedist1(pos);
    updatedist2(pos);

    updatedist1(pos + 1);
    updatedist2(pos + 1);

    // distances normally decrease here
    // we need to determine new maximum
    recalcmaxdist();
  }

  /**
   * delete corner from reduced polygonalcurve = merge
   */

  void reduced_poly_cyclic::deletecorner(int pos)
  {
    pos = reducednormal(pos);

    idx.erase(idx.begin() + pos);
    maxdist1.erase(maxdist1.begin() + pos);
    maxi1.erase(maxi1.begin() + pos);
    maxdist2.erase(maxdist2.begin() + pos);

    updatedist1(pos - 1);

    updatedist2(pos - 1);
    updatedist2(pos - 2);
    // since distances increase here updatedist1 will update maxdist
  }

  reduced_poly_cyclic::reduced_poly_cyclic(const std::vector<Point>& basep,
      initmode_t initmode):
    reduced_poly(basep)
  {
    nbasepoint = base.size();

    // std::cout << "Basepoints: " << base.size() << std::endl;

    switch (initmode)
      {
      case all:
        for (int i = 0; i < nbasepoint; i++)
          {
            idx.push_back(i);
          }

        break;

      case two:
        idx.push_back(0);
        idx.push_back(nbasepoint / 2); // mittlerer Punkt
        break;
      }

    init_distances();
  }

  void reduced_poly_cyclic::split()
  {
    // search segment to split
    double maxdist = 0;
    unsigned int maxind = 0;
    unsigned int maxedge = 0;

    for (unsigned int i = 0; i < maxdist1.size(); i++)
      {
        if (maxdist1[i] > maxdist)
          {
            maxdist = maxdist1[i];
            maxind  = maxi1[i];
            maxedge = i;
          }
      }

    // split now
    insertcorner(maxedge, maxind);
  }

  void reduced_poly_cyclic::merge()
  {
    // search segments to merge
    unsigned int minedge = 0;
    double mindist = maxdist2[0];

    for (unsigned int i = 0; i < maxdist2.size(); i++)
      {
        // std::cout << i << ": " << maxdist2[i] << std::endl;
        if (maxdist2[i] < mindist)
          {
            mindist = maxdist2[i];
            minedge = i;
          }
      }
    //    std::cout << "merge: " << minedge << std::endl;
    deletecorner(minedge + 1);
  }

} // namespace

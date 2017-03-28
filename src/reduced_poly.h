#ifndef REDUCED_POLY_H
#define REDUCED_POLY_H

#include <vector>
#include "Point.h"
#include "polygonalcurve.h"

namespace ice
{
  // helper class
  class reduced_poly
  {
    // describes a polygonalcurve which consists of a subset of corners of
    // a polygonalcurve

  public:
    enum initmode_t {all, two};

  protected:
    const std::vector<Point>& base;  // base polygon

    int nbasepoint; // number of base corners

    std::vector<int> idx; // indices of subset corners

    std::vector<double> maxdist1; // maximal "distance" of an edge of red. poly
    std::vector<int> maxi1; // point with maximal distance to an edge
    double maxdist; // maximal maximal distance

    std::vector<double> maxdist2; // maximal "distance" of two combined edges

    /**
     * Fehler der reduzierten Kante ii1..ii2 und Punkt mit maximalen Abstand.
     * Berechnet den max. Abstand der Zwischenpunkte des
     * Originalpolygons von der Kante ii1 - ii2 der
     * reduzierten PolygonalCurve
     * maxind gibt index des Punktes mit max. Abstand zurück
     */

    virtual double maxdistedge(int ii1, int ii2, int& maxind) const;

    /**
     * Fehler der reduzierten Kante ii1..ii2.
     */
    virtual double maxdistedge(int ii1, int ii2) const;

    /**
     * aktualisiert Fehlermass für die Kante i..i+1.
     */
    virtual void updatedist1(int i);

    virtual void updatedist2(int i);

    /**
     * initiale Distanzberechnung.
     * Beschreibt die felder maxdist1, maxi1 und maxdist2
     * reduzierte Kurve muss mindestens 2 Punkte beinhalten
     */
    virtual void init_distances();

    /**
     * update maxdist
     */
    virtual void recalcmaxdist();

    /**
     * insert new corner into reduced polygonalcurve = split
     */
    virtual void insertcorner(int pos, int baseindex);

    /**
     * delete corner from reduced polygonalcurve = merge
     */
    virtual void deletecorner(int pos);

  public:
    reduced_poly(const std::vector<Point>& basep);
    reduced_poly(const std::vector<Point>& basep, initmode_t initmode);
    virtual ~reduced_poly() {};

    virtual double precision() const
    {
      return maxdist;
    }

    virtual int size() const
    {
      return idx.size();
    }

    virtual void split();
    virtual void merge();

    virtual void getPolygonalCurve(PolygonalCurve& poly) const;
  };

  // helper class
  class reduced_poly_cyclic: public reduced_poly
  {
    // describes a closed polygonalcurve which consists of a subset of corners of
    // a closed polygonalcurve

    /**
     * Fehler der reduzierten Kante ii1..ii2 und Punkt mit maximalen Abstand.
     * Berechnet den max. Abstand der Zwischenpunkte des
     * Originalpolygons von der Kante ii1 - ii2 der
     * reduzierten PolygonalCurve
     * maxind gibt index des Punktes mit max. Abstand zurück
     */

    virtual double maxdistedge(int ii1, int ii2, int& maxind) const;

    /**
     * Fehler der reduzierten Kante ii1..ii2.
     */
    virtual double maxdistedge(int ii1, int ii2) const;

    /**
     * nomalisierung des index i in std::vector base
     */
    int basenormal(int i) const
    {
      // std::cout << "* " << i << " " << nbasepoint << std::endl;
      return ((i % nbasepoint) + nbasepoint) % nbasepoint;
    }

    /**
     * normalisierung des index i derart, dass er im Intervall
     * [refi,refi+nbasepoint) liegt
     */
    int basenormal2(int i, int refi) const
    {
      return basenormal(i - refi) + refi;
    }

    /**
     * nomalisierung des index i im std::vector idx
     */
    int reducednormal(int i) const
    {
      int m = idx.size();
      return (i % m + m) % m;
    }

    /**
     * aktualisiert Fehlermass für die Kante i..i+1.
     */
    virtual void updatedist1(int i);

    virtual void updatedist2(int i);

    /**
     * initiale Distanzberechnung.
     * Beschreibt die felder maxdist1, maxi1 und maxdist2
     * reduzierte Kurve muss mindestens 2 Punkte beinhalten
     */
    virtual void init_distances();

    /**
     * insert new corner into reduced polygonalcurve = split
     */
    virtual void insertcorner(int pos, int baseindex);

    /**
     * delete corner from reduced polygonalcurve = merge
     */
    virtual void deletecorner(int pos);

  public:
    reduced_poly_cyclic(const std::vector<Point>& basep,
                        initmode_t initmode);
    virtual ~reduced_poly_cyclic() {};

    virtual void split();
    virtual void merge();

  };
} // namespace
#endif

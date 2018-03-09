/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2017 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef ICE_ACCUMULATOR2_H
#define ICE_ACCUMULATOR2_H

#include <vector>

#include "defs.h"

#include "based.h"
#include "Matrix.h"

namespace ice
{
  /**
   * accumulation.
   * Accumulator2 accumulates two dimensional values and
   * returns the most frequently used value pair.
   * Accumulator uses an histogram and therefore quantizes the
   * given values.
   */

  class Accumulator2
  {
  public:
    /**
     * default constructor.
     * before use this Accumulator2 must be configured with setDim.
     */
    Accumulator2() {}
    /**
     * constructor with full initialization.
     * The parameters must be given for both values of the pair.
     * @see setDim
     */
    Accumulator2(int n1, double min1, double max1, bool mod1, int dn1,
                 int n2, double min2, double max2, bool mod2, int dn2);

    /**
     * set parameters for one value of the pair.
     * setDim must be called two times with i==0 and i==1
     * @param i  select between first (i==0) and second (i==1) value
     * @param nBins number of bins (resolution)
     * @param min, max range of values to analyze.
     *     values outside [min,max] are ignored.
     * @param cyclic should values outside [min,max] treaded cyclic.
     * @param dn spread to bins [n - dn, n + dn]
     */
    void setDim(int i, int nBins, double min, double max,
                bool cyclic = false, int dn = 5);
    /**
      * reset all counters.
      */
    virtual void clear();

    /**
     * insert pair of values into Accumulator
     */
    virtual void add(double v1, double v2);
    /**
      * insert pair given as Point into Accumulator
      */
    virtual void add(const Point& p);
    /**
     * insert pai given as Vector into Accumulator
     */
    virtual void add(const Vector& v);

    /**
     * get most frequently used value pair as Point.
     */
    virtual Point getMax() const;
    /**
     * get most frequently used value pair as Point.
     * @param val returns a measure for frequency of pair
     */
    virtual Point getMax(double& val) const;
    /**
     * get most frequently used value pair.
     * @param x,y return value pair
     */
    virtual void getMax(double& x, double& y) const;
    /**
     * get most frequently used value pair.
     * @param x,y return value pair
     * @param val returns a measure for frequency of pair
     */
    virtual void getMax(double& x, double& y, double& val) const;

    /**
     * destructor
     */
    virtual ~Accumulator2() {}
  private:
    matrix<double> data; /**< 2-dim counters*/

    // parameters first dimension
    int nBins1 = -1;
    double min1;
    double dif1;
    bool mod1;
    int dn1;

    // parameters second dimension
    int nBins2 = -1;
    double min2;
    double dif2;
    bool mod2;
    int dn2;

    double weightOfSingleEntry;
    int nValues;

    // current maximum
    int xMax, yMax;
    double maxFrequency;

    void init(); /**< initialisation of internal variables*/
    void inc(unsigned int x, unsigned int y, double val); /**< increment of one cell*/

  };
}
#endif

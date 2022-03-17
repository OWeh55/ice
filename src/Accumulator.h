/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
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
#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <vector>

namespace ice
{
  /**
   * accumulation.
   * Accumulator accumulates scalar (one dimensional) values and
   * returns the most frequently used value.
   * Accumulator uses an histogram and therefore quantizes the
   * given values.
   */
  class Accumulator
  {
  public:
    /**
     * default c'tor.
     * contructed Accumulator is not usable yet. It needs to be
     * initialized with setDim
     */
    Accumulator(): nBins(-1) {}

    /**
     * Constructor with given values.
     * @param nBins number of bins (resolution)
     * @param min, max range of values to analyze. values outside [min,max] are ignored.
     * @param cyclic should values outside [min,max] treaded cyclic
     * @param dn spread to bins [n - dn, n + dn]
     */
    Accumulator(int nBins, double min, double max, bool cyclic = false, int dn = 1);

    /**
     * setting the parameters for Accumulator
     * @param nBins number of bins (resolution)
     * @param min, max range of values to analyze. values outside [min,max] are ignored.
     * @param cyclic should values outside [min,max] treaded cyclic
     * @param dn spread to bins [n - dn, n + dn]
     */
    void set(int nBins, double min, double max, bool cyclic = false, int dn = 1);

    /**
     * reset all counters.
     */
    virtual void clear();

    /**
     * insert value into Accumulator
     */
    virtual void add(double v);

    /**
     * get most frequently used value.
     */
    virtual double getMax() const;
    /**
     * get most frequently used value.
     * @param x variable to store value.
     */
    virtual void getMax(double& x) const;

    /**
     * get most frequently used value.
     * @param x variable to store value.
     * @param val variable to store frequence measure.
     */
    virtual void getMax(double& x, double& val) const;

    /**
     * d'tor
     */
    virtual ~Accumulator() {};

  private:
    std::vector<double> data;

    // number of bins
    int nBins;

    // range of values (min..min+n*dif)
    double min;
    double dif;

    // cyclic?
    bool mod;
    // spreading range
    int dn;

    //
    int nValues;
    double weightOfSingleEntry;
    // current maximum
    int xMax; // bin
    double maxFrequency; // max value

    /**
     * set internal variables
     */

    virtual void init();
    virtual void inc(unsigned int x, double val); // increment of one cell
  };
}
#endif

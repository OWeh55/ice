/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2015 FSU Jena, Digital Image Processing Group
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
#ifndef FOURIERTRAFO_H
#define FOURIERTRAFO_H

#include "VWorker.h"

#include <vector>

//#define NOFFTW3

#ifndef NOFFTW3
#include <fftw3.h>
#endif

namespace ice
{
  class FourierTrafo: public VWorker
  {
  public:
    FourierTrafo(int size, bool forwardP = true, bool centered = false):
      VWorker(0), forward(true), // real setting in setParameter
      centered(centered)
    {
      setParameter(size, forwardP);
    }

    FourierTrafo(): forward(true), centered(false) {}

    virtual ~FourierTrafo()
    {
#ifndef NOFFTW3
      fftw_destroy_plan(fftw_p);
      fftw_free(in);
      fftw_free(out);
#endif
    }

    FourierTrafo(const FourierTrafo& ft) = delete;

    const FourierTrafo& operator=(const FourierTrafo& ft) = delete;

    void setParameter(int newSize, bool newForward = true);

    void getResultFromReal(std::vector<double>& destinationReal,
                           std::vector<double>& destinationImag) const;

    void getResultFromImag(std::vector<double>& destinationReal,
                           std::vector<double>& destinationImag) const;

    // simplified calls for special cases
    static void transform(const std::vector<double>& src,
                          std::vector<double>& dstre,
                          std::vector<double>& dstim,
                          bool forward = true);

    static void transform(const std::vector<double>& srcre,
                          const std::vector<double>& srcim,
                          std::vector<double>& dstre,
                          std::vector<double>& dstim,
                          bool forward = true);

  private:
    virtual void transform() const;

    // transformation frequency to index in vector
    int getIndexFromFrequency(int f) const
    {
      return (centered ? (f + size / 2) : (f + size)) % size;
    }

  private:

    bool forward = true;
    bool centered = false;
    double norm;
    mutable int oldsize = 0;
    mutable bool oldforward;

#ifndef NOFFTW3
    mutable fftw_complex* in;
    mutable fftw_complex* out;
    mutable fftw_plan fftw_p;
#else
    void makeSinCosTab() const;
    void FFT() const;
    void FT() const;
    void doBitReversal() const;

    mutable std::vector<double> sinTab;
    mutable std::vector<double> cosTab;
#endif
  };
}
#endif

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
  /**
   * worker class for one dimensional Fourier transform.
   *
   */
  class FourierTrafo: public VWorker
  {
  public:
    /**
     * c'tor with given parameters.
     * @param size size of vectors
     * @param forwardP forward transform
     * @param centered origin is center of vector (range -n/2 .. n/2)
     */
    FourierTrafo(int size, bool forwardP = true, bool centered = false):
      VWorker(0), forward(true), // real setting in setParameter()
      centered(centered)
    {
      setParameter(size, forwardP);
    }

    /**
     * default c'tor.
     * parameters are set later\n
     * size is derived from vector size in setInput
     * @see setParameter
     */
    FourierTrafo(): forward(true), centered(false) {}

    /**
     *  d'tor
     */
    virtual ~FourierTrafo()
    {
#ifndef NOFFTW3
      fftw_destroy_plan(fftw_p);
      fftw_free(inp);
      fftw_free(out);
#endif
    }

    /**
     * copy constructor is deleted.
     */
    FourierTrafo(const FourierTrafo& ft) = delete;

    /**
     * assignment operator is deleted.
     */
    const FourierTrafo& operator=(const FourierTrafo& ft) = delete;

    /**
     * set parameters.
     * if parameters are changed, result becomes invalid\n
     * parameters not specified remain unchanged\n
     * @param newSize size of vectors
     * @param forward forward transform
     * @param centered origin is center of vector (range -n/2 .. n/2)
     */
    void setParameter(int newSize);
    void setParameter(int newSize, bool forward);
    void setParameter(int newSize, bool forward, bool centered);
    /**
     * @name transform of two real valued functions
     * if two real valued functions are set as real and imaginary part
     * of input only one transform is necessary to generate the transformed
     * functions.
     */
    ///@{
    /**
     * get transformed function of (real valued) real part.
     * if two real valued functions are used as real and imaginary part
     * of input, the method returns the transformed function of real part.
     * @param resultReal real part of result
     * @param resultImag imaginary part of result
     */
    void getResultFromReal(std::vector<double>& resultReal,
                           std::vector<double>& resultImag) const;

    /**
     * get transformed function of (real valued) imaginary part.
     * if two real valued functions are used as real and imaginary part
     * of input, the method returns the transformed function of imaginary part.
     * @param resultReal real part of result
     * @param resultImag imaginary part of result
     */
    void getResultFromImag(std::vector<double>& resultReal,
                           std::vector<double>& resultImag) const;
    ///@}

    /**
     * @name simplified calls for special cases
     * input and output vectors are given directly as parameters.
     */
    ///@{
    /**
     * transforms real valued function.
     * @param src real valued function as vector<double>
     * @param resultReal real part of result
     * @param resultImag imaginary part of result
     * @param forward forward transform
     * @param centered origin is center of vector (range -n/2 .. n/2)
     */
    static void transform(const std::vector<double>& src,
                          std::vector<double>& resultReal,
                          std::vector<double>& resultImag,
                          bool forward = true, bool centered = false);

    /**
     * transforms a complex valued function.
     * @param srcReal real part of function as vector<double>
     * @param srcImag imaginary part of function as vector<double>
     * @param resultReal real part of result
     * @param resultImag imaginary part of result
     * @param forward forward transform
     * @param centered origin is center of vector (range -n/2 .. n/2)
     */
    static void transform(const std::vector<double>& srcReal,
                          const std::vector<double>& srcImag,
                          std::vector<double>& resultReal,
                          std::vector<double>& resultImag,
                          bool forward = true,  bool centered = false);
    ///@}
  private:
    /**
     * fourier transformation of data.
     * transforms vectors input[0] and input[1] to vectors result[0] and
     * result[1].
     */
    virtual void transform() const;

    // transformation frequency to index in vector
    /**
     * get index for given frequency.
     * depends on setting of centered
     * @param f frequency
     */
    int getIndexFromFrequency(int f) const
    {
      return (centered ? (f + size / 2) : (f + size)) % size;
    }

    bool forward = true;
    bool centered = false;
    double norm;
    mutable int oldsize = 0;
    mutable bool oldforward;

#ifndef NOFFTW3
    mutable fftw_complex* inp = nullptr;
    mutable fftw_complex* out = nullptr;
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

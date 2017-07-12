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

#include <vector>
#include "matrixtemplate.h"

//#define NOFFTW3

#ifndef NOFFTW3
#include <fftw3.h>
#endif

namespace ice
{
  class FourierTrafo final
  {
  public:
    FourierTrafo(int size, bool forward = true, bool centered = false):
      size(0), forward(true),     // the correct values will be set in setParameter
      centered(centered), state(sNull)
    {
      setParameter(size, forward);
    }

    FourierTrafo(): size(0), centered(false), state(sNull) {}

    ~FourierTrafo()
    {
      setParameter(0); // call with zero only frees data
    }

    FourierTrafo(const FourierTrafo& ft) = delete;

    const FourierTrafo& operator=(const FourierTrafo& ft) = delete;

    void setParameter(int newSize, bool newForward = true);

    // assign input data in different forms
    void setInput(const double *v, int n);
    void setInput(const double *vr, const double *vi, int n);
    void setInput(const std::vector<double>& v);
    void setInput(const std::vector<double>& vr, const std::vector<double>& vi);
    void setInput(const std::vector<int>& v, double factor = 1.0);
    void setInput(const std::vector<int>& vr, const std::vector<int>& vi, double factor = 1.0);

    // input from row of 2d matrix
    //   pure real
    void setInputFromRow(const ice::matrix<double>& v, int row);
    //   complex
    void setInputFromRow(const ice::matrix<double>& vr, const ice::matrix<double>& vi, int row);
    //   pur real (int)
    void setInputFromRow(const ice::matrix<int>& v, int row, double factor = 1.0);
    //   complex (int)
    void setInputFromRow(const ice::matrix<int>& vr, const ice::matrix<int>& vi, int row, double factor = 1.0);

    // input from column of 2d matrix
    //   pure real
    void setInputFromColumn(const ice::matrix<double>& v, int col);
    //   complex
    void setInputFromColumn(const ice::matrix<double>& vr, const ice::matrix<double>& vi, int col);
    //   pur real (int)
    void setInputFromColumn(const ice::matrix<int>& v, int col,
                            double factor = 1.0);
    //   complex (int)
    void setInputFromColumn(const ice::matrix<int>& vr,
                            const ice::matrix<int>& vi,
                            int col, double factor = 1.0);

    
    void getResult(double *dstre,
                   double *dstim,int n) const;

    double getResult(double *dstre,int n) const;

    void getResult(std::vector<double>& dstre,
                   std::vector<double>& dstim) const;

    double getResult(std::vector<double>& dstre) const;

    void getResultFromReal(std::vector<double>& dstre,
                           std::vector<double>& dstim) const;

    void getResultFromImag(std::vector<double>& dstre,
                           std::vector<double>& dstim) const;

    // put Result into row of 2d matrix
    double getResultToRow(ice::matrix<double>& v, int row) const;
    void getResultToRow(ice::matrix<double>& vr,
                        ice::matrix<double>& vi, int row) const;

    // put Result into column of 2d matrix
    double getResultToColumn(ice::matrix<double>& v, int col) const;
    void getResultToColumn(ice::matrix<double>& vr,
                           ice::matrix<double>& vi, int col) const;

    // simplified calls for special cases
    void transform(const std::vector<double>& src,
                   std::vector<double>& dstre,
                   std::vector<double>& dstim,
                   bool forward = true);

    void transform(const std::vector<double>& srcre,
                   const std::vector<double>& srcim,
                   std::vector<double>& dstre,
                   std::vector<double>& dstim,
                   bool forward = true);

  private:
    void transform();

    void setSource(const double* src,int n);
    void setSource(const double* srcre,
                   const double* srcim, int n);
    void setSource(const std::vector<double>& src);
    void setSource(const std::vector<double>& srcre,
                   const std::vector<double>& srcim);
    void setSource(const std::vector<int>& src, double factor = 1.0);
    void setSource(const std::vector<int>& srcre,
                   const std::vector<int>& srcim, double factor = 1.0);

    // transformation index in vector < -- > frequency

    int getFrequencyFromIndex(int idx) const
    {
      return centered ? ((idx - size / 2 + size) % size) : idx;
    }

    int getIndexFromFrequency(int f) const
    {
      return (centered ? (f + size / 2) : (f + size)) % size;
    }

  private:

    static const int sNull = 0;
    static const int sPara = 1;
    static const int sDone = 2;

    bool checkParameter(int size);

    int size = 0;
    bool forward = true;
    bool centered = false;
    double norm;

    int state = sNull;

#ifndef NOFFTW3
    fftw_complex* in;
    fftw_complex* out;
    fftw_plan fftw_p;
#else
    void makeSinCosTab();
    void FFT();
    void FT();
    void doBitReversal();

    std::vector<double> in_real;
    std::vector<double> in_imag;
    std::vector<double> out_real;
    std::vector<double> out_imag;
    std::vector<double> sinTab;
    std::vector<double> cosTab;
#endif
  };
}
#endif

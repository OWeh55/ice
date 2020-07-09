/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2014 FSU Jena, Digital Image Processing Group
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
#ifdef OPENMP
#include <omp.h>
#endif

#include <cmath>
#include <iostream>

using namespace std;
#include "fouriertrafo.h"

namespace ice
{
#define FNAME "FourierTrafo"
  // simplified (highlevel) calls for special cases
  void FourierTrafo::transform(const std::vector<double>& source,
                               std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag,
                               bool forward)
  {
    setParameter(source.size(), forward);
    setSource(source);
    getResult(destinationReal, destinationImag);
  }

  void FourierTrafo::transform(const std::vector<double>& sourceReal,
                               const std::vector<double>& sourceImag,
                               std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag,
                               bool forward)
  {
    setParameter(sourceReal.size(), forward);
    setSource(sourceReal, sourceImag);
    getResult(destinationReal, destinationImag);
  }

#ifndef NOFFTW3
  // use fftw3
  void FourierTrafo::setParameter(int newSize, bool newForward)
  {
    // cout << "setParameter" << endl;
    if ((newSize != size) || (newForward != forward))   // changes?
      {
        // cout << "setParameter " << newSize << endl;
        if (size > 0)   // already initialised
          {
            // free old array
            fftw_free(in);
            fftw_free(out);

            // free plan
            fftw_destroy_plan(fftw_p);
          }
        if (newSize > 0)
          {
            // alloc arrays
            in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * newSize);
            out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * newSize);
            forward = newForward;
            size = newSize;
            norm = 1.0 / sqrt((double)newSize);
#ifdef OPENMP
            #pragma omp critical
#endif
            {
              fftw_p = fftw_plan_dft_1d(size, in, out,
                                        forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                        FFTW_ESTIMATE);
            }
          }
        else
          {
            in = nullptr;
            out = nullptr;
            size = 0;
          }
      }
    state = sPara;
  }

  void FourierTrafo::transform()
  {
    fftw_execute(fftw_p);
    state = sDone;
  }

  // -------- getResult -----

  void FourierTrafo::getResult(double* destinationReal,
                               double* destinationImag, int n) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else if (size != n)
      {
        throw IceException(FNAME, M_VECTORDIM);
      }
    else
      {
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destinationReal[dest] = out[i][0] * norm;
            destinationImag[dest] = out[i][1] * norm;
          }
      }
  }

  double FourierTrafo::getResult(double* destinationReal, int n) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else if (size != n)
      {
        throw IceException(FNAME, M_VECTORDIM);
      }
    else
      {
        double imagSum = 0.0;
        int dest = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destinationReal[dest] = out[i][0] * norm;
            double imagValue = out[i][1] * norm;
            imagSum += imagValue * imagValue;
          }
        return sqrt(imagSum);
      }
    return 0.0;
  }

  void FourierTrafo::getResult(std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else
      {
        destinationReal.resize(size);
        destinationImag.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destinationReal[dest] = out[i][0] * norm;
            destinationImag[dest] = out[i][1] * norm;
          }
      }
  }

  void FourierTrafo::getResultFromReal(std::vector<double>& destinationReal,
                                       std::vector<double>& destinationImag) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else
      {
        destinationReal.resize(size);
        destinationImag.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            int negidx = (size - i) % size;
            destinationReal[dest] = 0.5 * (out[i][0] + out[negidx][0]) * norm;
            destinationImag[dest] = 0.5 * (out[i][1] - out[negidx][1]) * norm;
          }
      }
  }

  void FourierTrafo::getResultFromImag(std::vector<double>& destinationReal,
                                       std::vector<double>& destinationImag) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else
      {
        destinationReal.resize(size);
        destinationImag.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            int negidx = (size - i) % size;
            destinationReal[dest] = 0.5 * (out[i][1] + out[negidx][1]) * norm;
            destinationImag[dest] = 0.5 * (-out[i][0] + out[negidx][0]) * norm;
          }
      }
  }

  double FourierTrafo::getResult(std::vector<double>& destinationReal) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else
      {
        destinationReal.resize(size);
        double imagSum = 0.0;
        int dest = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destinationReal[dest] = out[i][0] * norm;
            double imagValue = out[i][1] * norm;
            imagSum += imagValue * imagValue;
          }
        return sqrt(imagSum);
      }
    return 0.0;
  }

  void FourierTrafo::getResult(std::vector<Point>& destination) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete");
      }
    else
      {
        destination.resize(size);
        int destIdx = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, destIdx = (destIdx + 1) % size)
          {
            destination[destIdx].x = out[i][0] * norm;
            destination[destIdx].y = out[i][1] * norm;
          }
      }
  }


  // -------- setSource (privat) ---------------------------
  // get source data and transform data

  void FourierTrafo::setSource(const double* source, int n)
  {
    if (checkParameter(n))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = source[sourceIndex];
            in[i][1] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const double* sourceReal,
                               const double* sourceImag, int n)
  {
    if (checkParameter(n))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = sourceReal[sourceIndex];
            in[i][1] = sourceImag[sourceIndex];
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<double>& source)
  {
    if (checkParameter(source.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = source[sourceIndex];
            in[i][1] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<Point>& source)
  {
    if (checkParameter(source.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = source[sourceIndex].x;
            in[i][1] = source[sourceIndex].y;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<double>& sourceReal,
                               const std::vector<double>& sourceImag)
  {
    if (checkParameter(sourceReal.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = sourceReal[sourceIndex];
            in[i][1] = sourceImag[sourceIndex];
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& source, double factor)
  {
    if (checkParameter(source.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = source[sourceIndex] * factor;
            in[i][1] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& sourceReal,
                               const std::vector<int>& sourceImag, double factor)
  {
    if (checkParameter(sourceReal.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in[i][0] = sourceReal[sourceIndex] * factor;
            in[i][1] = sourceImag[sourceIndex] * factor;
          }
        transform();
      }
  }

#else
  // NOFFTW3
  void FourierTrafo::setParameter(int newsize, bool newforward)
  {
    if ((newsize != size) || (newforward != forward))   // changes?
      {
        size = newsize;
        forward = newforward;

        if (size > 0)
          {
            in_real.resize(size);
            in_imag.resize(size);
            out_real.resize(size);
            out_imag.resize(size);
            sinTab.resize(size);
            cosTab.resize(size);
            norm = 1.0 / sqrt((double)size);

            makeSinCosTab();
          }
      }
    state = sPara;
  }

  void FourierTrafo::makeSinCosTab()
  // creates sinus / cosinus tables if neccessary
  // creates "inverse" table for inverse transform if (invers)
  {
    for (int i = 0; i < size; ++i)
      {
        double fi = 2 * M_PI * i / size;

        if (forward)
          {
            sinTab[i] = sin(fi);
          }
        else
          {
            sinTab[i] = -sin(fi);
          }

        cosTab[i] = cos(fi);
      }
  }

  void FourierTrafo::doBitReversal()
  {
    int ln = 0;
    for (int i = 1; i < size; i = i + i)
      {
        ln++;
      }

    /* Do the bit reversal */
    int i2 = size >> 1;
    int j = 0;
    for (int i = 0; i < size - 1; i++)
      {
        if (i < j)
          {
            swap(out_real[j], out_real[i]);
            swap(out_imag[j], out_imag[i]);
          }
        int k = i2;
        while (k <= j)
          {
            j -= k;
            k >>= 1;
          }
        j += k;
      }
  }

  void FourierTrafo::FFT()
  {
    /* ln = log_2(size) */
    int ln = 0;
    for (int i = 1; i < size; i = i + i)
      ln++;

    doBitReversal();

    /* Compute the FFT */
    double cosdelta = -1.0;
    double sindelta = 0.0;
    int l2 = 1;
    for (int l = 0; l < ln; l++)
      {
        int l1 = l2;
        l2 <<= 1;
        double cosfi = 1.0;
        double sinfi = 0.0;
        for (int j = 0; j < l1; j++)
          {
            for (int idx1 = j; idx1 < size; idx1 += l2)
              {
                int idx2 = idx1 + l1;
                double t_real = cosfi * out_real[idx2] - sinfi * out_imag[idx2];
                double t_imag = cosfi * out_imag[idx2] + sinfi * out_real[idx2];
                out_real[idx2] = out_real[idx1] - t_real;
                out_imag[idx2] = out_imag[idx1] - t_imag;
                out_real[idx1] += t_real;
                out_imag[idx1] += t_imag;
              }
            double cosfinew =  cosfi * cosdelta - sinfi * sindelta;
            sinfi = sinfi * cosdelta + cosfi * sindelta;
            cosfi = cosfinew;
          }
        sindelta = sqrt((1.0 - cosdelta) / 2.0);
        if (forward)
          {
            sindelta = -sindelta;
          }
        cosdelta = sqrt((1.0 + cosdelta) / 2.0);
      }
  }

  void FourierTrafo::FT()
  {
    for (int freq = 0; freq < size; ++freq)
      {
        double treal = 0.0;
        double timag = 0.0;

        int iPhi = 0;

        for (int j = 0; j < size; ++j)
          {
            treal += in_real[j] * cosTab[iPhi] + in_imag[j] * sinTab[iPhi];
            timag += -in_real[j] * sinTab[iPhi] + in_imag[j] * cosTab[iPhi];
            iPhi = (iPhi + freq) % size;
          }
        out_real[freq] = treal;
        out_imag[freq] = timag;
      }
  }

  void FourierTrafo::transform()
  {
    if ((size & (size - 1)) == 0)   // size is potence of 2 ?
      {
        out_real = in_real;
        out_imag = in_imag;
        FFT();
      }
    else
      {
        FT();
      }
    state = sDone;
  }

  void FourierTrafo::setSource(const std::vector<double>& source)
  {
    if (checkParameter(source.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in_real[i] = source[sourceIndex];
            in_imag[i] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<double>& sourceReal,
                               const std::vector<double>& sourceImag)
  {
    if (checkParameter(sourceReal.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in_real[i] = sourceReal[sourceIndex];
            in_imag[i] = sourceImag[sourceIndex];
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& source, double factor)
  {
    if (checkParameter(source.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in_real[i] = source[sourceIndex] * factor;
            in_imag[i] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& sourceReal,
                               const std::vector<int>& sourceImag, double factor)
  {
    if (checkParameter(sourceReal.size()))
      {
        int sourceIndex = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
          {
            in_real[i] = sourceReal[sourceIndex] * factor;
            in_imag[i] = sourceImag[sourceIndex] * factor;
          }
        transform();
      }
  }

  void FourierTrafo::getResult(std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag) const
  {
    destinationReal.resize(size);
    destinationImag.resize(size);
    int dest = getIndexFromFrequency(0);
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        destinationReal[dest] = out_real[i] * norm;
        destinationImag[dest] = out_imag[i] * norm;
      }
  }

  double FourierTrafo::getResult(std::vector<double>& destinationReal) const
  {
    destinationReal.resize(size);
    double imagSum = 0.0;
    int dest = getIndexFromFrequency(0);
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        destinationReal[dest] = out_real[i] * norm;
        double imagValue = out_imag[i] * norm;
        imagSum += imagValue * imagValue;
      }
    return sqrt(imagSum);
  }
#endif

  // put Result into row of 2d matrix
  double FourierTrafo::getResultToRow(ice::matrix<double>& v, int row) const
  {
    vector<double> res;
    double absim = getResult(res);
    for (int i = 0; i < size; ++i)
      {
        v[row][i] = res[i];
      }
    return absim;
  }

  void FourierTrafo::getResultToRow(ice::matrix<double>& vr,
                                    ice::matrix<double>& vi, int row) const
  {
    vector<double> resr;
    vector<double> resi;
    getResult(resr, resi);
    for (int i = 0; i < size; ++i)
      {
        vr[row][i] = resr[i];
        vi[row][i] = resi[i];
      }
  }

  // put Result into column of 2d matrix
  double FourierTrafo::getResultToColumn(ice::matrix<double>& v, int col) const
  {
    vector<double> res;
    double absim = getResult(res);
    for (int i = 0; i < size; ++i)
      {
        v[i][col] = res[i];
      }
    return absim;
  }

  void FourierTrafo::getResultToColumn(ice::matrix<double>& vr,
                                       ice::matrix<double>& vi, int col) const
  {
    vector<double> resr;
    vector<double> resi;
    getResult(resr, resi);
    for (int i = 0; i < size; ++i)
      {
        vr[i][col] = resr[i];
        vi[i][col] = resi[i];
      }
  }

  void FourierTrafo::setInputFromRow(const ice::matrix<double>& v, int row)
  {
    vector<double> thisrow(v.cols());
    for (int i = 0; i < v.cols(); ++i)
      {
        thisrow[i] = v[row][i];
      }
    setInput(thisrow);
  }

  void FourierTrafo::setInputFromRow(const ice::matrix<double>& vr, const ice::matrix<double>& vi, int row)
  {
    vector<double> rowr(vr.cols());
    for (int i = 0; i < vr.cols(); ++i)
      {
        rowr[i] = vr[row][i];
      }
    vector<double> rowi(vi.cols());
    for (int i = 0; i < vi.cols(); ++i)
      {
        rowi[i] = vi[row][i];
      }
    setInput(rowr, rowi);
  }

  void FourierTrafo::setInputFromRow(const ice::matrix<int>& v, int row, double factor)
  {
    vector<int> thisrow(v.cols());
    for (int i = 0; i < v.cols(); ++i)
      {
        thisrow[i] = v[row][i];
      }
    setInput(thisrow, factor);
  }

  void FourierTrafo::setInputFromRow(const ice::matrix<int>& vr, const ice::matrix<int>& vi, int row, double factor)
  {
    vector<int> rowr(vr.cols());
    for (int i = 0; i < vr.cols(); ++i)
      {
        rowr[i] = vr[row][i];
      }
    vector<int> rowi(vi.cols());
    for (int i = 0; i < vi.cols(); ++i)
      {
        rowi[i] = vi[row][i];
      }
    setInput(rowr, rowi, factor);
  }

  void FourierTrafo::setInputFromColumn(const ice::matrix<double>& v, int col)
  {
    vector<double> thiscol(v.rows());
    for (int i = 0; i < v.rows(); ++i)
      {
        thiscol[i] = v[i][col];
      }
    setInput(thiscol);
  }

  void FourierTrafo::setInputFromColumn(const ice::matrix<double>& vr, const ice::matrix<double>& vi, int col)
  {
    vector<double> colr(vr.rows());
    for (int i = 0; i < vr.rows(); ++i)
      {
        colr[i] = vr[i][col];
      }
    vector<double> coli(vi.rows());
    for (int i = 0; i < vi.rows(); ++i)
      {
        coli[i] = vi[i][col];
      }
    setInput(colr, coli);
  }

  void FourierTrafo::setInputFromColumn(const ice::matrix<int>& v, int col, double factor)
  {
    vector<int> thiscol(v.rows());
    for (int i = 0; i < v.rows(); ++i)
      {
        thiscol[i] = v[i][col];
      }
    setInput(thiscol, factor);
  }

  void FourierTrafo::setInputFromColumn(const ice::matrix<int>& vr, const ice::matrix<int>& vi, int col, double factor)
  {
    vector<int> colr(vr.rows());
    for (int i = 0; i < vr.rows(); ++i)
      {
        colr[i] = vr[i][col];
      }
    vector<int> coli(vi.rows());
    for (int i = 0; i < vi.rows(); ++i)
      {
        coli[i] = vi[i][col];
      }
    setInput(colr, coli, factor);
  }

  void FourierTrafo::setInput(const double* v, int n)
  {
    checkParameter(n);
    setSource(v, n);
  }

  void FourierTrafo::setInput(const double* vr,
                              const double* vi, int n)
  {
    checkParameter(n);
    setSource(vr, vi, n);
  }

  void FourierTrafo::setInput(const std::vector<double>& v)
  {
    checkParameter(v.size());
    setSource(v);
  }

  void FourierTrafo::setInput(const std::vector<Point>& v)
  {
    checkParameter(v.size());
    setSource(v);
  }

  void FourierTrafo::setInput(const std::vector<double>& vr,
                              const std::vector<double>& vi)
  {
    checkParameter(vr.size());
    setSource(vr, vi);
  }

  void FourierTrafo::setInput(const std::vector<int>& v, double factor)
  {
    checkParameter(v.size());
    setSource(v, factor);
  }

  void FourierTrafo::setInput(const std::vector<int>& vr,
                              const std::vector<int>& vi, double factor)
  {
    setSource(vr, vi, factor);
  }

  bool FourierTrafo::checkParameter(int sourceSize)
  {
    if (state == sNull)   // no set
      {
        setParameter(sourceSize);
      }
    else
      {
        if (size != sourceSize)
          throw IceException(FNAME, M_VECTORDIM);
      }
    return true;
  }
}

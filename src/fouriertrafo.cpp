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
#include <cmath>
#include <iostream>

using namespace std;
#include "fouriertrafo.h"

namespace ice
{
#define FNAME "FourierTrafo"
  // simplified (highlevel) calls for special cases
  void FourierTrafo::transform(const std::vector<double>& src,
                               std::vector<double>& destre,
                               std::vector<double>& destim,
                               bool forward)
  {
    setParameter(src.size(), forward);
    setSource(src);
    getResult(destre, destim);
  }

  void FourierTrafo::transform(const std::vector<double>& srcre,
                               const std::vector<double>& srcim,
                               std::vector<double>& destre,
                               std::vector<double>& destim,
                               bool forward)
  {
    setParameter(srcre.size(), forward);
    setSource(srcre, srcim);
    getResult(destre, destim);
  }

#ifndef NOFFTW3
  // use fftw3
  void FourierTrafo::setParameter(int newSize, bool newForward)
  {
    if ((newSize != size) || (newForward != forward))   // changes?
      {
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
            fftw_p = fftw_plan_dft_1d(size, in, out,
                                      forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                      FFTW_ESTIMATE);
          }
        else
          {
            in = nullptr;
            out = nullptr;
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

  void FourierTrafo::getResult(std::vector<double>& destre,
                               std::vector<double>& destim) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete", ERROR);
      }
    else
      {
        destre.resize(size);
        destim.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destre[dest] = out[i][0] * norm;
            destim[dest] = out[i][1] * norm;
          }
      }
  }

  void FourierTrafo::getResultFromReal(std::vector<double>& destre,
                                       std::vector<double>& destim) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete", ERROR);
      }
    else
      {
        destre.resize(size);
        destim.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            int negidx = (size - i) % size;
            destre[dest] = 0.5 * (out[i][0] + out[negidx][0]) * norm;
            destim[dest] = 0.5 * (out[i][1] - out[negidx][1]) * norm;
          }
      }
  }

  void FourierTrafo::getResultFromImag(std::vector<double>& destre,
                                       std::vector<double>& destim) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete", ERROR);
      }
    else
      {
        destre.resize(size);
        destim.resize(size);
        int dest = getIndexFromFrequency(0);
        //    cout << dest << endl;
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            int negidx = (size - i) % size;
            destre[dest] = 0.5 * (out[i][1] + out[negidx][1]) * norm;
            destim[dest] = 0.5 * (-out[i][0] + out[negidx][0]) * norm;
          }
      }
  }

  double FourierTrafo::getResult(std::vector<double>& destre) const
  {
    if (state != sDone)
      {
        throw IceException(FNAME, "data incomplete", ERROR);
      }
    else
      {
        destre.resize(size);
        double imagSum = 0.0;
        int dest = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
          {
            destre[dest] = out[i][0] * norm;
            double imagValue = out[i][1] * norm;
            imagSum += imagValue * imagValue;
          }
        return sqrt(imagSum);
      }
    return 0.0;
  }

  // -------- setSource (privat) ---------------------------
  // get source data and transform data
  void FourierTrafo::setSource(const std::vector<double>& src)
  {
    if (checkParameter(src.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in[i][0] = src[source];
            in[i][1] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<double>& srcre,
                               const std::vector<double>& srcim)
  {
    if (checkParameter(srcre.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in[i][0] = srcre[source];
            in[i][1] = srcim[source];
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& src, double factor)
  {
    if (checkParameter(src.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in[i][0] = src[source] * factor;
            in[i][1] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& srcre,
                               const std::vector<int>& srcim, double factor)
  {
    if (checkParameter(srcre.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in[i][0] = srcre[source] * factor;
            in[i][1] = srcim[source] * factor;
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
      {
        ln++;
      }

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

  void FourierTrafo::setSource(const std::vector<double>& src)
  {
    if (checkParameter(src.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in_real[i] = src[source];
            in_imag[i] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<double>& srcre,
                               const std::vector<double>& srcim)
  {
    if (checkParameter(srcre.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in_real[i] = srcre[source];
            in_imag[i] = srcim[source];
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& src, double factor)
  {
    if (checkParameter(src.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in_real[i] = src[source] * factor;
            in_imag[i] = 0.0;
          }
        transform();
      }
  }

  void FourierTrafo::setSource(const std::vector<int>& srcre,
                               const std::vector<int>& srcim, double factor)
  {
    if (checkParameter(srcre.size()))
      {
        int source = getIndexFromFrequency(0);
        for (int i = 0; i < size; ++i, source = (source + 1) % size)
          {
            in_real[i] = srcre[source] * factor;
            in_imag[i] = srcim[source] * factor;
          }
        transform();
      }
  }

  void FourierTrafo::getResult(std::vector<double>& destre,
                               std::vector<double>& destim) const
  {
    destre.resize(size);
    destim.resize(size);
    int dest = getIndexFromFrequency(0);
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        destre[dest] = out_real[i] * norm;
        destim[dest] = out_imag[i] * norm;
      }
  }

  double FourierTrafo::getResult(std::vector<double>& destre) const
  {
    destre.resize(size);
    double imagSum = 0.0;
    int dest = getIndexFromFrequency(0);
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        destre[dest] = out_real[i] * norm;
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

  void FourierTrafo::setInput(const std::vector<double>& v)
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
          {
            throw IceException(FNAME, M_VECTORDIM, WRONG_PARAM);
            return false;
          }
      }
    return true;
  }
}

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
#include "FourierTrafo.h"

namespace ice
{
#define FNAME "FourierTrafo"
  // simplified (highlevel) calls for special cases
  void FourierTrafo::transform(const std::vector<double>& source,
                               std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag,
                               bool forward)
  {
    FourierTrafo ft;
    ft.setParameter(source.size(), forward);
    ft.setInput(source);
    ft.getResult(destinationReal, destinationImag);
  }

  void FourierTrafo::transform(const std::vector<double>& sourceReal,
                               const std::vector<double>& sourceImag,
                               std::vector<double>& destinationReal,
                               std::vector<double>& destinationImag,
                               bool forward)
  {
    FourierTrafo ft;
    ft.setParameter(sourceReal.size(), forward);
    ft.setInput(sourceReal, sourceImag);
    ft.getResult(destinationReal, destinationImag);
  }

  void FourierTrafo::setParameter(int newSize, bool newForward)
  {
    // cout << "setParameter" << endl;
    VWorker::setParameter(newSize);
    norm = 1.0 / sqrt(size);
    if (forward != newForward)
      {
        forward = newForward;
        resultValid = false;
      }
  }

  void FourierTrafo::getResultFromReal(std::vector<double>& destinationReal,
                                       std::vector<double>& destinationImag) const
  {
    checkDone();
    destinationReal.resize(size);
    destinationImag.resize(size);
    int dest = getIndexFromFrequency(0);
    //    cout << dest << endl;
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        int negidx = (size - i) % size;
        destinationReal[dest] = 0.5 * (result[0][i] + result[0][negidx]);
        destinationImag[dest] = 0.5 * (result[1][i] - result[1][negidx]);
      }
  }

  void FourierTrafo::getResultFromImag(std::vector<double>& destinationReal,
                                       std::vector<double>& destinationImag) const
  {
    checkDone();
    destinationReal.resize(size);
    destinationImag.resize(size);
    int dest = getIndexFromFrequency(0);

    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        int negidx = (size - i) % size;
        destinationReal[dest] = 0.5 * (result[1][i] + result[1][negidx]);
        destinationImag[dest] = 0.5 * (-result[0][i] + result[0][negidx]);
      }
  }

#ifndef NOFFTW3
  // use fftw3
  void FourierTrafo::transform() const
  {
    if ((oldsize != size) || (oldforward != forward))
      {
        in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
        out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);

        fftw_p = fftw_plan_dft_1d(size, in, out,
                                  forward ? FFTW_FORWARD : FFTW_BACKWARD,
                                  FFTW_ESTIMATE);
        oldsize = size;
        oldforward = forward;
      }

    int sourceIndex = getIndexFromFrequency(0);

    if (input[1].empty())
      for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
        {
          in[i][0] = input[0][sourceIndex];
          in[i][1] = 0.0;
        }
    else
      for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
        {
          in[i][0] = input[0][sourceIndex];
          in[i][1] = input[1][sourceIndex];
        }

    fftw_execute(fftw_p);

    result[0].resize(size);
    result[1].resize(size);

    int dest = getIndexFromFrequency(0);
    for (int i = 0; i < size; ++i, dest = (dest + 1) % size)
      {
        result[0][dest] = out[i][0] * norm;
        result[1][dest] = out[i][1] * norm;
      }
  }

#else
  void FourierTrafo::makeSinCosTab() const
  // creates sinus / cosinus tables if neccessary
  // creates "inverse" table for inverse transform if (invers)
  {
    for (int i = 0; i < size; ++i)
      {
        double fi = 2 * M_PI * i / size;

        if (forward)
          sinTab[i] = sin(fi);
        else
          sinTab[i] = -sin(fi);

        cosTab[i] = cos(fi);
      }
  }

  void FourierTrafo::doBitReversal() const
  {
    /* ln = log_2(size) */
    int ln = 0;
    for (int i = 1; i < size; i = i + i)
      ln++;

    /* Do the bit reversal */
    int i2 = size >> 1;
    int j = 0;
    for (int i = 0; i < size - 1; i++)
      {
        if (i < j)
          {
            swap(result[0][j], result[0][i]);
            swap(result[1][j], result[1][i]);
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

  void FourierTrafo::FFT() const
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
                double t_real = cosfi * result[0][idx2] - sinfi * result[1][idx2];
                double t_imag = cosfi * result[1][idx2] + sinfi * result[0][idx2];
                result[0][idx2] = result[0][idx1] - t_real;
                result[1][idx2] = result[1][idx1] - t_imag;
                result[0][idx1] += t_real;
                result[1][idx1] += t_imag;
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

  void FourierTrafo::FT() const
  {
    vector<double> rr(size);
    vector<double> ri(size);
    for (int freq = 0; freq < size; ++freq)
      {
        double treal = 0.0;
        double timag = 0.0;

        int iPhi = 0;

        for (int j = 0; j < size; ++j)
          {
            treal += result[0][j] * cosTab[iPhi] + result[1][j] * sinTab[iPhi];
            timag += -result[0][j] * sinTab[iPhi] + result[1][j] * cosTab[iPhi];
            iPhi = (iPhi + freq) % size;
          }
        rr[freq] = treal;
        ri[freq] = timag;
      }
    result[0] = rr;
    result[1] = ri;
  }

  void FourierTrafo::transform() const
  {
    result[0].resize(size);
    result[1].resize(size);
    int sourceIndex = getIndexFromFrequency(0);
    if (input[1].empty())
      for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
        {
          result[0][i] = input[0][sourceIndex];
          result[1][i] = 0.0;
        }
    else
      for (int i = 0; i < size; ++i, sourceIndex = (sourceIndex + 1) % size)
        {
          result[0][i] = input[0][sourceIndex];
          result[1][i] = input[1][sourceIndex];
        }

    if ((size & (size - 1)) == 0)   // size is potence of 2 ?
      FFT();
    else
      {
        if ((oldsize != size) || (oldforward != forward))
          {
            sinTab.resize(size);
            cosTab.resize(size);
            makeSinCosTab();
            oldsize = size;
            oldforward = forward;
          }
        FT();
      }
  }
#endif
}

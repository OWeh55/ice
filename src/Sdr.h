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
#ifndef SDR_CLASS_H
#define SDR_CLASS_H

#include <complex>
#ifdef USE_CUDA
#include <cufftw.h>
#else
#include <fftw3.h>
#endif
#include <vector>

#include <based.h>
#include <base.h>
//#include <image.h>

using namespace ice;
// optimized class for shift detection by restoration
// uses fftw directly, avoids multiple copying of data
namespace ice
{
  class Sdr
  {
  public:
    Sdr(int xSize, int ySize, double b = 10, int factor = 1, int window = 1);
    virtual ~Sdr();
    virtual void setCutOff(double f)
    {
      fg = f;
      fg2 = fg * fg;
    }
    virtual void setWindowMode(int m);

    virtual void setBeta(double b)
    {
      beta = b;
      beta2 = b * b;
    }

    virtual double getBeta() const
    {
      return beta;
    }

    virtual int getFactor() const
    {
      return factor;
    }
    virtual int getXSize() const
    {
      return xSize;
    }
    virtual int getYSize() const
    {
      return ySize;
    }

    virtual void setInput(const Image& img1, const Image& img2);
    virtual void setInput(const ImageD& img1, const ImageD& img2);
    virtual void getPeak(ImageD& result);
    virtual void getPeak(Image& result);
  private:
    // basic parameters set by c'tor
    int xSize;
    int ySize;
    double beta;
    double beta2;
    int factor;

    int nPixel;
    int bigXSize;
    int bigYSize;

    int windowing;

    // quasi fixed parameters
    double fg = 2;        // cutoff frequency of highpass
    double fg2 = fg * fg; // square of cutoff frequency of highpass

    fftw_plan spectrumPlan;
    fftw_plan peakPlan;

    std::vector<double> data;
    std::vector<double> peakdata;
    std::vector<double> xWindow;
    std::vector<double> yWindow;

    static void window0(std::vector<double>& window);
    static void window1(std::vector<double>& window);
    static void window2(std::vector<double>& window);
  };
}
#endif

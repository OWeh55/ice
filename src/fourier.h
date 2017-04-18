/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
   fourier.h,
   Wolfgang Ortmann 1997, 1998, 2006, 2013
*/
#ifndef _FOURIER_H
#define _FOURIER_H

#include <vector>

#include "based.h"
#include "Vector.h"
#include "Matrix.h"

namespace ice
{
  // 1d Fourier transform

  // low level function using c array
  // (prefer to use class FourierTrafo for Fourier transform!)
  // implementation in fourier_fftw.cpp and fourier_nofftw.cpp
  // define NOFFTW3 if you do not want or nor can use library fftw3
  //
  int FourierD(const double* srcre, const double* srcim,
               int dim, int option,
               double* dstre, double* dstim);

  // Fourier transform for c++ and ice datatypes
  // (prefer to use class FourierTrafo for Fourier transform!)
  // implementation in fourier.cpp
  // vector + vector -> vector + vector
  int Fourier(const std::vector<double>& srcre, const std::vector<double>& srcim,
              std::vector<double>& dstre, std::vector<double>& dstim,
              int option = NORMAL);
  // inplace  vector + vector
  int Fourier(std::vector<double>& srcre, std::vector<double>& srcim,
              int option = NORMAL);

  // Vector + Vector -> Vector + Vector
  int Fourier(const Vector& re, const Vector& im,
              int option,
              Vector& rre, Vector& rim);
  // inplace  Vector + Vector
  int Fourier(Vector& re, Vector& im, int option = NORMAL);

  // complex Numbers as row in a Matrix
  int Fourier(const Matrix& ms, int option, Matrix& md);
  int Fourier(Matrix& ms, int option = NORMAL);


  // (prefer to use class HartleyTrafo for Hartley transform!)
  // low level 1d Hartley transform
  void HartleyD(const double* src, int dim, double* dst);

  void Hartley(const Vector& s, Vector& d);
  void Hartley(Vector& s);

  // constants for parameter dir in 2d-functions
#define XY 0
#define X_ONLY 1
#define Y_ONLY 2

  // constants for parameter mode
#define MD_MAGNITUDE 0
#define MD_POWER 1
#define MD_LOG 2

  // 2d Fourier transform
  // implementation in fourier1.cpp
  int FourierImgD(ImageD srcre, ImageD srcim,
                  int option,
                  ImageD dstre, ImageD dstim,
                  int dir = XY);

  // 2d Hartley transform
  void HartleyImgD(ImageD src, ImageD dst, int dir = XY);
  void HartleyImg(const Image& src, ImageD dst, int dir = XY);

  // fourier2.cpp

  // Powerspectrum from Fourier
  int PowerSpectrumFImgD(ImageD re, ImageD im, ImageD nrm, int mode = MD_POWER);

  // Powerspectrum from Hartley
  int PowerSpectrumHImgD(ImageD sp, ImageD nrm, int mode = MD_POWER);

  // magnitude-phase-spectrum from Fourier
  int MPSpectrumFImgD(ImageD re, ImageD im, ImageD magnitude, ImageD phase);

  // magnitude-phase-spectrum from Hartley
  int MPSpectrumHImgD(ImageD sp, ImageD magnitude, ImageD phase);

  // Fourier from MPspectrum
  int FourierMPImgD(ImageD magnitude, ImageD phase, ImageD re, ImageD im);

  // Hartley from MPspectrum
  int HartleyMPImgD(ImageD magnitude, ImageD phase, ImageD ht);

  // Power spectrum of an image
  int PowerSpectrumImgD(ImageD img, ImageD spec, int mode = MD_POWER);
  // cepstrum of an image
  int CepstrumImgD(ImageD img, ImageD ceps);
  // Magnitude phase spectrum of an image
  void MPSpectrumImgD(ImageD img, ImageD mag, ImageD phase);

  // fourier3.cpp
  // convolution in frequency domain
  // convolution in fourier images
  void ConvolutionFImgD(ImageD re1, ImageD im1, ImageD re2, ImageD im2, ImageD re3, ImageD im3);
  // convolution in hartley images
  void ConvolutionHImgD(ImageD im1, ImageD im2, ImageD im3);

  // inverse convolution in frequency domain
  // inverse convolution in fourier images
  void InvConvolutionFImgD(ImageD re1, ImageD im1, ImageD re2, ImageD im2,
                           ImageD re3, ImageD im3, double beta);
  // inverse convolution in hartley images
  void InvConvolutionHImgD(ImageD im1, ImageD im2, ImageD im3, double beta);

  // CrossCorrelation
  void CrossCorrelationFImgD(ImageD re1, ImageD im1, ImageD re2, ImageD im2, ImageD re3, ImageD im3);
  void CrossCorrelationHImgD(ImageD im1, ImageD im2, ImageD im3);
  // CrossCorrelation
  void CrossCorrelationImgD(ImageD im1, ImageD im2, ImageD im3);


  // fourier4.cpp
  // Whitening
  void WhiteningFImgD(ImageD re1, ImageD im1, ImageD re2, ImageD im2, double beta = 0);
  void WhiteningHImgD(ImageD im1, ImageD im2, double beta = 0);

  // Whitening
  void WhiteningImgD(ImageD im1, ImageD im2, double beta = 0);

  // in fourier?.cpp/conv.cpp internally used tools

  inline int negf(int i, int size)
  {
    // returns index, which holds the coefficient with
    // frequency negative to that of index i
    // frequenzy 0 is at index size/2
    if ((size % 2) != 0)
      {
        return size - 1 - i;
      }
    if (i == 0)
      {
        return 0;
      }
    return size - i;
  }

  inline int negfa(int f, int size)
  {
    // Returns index, which holds the coefficient with
    // frequency negative to that of index f
    // Frequenzy 0 is at index 0
    return (size - f) % size;
  }
}
#endif

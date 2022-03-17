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

/***********************************************************************
 * "ColorSpace.h"
 *    Wolfgang Ortmann 2013
 ***********************************************************************/

#ifndef _COLORSPACE_H
#define _COLORSPACE_H

#include "numbase.h"
#include "util.h"
#include "ColorValue.h"
#include "ColorImage.h"

namespace ice
{
  void HsiToRgb(double h, double s, double i, ColorValue& dst, int maxval = 255);
  void HsiToRgb(double h, double s, double i, double& r, double& g, double& b);

  void RgbToHsi(const ColorValue& src, int maxval,
                double& h, double& s, double& i);
  void RgbToHsi(const ColorValue& src,
                double& h, double& s, double& i);
  void RgbToHsi(double r, double g, double b,
                double& h, double& s, double& i);
//---------------------------------
  void YuvToRgb(double y, double u, double v, ColorValue& dst, int maxval = 255);
  void YuvToRgb(double y, double u, double v, double& r, double& g, double& b);
  void RgbToYuv(const ColorValue& src, int maxval, double& h, double& s, double& i);
  void RgbToYuv(const ColorValue& src, double& h, double& s, double& i);
  void RgbToYuv(double r, double g, double b, double& h, double& s, double& i);
//-----------------------------------------
  void RgbToXyz(const ColorValue& src, int maxval, double& x, double& y, double& z);
  void RgbToXyz(double r, double g, double b, double& x, double& y, double& z);

  void XyzToRgb(double x, double y, double z, double& r, double& g, double& b);
  void XyzToRgb(double x, double y, double z, ColorValue& dst, int maxval = 255);
//--------------------------------------
  void XyzToLab(double x, double y, double z, double& l, double& a, double& b);
  void LabToXyz(double l, double a, double b, double& x, double& y, double& z);

//-----------------------------------
  void RgbToLab(const ColorValue& src, int maxval, double& x, double& y, double& z);
  void RgbToLab(double r, double g, double b, double& x, double& y, double& z);

  void LabToRgb(double l, double a, double b, double& rr, double& gg, double& bb);
  void LabToRgb(double l, double a, double b, ColorValue& dst, int maxval = 255);

//=====================================
  int ColorImageToHsi(const ColorImage& src, const Image& hue, const Image& saturation, const Image& intensity);
  int HsiToColorImage(const Image& hue, const Image& saturation, const Image& intensity, const ColorImage& dst);

  int ColorImageToLab(const ColorImage& src,
                      const Image& lImg, const Image& aImg, const Image& bImg);
  int LabToColorImage(const Image& lImg, const Image& aImg, const Image& bImg,
                      const ColorImage& src);

  int ColorImageToYuv(const ColorImage& src, const Image& y, const Image& u, const Image& v);
  int YuvToColorImage(const Image& y, const Image& u, const Image& v, const ColorImage& dst);

} /* namespace ice */
#endif /* IFNDEF _COLORSPACE_H */

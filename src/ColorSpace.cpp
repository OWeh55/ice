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

#include <math.h>
#include <stdio.h>

#include "macro.h"
#include "util.h"

#include "ColorSpace.h"

namespace ice
{
  void HsiToRgb(double h, double s, double i, double& r, double& g, double& b)
  {
    double H = h * 2.0 * M_PI; // 0 <= H <= 2pi

    double R = 1.0;
    double G = 1.0;
    double B = 1.0;

    constexpr double pi3 = M_PI / 3.0;
    constexpr double pi23 = M_PI * 2.0 / 3.0;

    // Saturation = 0 is equal to the gray-value R=G=B=I
    if (s >= 0)
      {
        // H < 60 with maxHue = 180  OR  H < 120 with H in [0,360)
        if (H < pi23)
          {
            B = 1.0 - s;
            R = 1.0 + s * cos(H) / cos(pi3 - H);
            G = 3.0 - (R + B);
          }
        // 60 <= H < 120 with maxHue = 180  OR  120 <= H < 240 with H in [0,360)
        else
          {
            H -= pi23;
            if (H < pi23)
              {
                R = 1.0 - s;
                G = 1.0 + s * cos(H) / cos(pi3 - H);
                B = 3.0 - (R + G);

                // 120 <= H < 180 with maxHue = 180 OR 240 <= H < 360 with H in [0,360)
              }
            else
              {
                H -= pi23;
                G = 1.0 - s;
                B = 1.0 + s * cos(H) / cos(pi3 - H);
                R = 3.0 - (G + B);
              }
          }
      }
    r = R * i;
    g = G * i;
    b = B * i;
  }

  void RgbToHsi(double r, double g, double b, double& h, double& s, double& i)
  {
    // compute Intensity
    i = (r + g + b) / 3.0;
    s = 0;
    h = 0;

    // compute Saturation
    if (i > 0.0)
      {
        if (r != g || g != b)
          {
            double min = r;
            if (g < min)
              {
                min = g;
              }
            if (b < min)
              {
                min = b;
              }
            s = 1.0 - min / i;

            if (s > 0.0)
              {
                double tmp = sqrt((r - g) * (r - g) + (r - b) * (g - b));

                double tmpH = acos(0.5 * ((r - g) + (r - b)) / tmp);
                if (b > g)
                  {
                    h = (2.0 * M_PI - tmpH) / (2.0 * M_PI);
                  }
                else
                  {
                    h = tmpH / (2.0 * M_PI);
                  }
              }
          }
      }
  }

  static void fromColorValue(const ColorValue& src, int maxval,
                             double& r, double& g, double& b)
  {
    r = (double)src.red / maxval;
    g = (double)src.green / maxval;
    b = (double)src.blue / maxval;
  }

  static void toColorValue(double r, double g, double b, ColorValue& dst, int maxval)
  {
    dst.red = RoundInt(limited(r * maxval, 0, maxval));
    dst.green = RoundInt(limited(g * maxval, 0, maxval));
    dst.blue = RoundInt(limited(b * maxval, 0, maxval));
  }

  void HsiToRgb(double h, double s, double i, ColorValue& dst, int maxval)
  {
    double r, g, b;
    HsiToRgb(h, s, i, r, g, b);
    toColorValue(r, g, b, dst, maxval);
  }

  void RgbToHsi(const ColorValue& src, int maxval, double& h, double& s, double& i)
  {
    double r, g, b;
    fromColorValue(src, maxval, r, g, b);
    RgbToHsi(r, g, b, h, s, i);
  }

  void RgbToHsi(const ColorValue& src, double& h, double& s, double& i)
  {
    RgbToHsi(src, 255, h, s, i);
  }

  //====================================================================

  void YuvToRgb(double y, double u, double v, double& r, double& g, double& b)
  {
    b = y + u / 0.493;
    r = y + v / 0.877;
    g = y - 0.39393 * u - 0.58081 * v;
  }

  void RgbToYuv(double r, double g, double b, double& y, double& u, double& v)
  {
    y = 0.299 * r + 0.587 * g + 0.114 * b;
    u = 0.493 * (b - y);
    v = 0.877 * (r - y);
  }

  void RgbToYuv(const ColorValue& src, int maxval, double& y, double& u, double& v)
  {
    double r, g, b;
    fromColorValue(src, maxval, r, g, b);
    RgbToYuv(r, g, b, y, u, v);
  }

  void RgbToYuv(const ColorValue& src, double& y, double& u, double& v)
  {
    RgbToYuv(src, 255, y, u, v);
  }

  void YuvToRgb(double y, double u, double v, ColorValue& dst, int maxval)
  {
    double r, g, b;
    YuvToRgb(y, u, v, r, g, b);
    toColorValue(r, g, b, dst, maxval);
  }

  //====================================================================

  void RgbToXyz(double r, double g, double b, double& x, double& y, double& z)
  {
    x = 0.4124564 * r + 0.3575761 * g + 0.1804375 * b;
    y = 0.2126729 * r + 0.7151522 * g + 0.0721750 * b;
    z = 0.0193339 * r + 0.1191920 * g + 0.9503041 * b;
  }

  void RgbToXyz(const ColorValue& src, int maxval, double& x, double& y, double& z)
  {
    double r, g, b;
    fromColorValue(src, maxval, r, g, b);
    RgbToXyz(r, g, b, x, y, z);
  }

  void XyzToRgb(double x, double y, double z, double& r, double& g, double& b)
  {
    r =  3.2404542 * x + -1.5371385 * y + -0.4985314 * z;
    g = -0.9692660 * x +  1.8760108 * y +  0.0415560 * z;
    b =  0.0556434 * x + -0.2040259 * y +  1.0572252 * z;
  }

  void XyzToRgb(double x, double y, double z, ColorValue& cv, int maxval)
  {
    double r, g, b;
    XyzToRgb(x, y, z, r, g, b);
    toColorValue(r, g, b, cv, maxval);
  }

  //===================================================================
  const double xref = 0.95;
  const double yref = 1.00;
  const double zref = 1.09;

  const double eps = 0.008856;
  const double kappa = 903.3;

  static double fxyz(double cn)
  {
    if (cn > eps)
      {
        return cbrt(cn);
      }
    else
      {
        return (kappa * cn + 16) / 116 ;
      }
  }

  static double fxyzr(double cn)
  {
    double cn3 = cn * cn * cn;
    if (cn3 > eps)
      {
        return cn3;
      }
    else
      {
        return (116 * cn - 16) / kappa;
      }
  }

  void XyzToLab(double x, double y, double z, double& l, double& a, double& b)
  {
    double xn = x / xref;
    double yn = y / yref;
    double zn = z / zref;

    double x3 = fxyz(xn);
    double y3 = fxyz(yn);
    double z3 = fxyz(zn);

    l = 116 * y3 - 16;
    a = 500 * (x3 - y3);
    b = 200 * (y3 - z3);
  }

  void LabToXyz(double l, double a, double b, double& x, double& y, double& z)
  {
    double y3 = (l + 16) / 116;
    double x3 = a / 500 + y3;
    double z3 = y3 - b / 200;

    x = fxyzr(x3) * xref;
    y = fxyzr(y3) * yref;
    z = fxyzr(z3) * zref;
    //    cout << x << " " << y << " " << z << endl;
  }

  //============================================================
  void RgbToLab(const ColorValue& src, int maxval, double& l, double& a, double& b)
  {
    double rr, gg, bb;
    fromColorValue(src, maxval, rr, gg, bb);
    RgbToLab(rr, gg, bb, l, a, b);
  }

  void RgbToLab(double rr, double gg, double bb, double& l, double& a, double& b)
  {
    double x, y, z;
    RgbToXyz(rr, gg, bb, x, y, z);
    XyzToLab(x, y, z, l, a, b);
  }

  void LabToRgb(double l, double a, double b, double& rr, double& gg, double& bb)
  {
    double x, y, z;
    LabToXyz(l, a, b, x, y, z);
    XyzToRgb(x, y, z, rr, gg, bb);
  }

  void LabToRgb(double l, double a, double b, ColorValue& dst, int maxval)
  {
    double rr, gg, bb;
    LabToRgb(l, a, b, rr, gg, bb);
    toColorValue(rr, gg, bb, dst, maxval);
  }

  //============================================================

  void setPixel(const Image& img, int x, int y, double value)
  {
    img.setPixelLimited(x, y, RoundInt(value * img.maxval));
  }

#define FNAME "ColorImageToLab"
  int ColorImageToLab(const ColorImage& src,
                      const Image& lImg, const Image& aImg, const Image& bImg)
  {
    if (!src.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);
    int xs = src.xsize;
    int ys = src.ysize;
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(lImg, aImg, bImg, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double xx, yy, zz;
          ColorValue rgb = src.getPixel(x, y);
          RgbToXyz(rgb, src.maxval, xx, yy, zz);
          double ll, aa, bb;
          XyzToLab(xx, yy, zz, ll, aa, bb);
          setPixel(lImg, x, y, ll / 100);
          setPixel(aImg, x, y, aa / 200.0 + 0.5);
          setPixel(bImg, x, y, bb / 200.0 + 0.5);
        }
    return OK;
  }
#undef FNAME
#define FNAME "LabToColorImage"
  int LabToColorImage(const Image& lImg, const Image& aImg, const Image& bImg,
                      const ColorImage& dst)
  {
    if (!dst.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    int xs = dst.xsize;
    int ys = dst.ysize;

    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(lImg, aImg, bImg, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double l = (double)lImg.getPixel(x, y) / lImg.maxval * 100;
          double a = ((double)aImg.getPixel(x, y) / aImg.maxval - 0.5) * 200 ;
          double b = ((double)bImg.getPixel(x, y) / bImg.maxval - 0.5) * 200;
          ColorValue cv;
          LabToRgb(l, a, b, cv, dst.maxval);
          dst.setPixelLimited(x, y, cv);
        }
    return OK;
  }
#undef FNAME
#define FNAME "ColorImageToHsi"
  int ColorImageToHsi(const ColorImage& src,
                      const Image& hue, const Image& saturation, const Image& intensity)
  {
    if (!src.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);
    int xs = src.xsize;
    int ys = src.ysize;
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(hue, saturation, intensity, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double h, s, i;
          RgbToHsi(src.getPixel(x, y), h, s, i);
          setPixel(hue, x, y, h);
          setPixel(saturation, x, y, s);
          setPixel(intensity, x, y, i);
        }

    return OK;
  }
#undef FNAME
#define FNAME "HsiToColorImage"
  int HsiToColorImage(const Image& hue, const Image& saturation, const Image& intensity,
                      const ColorImage& dst)
  {
    if (!dst.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);

    int xs = dst.xsize;
    int ys = dst.ysize;

    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(hue, saturation, intensity, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double h = (double)hue.getPixel(x, y) / hue.maxval;
          double s = (double)saturation.getPixel(x, y) / saturation.maxval;
          double i = (double)intensity.getPixel(x, y) / intensity.maxval;
          ColorValue cv;
          HsiToRgb(h, s, i, cv, dst.maxval);
          dst.setPixelLimited(x, y, cv);
        }

    return OK;
  }

  int ColorImageToYuv(const ColorImage& src,
                      const Image& yImg, const Image& uImg, const Image& vImg)
  {
    if (!src.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);
    int xs = src.xsize;
    int ys = src.ysize;
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(yImg, uImg, vImg, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double yy, uu, vv;
          ColorValue rgb = src.getPixel(x, y);
          RgbToYuv(rgb, src.maxval, yy, uu, vv);
          setPixel(yImg, x, y, yy);
          setPixel(uImg, x, y, uu + 0.5);
          setPixel(vImg, x, y, vv + 0.5);
        }
    return OK;
  }

  int YuvToColorImage(const Image& yImg, const Image& uImg, const Image& vImg,
                      const ColorImage& dst)
  {
    if (!dst.isValid())
      throw IceException(FNAME, M_WRONG_IMAGE);
    int xs = dst.xsize;
    int ys = dst.ysize;
    int dx, dy;

    RETURN_ERROR_IF_FAILED(MatchImg(yImg, uImg, vImg, dx, dy));

    if (dx != xs || dy != ys)
      throw IceException(FNAME, M_SIZES_DIFFER);

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          double yy = (double)yImg.getPixel(x, y) / yImg.maxval ;
          double uu = (double)uImg.getPixel(x, y) / uImg.maxval - 0.5;
          double vv = (double)vImg.getPixel(x, y) / vImg.maxval - 0.5;
          ColorValue cv;
          YuvToRgb(yy, uu, vv, cv, dst.maxval);
          dst.setPixelLimited(x, y, cv);
        }
    return OK;
  }
}

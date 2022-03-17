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
#ifndef IMAGE3D_H
#define IMAGE3D_H

#include <vector>
#include <string>

#include "arith.h"
#include "Window3d.h"
#include "base.h"

#include "dirfunc.h"
#include "picio.h"

namespace ice
{
  template<typename TImage>
  class Image3d
  {
  public:
    typedef typename TImage::ValueType ValueType;

    Image3d():
      seq(), xsize(0), ysize(0), zsize(0),  maxval(0),
      scale_x(0), scale_y(0), scale_z(0) {}

    Image3d(const Image3d& src, Window3d w):
      xsize(w.p2.x - w.p1.x + 1), ysize(w.p2.y - w.p1.y + 1), zsize(w.p2.z - w.p1.z + 1), maxval(src.maxval),
      scale_x(src.scale_x), scale_y(src.scale_y), scale_z(src.scale_z)
    {
      Window ws(w.p1.x, w.p1.y, w.p2.x, w.p2.y);
      for (int z = w.p1.z; z <= w.p2.z; z++)
        {
          seq.push_back(src[z](ws));
        }
    }

    virtual ~Image3d() {}

    static Image3d<TImage> createImage3d(int xs, int ys, int zs, int maxval = 255,
                                         double xscale = 1.0, double yscale = 1.0, double zscale = 1.0)
    {
      Image3d<TImage> res;
      res.create(xs, ys, zs, maxval, xscale, yscale, zscale);
      return res;
    }

    static Image3d<TImage> createImage3d(const std::string& filemask,
                                         double xscale = 1.0, double yscale = 1.0, double zscale = 1.0)
    {
      Image3d<TImage> res;
      res.create(filemask, xscale, yscale, zscale);
      return res;
    }

    static Image3d<TImage> createImage3d(const std::vector<std::string>& files,
                                         double xscale = 1.0, double yscale = 1.0, double zscale = 1.0)
    {
      Image3d<TImage> res;
      res.create(files, xscale, yscale, zscale);
      return res;
    }

    virtual void create(int xs, int ys, int zs, int maxvalue = 255,
                        double xscale = 1.0, double yscale = 1.0, double zscale = 1.0)
    {
      xsize = xs;
      ysize = ys;
      zsize = zs;
      maxval = maxvalue;
      scale_x = xscale;
      scale_y = yscale;
      scale_z = zscale;
      seq.clear();
      for (int i = 0; i < zsize; i++)
        {
          seq.push_back(NewImg(xsize, ysize, maxval));
        }
    }

    virtual void create(const Image3d<TImage>& src)
    {
      if (src.isValid())
        {
          create(src.xsize, src.ysize, src.zsize, src.maxval, src.scale_x, src.scale_y, src.scale_z);
        }
    }
#define FNAME "Image3d::create"
    virtual void create(const std::string& filemask,
                        double xscale = 1.0, double yscale = 1.0, double zscale = 1.0)
    {
      try
        {
          seq.clear();
          std::vector<std::string> fn;
          Directory(fn, filemask, DIR_FILE | DIR_WITHPATH);
          zsize = fn.size();

          if (zsize == 0)
            throw IceException("", M_NOT_FOUND, filemask);

          seq.resize(zsize);

          for (int i = 0; i < zsize; i++)
            {
              // cout << "lese " << fn[i] << endl;
              seq[i] = ReadImg(fn[i]);
            }
          xsize = seq[0].xsize;
          ysize = seq[0].ysize;
          maxval = seq[0].maxval;

          scale_x = xscale;
          scale_y = yscale;
          scale_z = zscale;
        }
      RETHROW;
    }

    virtual void create(const std::vector<std::string>& files,
                        double xscale, double yscale, double zscale)
    {
      scale_x = xscale;
      scale_y = yscale;
      scale_z = zscale;

      zsize = files.size();
      if (zsize == 0)
        throw IceException(FNAME, M_EMPTY_LIST);
      for (unsigned int i = 0; i < files.size(); i++)
        {
          seq.push_back(ReadImg(files[i]));
        }
      xsize = seq[0].xsize;
      ysize = seq[0].ysize;
      maxval = seq[0].maxval;
    }
#undef FNAME

    bool isValid() const
    {
      return !seq.empty();
    }

#define FNAME "Image3d::read"
    int read(const std::string& filemask)
    {
      std::vector<std::string> fn;
      Directory(fn, filemask, DIR_FILE | DIR_WITHPATH);
      if ((int)fn.size() == zsize)
        {
          for (unsigned int i = 0; i < fn.size(); i++)
            {
              RETURN_ERROR_IF_FAILED(ReadImg(fn[i], seq[i]));
            }
        }
      else
        {
          throw IceException(FNAME, M_WRONG_IMGSIZE);
        }
      return OK;
    }
#undef FNAME

#define FNAME "Image3d::copy"
    int copy(const Image3d& src)
    {
      create(src);
      for (int z = 0; z < src.zsize; z++)
        {
          seq[z].copy(src[z]);
        }
      return OK;
    }
#undef FNAME

    // Access with pixel index
    bool inside(IVector3d p) const
    {
      return p.x >= 0 && p.x < xsize && p.y >= 0 && p.y < ysize && p.z >= 0 && p.z < zsize;
    }

    bool inside(int x, int y, int z) const
    {
      return x >= 0 && x < xsize && y >= 0 && y < ysize && z >= 0 && z < zsize;
    }

#define FNAME "Image3d::getPixel"
    ValueType getPixel(int x, int y, int z) const
    {
      if (x < 0)
        throw IceException(FNAME, M_XTOOSMALL);
      else if (x >= xsize)
        throw IceException(FNAME, M_XTOOLARGE);
      else if (y < 0)
        throw IceException(FNAME, M_YTOOSMALL);
      else if (y >= ysize)
        throw IceException(FNAME, M_YTOOLARGE);
      else if (z < 0)
        throw IceException(FNAME, M_ZTOOSMALL);
      else if (z >= zsize)
        throw IceException(FNAME, M_ZTOOLARGE);
      else
        {
          return seq[z].getPixelUnchecked(x, y);
        }
    }
#undef FNAME

    ValueType getPixel(IVector3d p) const
    {
      return getPixel(p.x, p.y, p.z);
    }

#define FNAME "Image3d::setPixel"
    void setPixel(int x, int y, int z, ValueType val) const
    {
      if (x < 0)
        throw IceException(FNAME, M_XTOOSMALL);
      else if (x >= xsize)
        throw IceException(FNAME, M_XTOOLARGE);
      else if (y < 0)
        throw IceException(FNAME, M_YTOOSMALL);
      else if (y >= ysize)
        throw IceException(FNAME, M_YTOOLARGE);
      else if (z < 0)
        throw IceException(FNAME, M_ZTOOSMALL);
      else if (z >= zsize)
        throw IceException(FNAME, M_ZTOOLARGE);
      else
        {
          seq[z].setPixelUnchecked(x, y, val);
        }
    }
#undef FNAME

    void setPixel(IVector3d p, ValueType val) const
    {
      setPixel(p.x, p.y, p.z, val);
    }

    void getSize(int& xa, int& ya, int& za) const
    {
      xa = xsize;
      ya = ysize;
      za = zsize;
    }

    TImage& operator[](int i)
    {
      if (i < 0 || i >= (int)seq.size())
        {
          throw "z index out of range";
        }
      return seq[i];
    }

    const TImage& operator[](int i) const
    {
      if (i < 0 || i >= (int)seq.size())
        {
          throw "z index out of range";
        }
      return seq[i];
    }

#define FNAME "Image::match"
    void match(const Image3d<TImage>& img2) const
    {
      if (!isValid() || !img2.isValid())
        throw IceException(FNAME, M_WRONG_IMAGE);

      if ((xsize != img2.xsize) || (ysize != img2.ysize) || (zsize != img2.zsize))
        throw IceException(FNAME, M_WRONG_IMGSIZE);
    }

    void match(const Image3d<TImage>& img2, const Image3d<TImage>& img3) const
    {
      match(img2);
      match(img3);
    }
#undef FNAME

    // Access with "physical" position
    void d2i(double x, double y, double z, int& xi, int& yi, int& zi) const
    {
      xi = RoundInt(x / scale_x);
      yi = RoundInt(y / scale_y);
      zi = RoundInt(z / scale_z);
    }

    void i2d(int xi, int yi, int zi, double& x, double& y, double& z) const
    {
      x = xi * scale_x;
      y = yi * scale_y;
      z = zi * scale_z;
    }

    void d2i(Vector3d lp, IVector3d& pp) const
    {
      pp.x = RoundInt(lp.x / scale_x);
      pp.y = RoundInt(lp.y / scale_y);
      pp.z = RoundInt(lp.z / scale_z);
    }

    void i2d(IVector3d lp, Vector3d& pp) const
    {
      pp.x = lp.x * scale_x;
      pp.y = lp.y * scale_y;
      pp.z = lp.z * scale_z;
    }

    bool insideD(double x, double y, double z) const
    {
      return x >= 0 && x < xsize * scale_x &&
             y >= 0 && y < ysize * scale_y &&
             z >= 0 && z < zsize * scale_z;
    }

#define FNAME "Image3d::getValue"
    double getValue(double x, double y, double z) const
    {
      x /= scale_x;
      y /= scale_y;
      z /= scale_z;

      if (z < -0.5)
        throw IceException(FNAME, M_ZTOOSMALL);
      else if (z > zsize)
        throw IceException(FNAME, M_ZTOOLARGE);
      else if (z < 0)
        {
          GetInterpolVal(seq[0], x, y);
        }
      if (z > zsize - 1)
        {
          return GetInterpolVal(seq.back(), x, y);
        }

      int zi = (int)z;
      double zr = z - zi;

      double v1 = GetInterpolVal(seq[zi], x, y);
      double v2 = v1;
      if (zi + 1 < (int)seq.size())
        {
          v2 = GetInterpolVal(seq[zi + 1], x, y);
        }
      return v1 * (1 - zr) + v2 * zr;
    }
#undef FNAME

    void getScale(double& xa, double& ya, double& za) const
    {
      xa = scale_x;
      ya = scale_y;
      za = scale_z;
    }

    void getSizeD(double& xa, double& ya, double& za) const
    {
      xa = xsize * scale_x;
      ya = ysize * scale_y;
      za = zsize * scale_z;
    }

    int set(int val) const
    {
      int rc = 0;
      for (int i = 0; i < (int)seq.size() && rc == 0; i++)
        {
          rc = seq[i].set(val);
        }
      return rc;
    }

    int clear() const
    {
      return set(0);
    }

  protected:
    std::vector<TImage> seq;

  public:
    // size of image in pixel
    int xsize;
    int ysize;
    int zsize;
    int maxval;

  protected:
    // "physical" size of pixel
    double scale_x;
    double scale_y;
    double scale_z;
  };
}
#endif

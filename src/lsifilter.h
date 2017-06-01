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
/************************************************/
/*            Filter                            */
/************************************************/
#ifndef _LSIFILTER_H
#define _LSIFILTER_H

#include <iostream>

#include "matrixtemplate.h"
#include "base.h"
#include "Matrix.h"

#include "LsiRepresentation.h"
#include "LsiRepresentationD.h"
#include "LsiRepresentationI.h"

namespace ice
{
  class LSIFilter
  {
  public:
    typedef enum {it_int, it_double} impl_type;

    impl_type typ;
    LsiRepresentation* rep;

    LSIFilter(): typ(it_int), rep(nullptr) {}
    LSIFilter(const LSIFilter& f): typ(f.typ)
    {
      if (f.rep != nullptr)
        {
          switch (typ)
            {
            case it_int:
              rep = new LsiRepresentationI(*(LsiRepresentationI*)f.rep);
              break;
            case it_double:
              rep = new LsiRepresentationD(*(LsiRepresentationD*)f.rep);
              break;
            }
        }
      else
        {
          rep = nullptr;
        }
    }

    explicit LSIFilter(const Matrix& m, bool convolution = false): typ(it_double)
    {
      rep = new LsiRepresentationD(m);
      if (convolution)
        {
          rep->reflectMask();
        }
    }

    LSIFilter(const IMatrix& m, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, norm);
      if (convolution)
        {
          rep->reflectMask();
        }
    }

    explicit LSIFilter(const matrix<double>& m, bool convolution = false): typ(it_double)
    {
      rep = new LsiRepresentationD(m);
      if (convolution)
        {
          rep->reflectMask();
        }
    }

    LSIFilter(const matrix<int>& m, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, norm);
      if (convolution)
        {
          rep->reflectMask();
        }
    }

    LSIFilter(int* m, int sizex, int sizey, int norm, bool convolution = false): typ(it_int)
    {
      rep = new LsiRepresentationI(m, sizex, sizey, norm);
      if (convolution)
        {
          rep->reflectMask();
        }
    }

    ~LSIFilter()
    {
      delete rep;
    }

    virtual void Filter(const Image& src, const Image& dst, int offset/*=0*/) const
    {
      rep->Filter(src, dst, offset);
    }
    virtual void Filter(const Image& src, ImageD dst) const
    {
      rep->Filter(src, dst);
    }
    virtual void Filter(ImageD src, ImageD dst) const
    {
      rep->Filter(src, dst);
    }

    virtual double getMask(int x, int y) const
    {
      return rep->getMask(x, y);
    }

    virtual void getMask(matrix<double>& m) const
    {
      rep->getMask(m);
    }

    virtual void NegateMask()
    {
      rep->negateMask();
    }

    virtual int getDX() const { return rep->getDX();}
    virtual int getDY() const { return rep->getDY();}
    virtual int getXDimension() const { return rep->getXDimension();}
    virtual int getYDimension() const { return rep->getYDimension();}

    virtual LSIFilter getInverse(int sizex, int sizey) const;
    virtual LSIFilter getInverse(int size) const
    {
      return getInverse(size, size);
    }

    virtual double proposeOffset(const Image& img) const
    {
      return rep->proposeoffset(img);
    }

    virtual LSIFilter& operator=(const LSIFilter& f);
  };

  std::ostream& operator<<(std::ostream& os, const LSIFilter& f);

  // basic implementation functions - lsifilter0.cpp
  // not for public use, no parameter check
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, int* mask, int norm, int off);
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, double* mask, int off);

  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, int* mask, int norm);
  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, double* mask);

  void lsiimg(ImageD src, ImageD dest, int nx, int ny,
              int* mask, int norm);
  void lsiimg(ImageD src, ImageD dest,
              int nx, int ny, double* mask);

  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, int* mask, int norm, int off);
  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, double* mask, int off);

  // second level function - with parameter check
  // use not recommended
  void LSIImg(const Image& src, const Image& dst,
              int nx, int ny, int* mask, int norm, int off);
  void LSIImg(const Image& src, const Image& dst,
              int nx, int ny, double* mask, int off);

  void LSIImg(const Image& src, ImageD dst,
              int nx, int ny, int* mask, int norm = 1);
  void LSIImg(const Image& src, ImageD dst,
              int nx, int ny, double* mask);

  void LSIImg(ImageD src, ImageD dst,
              int nx, int ny, int* mask, int norm = 1);
  void LSIImg(ImageD src, ImageD dst,
              int nx, int ny, double* mask);

  // different parameter order (for compatibility)
  void LSIImg(const Image& imgs,
              int nx, int ny, int* mask, int norm,
              int off, const Image& imgd);
  void LSIImg(const Image& imgs,
              int nx, int ny, double* mask,
              int off, const Image& imgd);

// variants with "cyclic" interpretation of values (like hue or angles)
  void LSIImgCyc(const Image&, const Image&,
                 int nx, int ny, int* mask, int norm, int off);
  void LSIImgCyc(const Image&, const Image&,
                 int nx, int ny, double* mask, int off);

  // special access to pixel value through filter
  int GetVal(const Image& img, int x, int y, const LSIFilter& f);
  int GetVal(const Image& img, IPoint p, const LSIFilter& f);

  // Filtering with mask given as matrix
  void LSIImg(const Image&, const Image&,
              const IMatrix& m, int norm, int off);
  void LSIImg(const Image&, const Image&,
              const Matrix& m, int off);
  // different parameter order (for compatibility)
  void LSIImg(const Image& imgs, const IMatrix& m, int norm, int off, const Image& imgd);
  void LSIImg(const Image& imgs, const Matrix& m, int off, const Image& imgd);

  // !! Preferred filter function with LSIFilter class parameter
  // equivalent to f.Filter(src,dest)
  void LSIImg(const Image& src, const Image& dest,
              const LSIFilter& f, int offset);

  // filtering for double images
  void LSIImg(ImageD imgs, ImageD imgd,
              const Matrix& m);

  // special implementations for row or column vector
  void LSIHImg(const Image& pn1, const Image& pn2, const std::vector<int>& mask,
               int norm, int offset/*=0*/);
  void LSIHImg(const Image& pn1, const Image& pn2, const IVector& mask,
               int norm, int offset/*=0*/);
  void LSIVImg(const Image& pn1, const Image& pn2, const std::vector<int>& mask,
               int norm, int offset/*=0*/);
  void LSIVImg(const Image& pn1, const Image& pn2, const IVector& mask,
               int norm, int offset/*=0*/);

// functions for generation of LSIFilters
  LSIFilter makeMexicanHatFilter(double sigma, int size);
  LSIFilter makePolynomFilter(int size, int grad, int ii, int jj);
 
  LSIFilter makeOrientedSmearFilter(int n, double dir, double len, double width);
  LSIFilter makeOrientedDoBFilter(int n, double dir, double len, double width);
  LSIFilter makeOrientedEdgeFilter(int n, double dir, double rad);

}
#endif

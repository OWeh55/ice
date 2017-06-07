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
//#include "LsiRepresentationI.h"

namespace ice
{
  class LsiFilter
  {
  public:

    LsiFilter(): type(it_double), rep(nullptr) {}
    LsiFilter(const LsiFilter& f): type(f.type)
    {
      if (f.rep != nullptr)
        {
          switch (type)
            {
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

    explicit LsiFilter(const Matrix& m): type(it_double)
    {
      rep = new LsiRepresentationD(m);
    }

    LsiFilter(const IMatrix& m, int norm): type(it_double)
    {
      matrix<double> md(m.rows(), m.cols());
      for (int r = 0; r < m.rows(); r++)
        for (int c = 0; c < m.cols(); c++)
          md[r][c] = static_cast<double>(m[r][c]) / norm;
      rep = new LsiRepresentationD(md);
    }

    explicit LsiFilter(const matrix<double>& m): type(it_double)
    {
      rep = new LsiRepresentationD(m);
    }

    LsiFilter(const matrix<int>& m, int norm): type(it_double)
    {
      matrix<double> md(m.rows(), m.cols());
      for (int r = 0; r < m.rows(); r++)
        for (int c = 0; c < m.cols(); c++)
          md[r][c] = static_cast<double>(m[r][c]) / norm;
      rep = new LsiRepresentationD(md);
    }

    ~LsiFilter()
    {
      delete rep;
    }

    virtual void filter(const Image& src, const Image& dst, int offset/*=0*/) const
    {
      rep->filter(src, dst, offset);
    }
    virtual void filter(const Image& src, ImageD dst) const
    {
      rep->filter(src, dst);
    }
    virtual void filter(ImageD src, ImageD dst) const
    {
      rep->filter(src, dst);
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

    virtual int getDX() const
    {
      return rep->getDX();
    }
    virtual int getDY() const
    {
      return rep->getDY();
    }

    virtual int getXDimension() const
    {
      return rep->getXDimension();
    }
    virtual int getYDimension() const
    {
      return rep->getYDimension();
    }

    virtual const LsiRepresentation* getRepresentation() const
    {
      return rep;
    }

    virtual LsiFilter getInverse(int sizex, int sizey) const;
    virtual LsiFilter getInverse(int size) const
    {
      return getInverse(size, size);
    }

    virtual double proposeOffset(const Image& img) const
    {
      return rep->proposeOffset(img);
    }

    virtual LsiFilter& operator=(LsiFilter f);

    typedef enum {it_double} impl_type;

    impl_type getImplementationType() const
    {
      return type;
    }
  private:
    impl_type type;
    LsiRepresentation* rep;
  };

  std::ostream& operator<<(std::ostream& os, const LsiFilter& f);

  // basic implementation functions - lsifilter0.cpp
  // not for public use, no parameter check
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, const int* mask, int norm, int off);
  void lsiimg(const Image& src, const Image& dest,
              int nx, int ny, const double* mask, int off);

  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, const int* mask, int norm);
  void lsiimg(const Image& src, ImageD dest,
              int nx, int ny, const double* mask);

  void lsiimg(ImageD src, ImageD dest, int nx, int ny,
              const int* mask, int norm);
  void lsiimg(ImageD src, ImageD dest,
              int nx, int ny, const double* mask);

  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, const int* mask, int norm, int off);
  void lsiimgcyc(const Image& src, const Image& dest,
                 int nx, int ny, const double* mask, int off);

  // second level functions - with parameter check
  // public use not recommended
  void LSIImg(const Image& src, const Image& dst,
              int nx, int ny, const int* mask, int norm, int off);
  void LSIImg(const Image& src, const Image& dst,
              int nx, int ny, const double* mask, int off);

  void LSIImg(const Image& src, ImageD dst,
              int nx, int ny, const int* mask, int norm = 1);
  void LSIImg(const Image& src, ImageD dst,
              int nx, int ny, const double* mask);

  void LSIImg(ImageD src, ImageD dst,
              int nx, int ny, const int* mask, int norm = 1);
  void LSIImg(ImageD src, ImageD dst,
              int nx, int ny, const double* mask);

  // different parameter order (for compatibility)
  void LSIImg(const Image& imgs,
              int nx, int ny, const int* mask, int norm,
              int off, const Image& imgd);
  void LSIImg(const Image& imgs,
              int nx, int ny, const double* mask,
              int off, const Image& imgd);

// variants with "cyclic" interpretation of values (like hue or angles)
  void LSIImgCyc(const Image&, const Image&,
                 int nx, int ny, const int* mask, int norm, int off);
  void LSIImgCyc(const Image&, const Image&,
                 int nx, int ny, const double* mask, int off);

  // special access to pixel value through filter
  int getValueFiltered(const Image& img, int x, int y, const LsiFilter& f);
  int getValueFiltered(const Image& img, IPoint p, const LsiFilter& f);

  // Filtering with mask given as matrix
  void LSIImg(const Image&, const Image&,
              const IMatrix& m, int norm, int off);
  void LSIImg(const Image&, const Image&,
              const Matrix& m, int off);
  // different parameter order (for compatibility)
  void LSIImg(const Image& imgs, const IMatrix& m, int norm, int off, const Image& imgd);
  void LSIImg(const Image& imgs, const Matrix& m, int off, const Image& imgd);

  // !! Preferred filter function with LsiFilter class parameter
  // equivalent to f.filter(src, dest)
  void LSIImg(const Image& src, const Image& dest,
              const LsiFilter& f, int offset);

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

// functions for generation of LsiFilters
  LsiFilter makeGaussFilter(int size, double sigma);
  LsiFilter makeMexicanHatFilter(int size, double sigma);
  LsiFilter makePolynomFilter(int size, int grad, int ii, int jj);

  LsiFilter makeOrientedSmearFilter(int n, double dir, double len, double width);
  LsiFilter makeOrientedDoBFilter(int n, double dir, double len, double width);
  LsiFilter makeOrientedEdgeFilter(int n, double dir, double rad);
}
#endif

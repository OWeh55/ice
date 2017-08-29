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
#ifndef TRAFO_H
#define TRAFO_H

#include <vector>

#include "defs.h"
#include "matrixtemplate.h"

#include "Vector3d.h"
#include "Matrix.h"

#include "Point.h"

namespace ice
{
#define TRM_REFLECTION 16
#define TRM_SCALE_SHIFT 32

#define TRM_SHIFT 1
#define TRM_EUCLIDEAN_NOR 2
#define TRM_SIMILARITY_NOR 3
#define TRM_AFFINE_NOR 4
// TRM_PROJECTIVE_NOR doesn't make sense. It's only defined to
// define TRM_PROJECTIV later with TRM_REFLECTION
#define TRM_PROJECTIVE_NOR 5

#define TRM_EUCLIDEAN ( TRM_EUCLIDEAN_NOR | TRM_REFLECTION )
#define TRM_SIMILARITY ( TRM_SIMILARITY_NOR | TRM_REFLECTION )
#define TRM_AFFINE ( TRM_AFFINE_NOR | TRM_REFLECTION )
#define TRM_PROJECTIVE ( TRM_PROJECTIVE_NOR | TRM_REFLECTION )

#define HAS_REFLECTION(x) (x & TRM_REFLECTION)

// bit masks for "panoramic" transform
#define PANO_ANISOTROP 1
#define PANO_SHEARING 2

  class Trafo
  {
  public:
    Trafo();
    Trafo(const Trafo&);
    Trafo(const Matrix& m);
    Trafo(const matrix<double>& m);
    Trafo(int dimSourcep, int dimTargetp);
    virtual ~Trafo();

    virtual Trafo& operator =(const Trafo&);
    virtual Trafo& operator =(const Matrix&);
    virtual double& operator()(int i, int j);
    virtual const double& operator()(int i, int j) const;
    virtual int DimSource() const;
    virtual int DimTarget() const;

    virtual void init();

    // these methods *append* transformations to existing one

    virtual void shift(double x0, double y0);
    virtual void shift(double x0, double y0, double z0);
    virtual void shift(Vector3d v);
    virtual void shift(const ice::Vector& v);
    virtual void shift(const ice::Point& p)
    {
      return shift(p.x, p.y);
    }

    virtual void rotate(double x0, double y0, double phi);
    virtual void rotate(Vector3d point, Vector3d axis, double phi);

    // Eulersche Winkel!
    virtual void rotateX(double phi);
    virtual void rotateY(double phi);
    virtual void rotateZ(double phi);

    // euclidian move in 3d
    // shift (dx, dy, dz) + z-rotation alpha + y-rotation beta + x-rotation gamma
    virtual void move(double dx, double dy, double dz, double alpha, double beta, double gamma);
    // shift d + z-rotation alpha + y-rotation beta + x-rotation gamma
    virtual void move(Vector3d d, double alpha, double beta, double gamma);

    virtual void flip(int axis);

    virtual void shearY(double dyx);
    virtual void shearX(double dxy);

    virtual void scale(double x0, double y0, double f);
    virtual void scale(double x0, double y0, double fx, double fy);
    virtual void scale(Vector3d center, double f);
    virtual void scale(Vector3d center, double fx, double fy, double fz);
    virtual void scale(const ice::Vector& center, double f);
    virtual void scale(const ice::Vector& center, const ice::Vector& f);

    virtual void Projective();

    // append another Trafo
    virtual void append(const Trafo& tr);

    // prepend another Trafo
    virtual void prepend(const Trafo& tr);

    virtual void invert();

    virtual Trafo inverse() const;

    //    virtual Matrix getMatrix() const;
    // matrix must not / cannot be changed
    virtual const matrix<double>& getMatrix() const
    {
      return m;
    }

    // product t1 * t2 is trafo t2 followed by t1 (like matrix product)!!!
    friend Trafo operator *(const Trafo& t1, const Trafo& t2);
    // apply trafo to vector
    friend ice::Vector operator *(const Trafo&, const ice::Vector&);
    friend ice::Vector3d operator *(const Trafo&, const ice::Vector3d&);
    friend ice::Point operator *(const Trafo&, const ice::Point&);

    // transformations in 2d
    friend void transform(const Trafo&, double& x, double& y);
    friend void transform(const Trafo&,
                          double x, double y,
                          double& xt, double& yt);

    // transformations in 3d
    friend void transform(const Trafo&, double& x, double& y, double& z);
    friend void transform(const Trafo&,
                          double x, double y, double z,
                          double& xt, double& yt, double& zt);

    // transformations 3d to 2d
    friend void transform(const Trafo&,
                          double x, double y, double z,
                          double& xt, double& yt);

    friend void transformList(const Trafo& tr, Matrix& m);
    friend void transformList(const Trafo& tr, const Matrix& m1, Matrix& m2);

    friend Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                                 int mode, const ice::Vector& weight);

  protected:
    void normalize();
    matrix<double> m;
    int dimSource, dimTarget;
  };

  Trafo operator *(const Trafo&, const Trafo&);
  ice::Vector operator *(const Trafo&, const ice::Vector&);

// transform in 2d
  void transform(const Trafo&, double& x, double& y);
  void transform(const Trafo&, double x, double y, double& xt, double& yt);
  void transform(const Trafo&, Point p1, Point& p2);
  void transform(const Trafo&, Point& p);

  void transform(const Trafo&, std::vector<Point>& p);
  void transform(const Trafo&, const std::vector<Point>& p1, std::vector<Point>& p2);

  // "Round" in name of function to avoid accidentally rounding
  void transformAndRound(const Trafo& tr, double x, double y, int& xt, int& yt);
  void transformAndRound(const Trafo& tr, int& xt, int& yt);

// transform in 3d
  void transform(const Trafo&, double& x, double& y, double& z);
  void transform(const Trafo&, double x, double y, double z,
                 double& xt, double& yt, double& zt);

// transform 3d to 2d
  void transform(const Trafo&, double x, double y, double z,
                 double& xt, double& yt);
}
#endif

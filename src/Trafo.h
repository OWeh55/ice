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

    virtual void Init();

    // these methods *append* transformations to existing one

    virtual int Shift(double x0, double y0);
    virtual int Shift(double x0, double y0, double z0);
    virtual int Shift(Vector3d v);
    virtual int Shift(const ice::Vector& v);
    virtual int Shift(const ice::Point& p)
    {
      return Shift(p.x, p.y);
    }

    virtual int Rotate(double x0, double y0, double phi);
    virtual int Rotate(Vector3d point, Vector3d axis, double phi);

    // Eulersche Winkel!
    virtual int RotateX(double phi);
    virtual int RotateY(double phi);
    virtual int RotateZ(double phi);

    // euclidian move in 3d
    // shift (dx, dy, dz) + z-rotation alpha + y-rotation beta + x-rotation gamma
    virtual int Move(double dx, double dy, double dz, double alpha, double beta, double gamma);
    // shift d + z-rotation alpha + y-rotation beta + x-rotation gamma
    virtual int Move(Vector3d d, double alpha, double beta, double gamma);

    virtual int Flip(int axis);

    virtual int ShearY(double dyx);
    virtual int ShearX(double dxy);

    virtual int Scale(double x0, double y0, double f);
    virtual int Scale(double x0, double y0, double fx, double fy);
    virtual int Scale(Vector3d center, double f);
    virtual int Scale(Vector3d center, double fx, double fy, double fz);
    virtual int Scale(const ice::Vector& center, double f);
    virtual int Scale(const ice::Vector& center, const ice::Vector& f);

    virtual int Projective();

    // append another Trafo
    virtual int append(const Trafo& tr);
    // prepend another Trafo
    virtual int prepend(const Trafo& tr);

    virtual int Invert();

    virtual Trafo Inverse() const;

    virtual Matrix Tmatrix() const;

    friend Trafo operator *(const Trafo&, const Trafo&);
    friend ice::Vector operator *(const Trafo&, const ice::Vector&);
    friend ice::Vector3d operator *(const Trafo&, const ice::Vector3d&);
    friend ice::Point operator *(const Trafo&, const ice::Point&);

    // transformations in 2d
    friend int Transform(const Trafo&, double& x, double& y);
    friend int Transform(const Trafo&,
                         double x, double y,
                         double& xt, double& yt);

    // transformations in 3d
    friend int Transform(const Trafo&, double& x, double& y, double& z);
    friend int Transform(const Trafo&,
                         double x, double y, double z,
                         double& xt, double& yt, double& zt);

    // transformations 3d to 2d
    friend int Transform(const Trafo&,
                         double x, double y, double z,
                         double& xt, double& yt);

    friend int TransformList(const Trafo& tr, Matrix& m);
    friend int TransformList(const Trafo& tr, const Matrix& m1, Matrix& m2);

    friend Trafo MatchPointlists(const Matrix& p1, const Matrix& p2,
                                 int mode, const ice::Vector& weight);

    const matrix<double>& getMatrix() const
    {
      return m;
    }

  protected:
    int normalize();
    matrix<double> m;
    int dimSource, dimTarget;
  };

  Trafo operator *(const Trafo&, const Trafo&);
  ice::Vector operator *(const Trafo&, const ice::Vector&);

// transform in 2d
  int Transform(const Trafo&, double& x, double& y);
  int Transform(const Trafo&, double x, double y, double& xt, double& yt);
  int Transform(const Trafo&, Point p1, Point& p2);
  int Transform(const Trafo&, Point& p);

  int Transform(const Trafo&, std::vector<Point>& p);
  int Transform(const Trafo&, const std::vector<Point>& p1, std::vector<Point>& p2);

  // rounding in name of function to avoid accidentally rounding
  int TransformAndRound(const Trafo& tr, int x, int y, int& xt, int& yt);
  int TransformAndRound(const Trafo& tr, int& xt, int& yt);

// transform in 3d
  int Transform(const Trafo&, double& x, double& y, double& z);
  int Transform(const Trafo&, double x, double y, double z,
                double& xt, double& yt, double& zt);

// transform 3d to 2d
  int Transform(const Trafo&, double x, double y, double z,
                double& xt, double& yt);

}
#endif

#ifndef _LSI_FILTER_REPRESENTATION_H
#define _LSI_FILTER_REPRESENTATION_H

#include "ImageBase.h"
#include "based.h"

namespace ice
{
  class LsiRepresentation
  {
  public:
    LsiRepresentation(int nx, int ny);
    virtual ~LsiRepresentation() {}

    int getXDimension() const
    {
      return dimx;
    }

    int getYDimension() const
    {
      return dimy;
    }

    int getDX() const
    {
      return (dimx - 1) / 2;
    }

    int getDY() const
    {
      return (dimy - 1) / 2;
    }

    int proposeoffset(const Image& img) const
    {
      double p, m;
      sumPlusSumMinus(p, m);
      double range = p - m;
      return RoundInt(-m / range * img.maxval);
    }

    virtual void sumPlusSumMinus(double& sump, double& summ) const = 0;
    virtual void normalize() = 0;

    virtual void filter(const Image& src, const Image& dst, int offset) const = 0;
    virtual void filter(const Image& src, ImageD dst) const = 0;
    virtual void filter(ImageD src, ImageD dst) const = 0;

    virtual double getMask(int i, int j) const = 0;

    virtual void getMask(matrix<double>& m) const = 0;
    virtual void negateMask() = 0;
    virtual void reflectMask() = 0;

  protected:
    int idx(int x, int y) const
    {
      return y * dimx + x;
    }

    int dimx;
    int dimy;
  };
}
#endif


#ifndef LSI_FILTER_REPRESENTATION_I_H
#define LSI_FILTER_REPRESENTATION_I_H

#include "matrixtemplate.h"
#include "Matrix.h"

namespace ice
{
  class LsiRepresentationI: public LsiRepresentation
  {
  public:
    LsiRepresentationI(const LsiRepresentationI& r);
    LsiRepresentationI(const IMatrix& m, int normp);
    LsiRepresentationI(const matrix<int>& m, int normp);
    LsiRepresentationI(int* m, int sizex, int sizey, int normp);

    virtual void sumPlusSumMinus(double& sump, double& summ) const;
    virtual LsiRepresentationI normalized(int max) const;

    virtual void negateMask();
    virtual void normalize();
    virtual void reflectMask();

    virtual void filter(const Image& src, const Image& dst, int offset) const;
    virtual void filter(const Image& src, ImageD dst) const;
    virtual void filter(ImageD src, ImageD dst) const;

    virtual ~LsiRepresentationI()
    {
      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual const int* getMask() const
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const;

    virtual double getMask(int x, int y) const
    {
      return static_cast<double>(mask[idx(x, y)]) / norm;
    };

    virtual int getNorm() const
    {
      return norm;
    }
  protected:
    int* mask;
    int norm;

    int ggt(int a, int b) const;
  };
}
#endif

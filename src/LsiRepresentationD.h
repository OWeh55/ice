#ifndef LSI_FILTER_REPRESENTATION_D_H
#define LSI_FILTER_REPRESENTATION_D_H

namespace ice {
  class LsiRepresentationD: public LsiRepresentation
  {
  public:

    LsiRepresentationD(const LsiRepresentationD& r);
    LsiRepresentationD(const Matrix& m);
    LsiRepresentationD(const matrix<double>& m);

    virtual void sumPlusSumMinus(double& sump, double& summ) const;

    virtual void negateMask();
    virtual void normalize();
    virtual void reflectMask();

    virtual void Filter(const Image& src, const Image& dst, int offset) const;
    virtual void Filter(const Image& src, ImageD dst) const;
    virtual void Filter(ImageD src, ImageD dst) const;

    virtual ~LsiRepresentationD()
    {
      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual double getMask(int x, int y) const
    {
      return mask[idx(x, y)];
    };

    virtual const double* getMask() const
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const;

  protected:
    double* mask;
  };
}
#endif

#ifndef LSI_FILTER_REPRESENTATION_D_H
#define LSI_FILTER_REPRESENTATION_D_H

namespace ice
{
  class LsiRepresentationD: public LsiRepresentation
  {
  public:

    LsiRepresentationD(const LsiRepresentationD& r);
    LsiRepresentationD(const Matrix& m);
    LsiRepresentationD(const matrix<double>& m);

    virtual void sumPlusSumMinus(double& sump, double& summ) const override;

    virtual void negateMask() override;
    virtual void normalize() override;
    virtual void reflectMask() override;

    virtual void filter(const Image& src, const Image& dst, int offset) const override;
    virtual void filter(const Image& src, ImageD dst) const override;
    virtual void filter(ImageD src, ImageD dst) const override;

    virtual ~LsiRepresentationD()
    {
      //      delete [] mask;
    }

    // several forms of access to the filter mask
    virtual double getMask(int x, int y) const override
    {
      //      return mask[idx(x, y)];
      return mask[y][x];
    };

    virtual const matrix<double>& getMask() const override
    {
      return mask;
    }

    virtual void getMask(matrix<double>& m) const override;

  protected:
    matrix<double> mask;
    //    double* mask;
  };
}
#endif

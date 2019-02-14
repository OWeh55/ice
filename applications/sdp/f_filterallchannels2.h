#ifndef FILTER_ALL_CHANNELS_TWO_INPUT_H
#define FILTER_ALL_CHANNELS_TWO_INPUT_H

#include "f_ifilter.h"

class FilterAllChannelsTwoInputs: public ImageFilter
{
  // abstract base class for filters that are applied to
  // all channels separately
public:
  FilterAllChannelsTwoInputs(const string& name,
                             int nInput, int nOutput,
                             const std::string& help = "", int matchmode = mm_strict):
    ImageFilter(name, nInput, nOutput, help, matchmode)
  {
    if (nOutput < 5)
      throw SdpException(name, "result array too small for image");
  }
  virtual ~FilterAllChannelsTwoInputs() {}

protected:

  virtual void filterOneChannel(const Image& src1, const Image& src2, const Image& dst) = 0;

  virtual void get_data()
  {
    inp1 = getInputImage(0);
    inp2 = getInputImage(1);
    calcSize(inp1, inp2);
    GImage* tresult = createResult();
    try
      {
        for (int i = 0; i < nImg; ++i) // all channels
          {
            filterOneChannel((*inp1)[i], (*inp2)[i], (*tresult)[i]);
          }
      }
    catch (SdpException& exc)
      {
        exc.setWhere(name);
        throw exc;
      }
  }

  const GImage* inp1;
  const GImage* inp2;
};
#endif

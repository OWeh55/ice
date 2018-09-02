#ifndef FILTER_ALL_CHANNELS_ONE_INPUT_H
#define FILTER_ALL_CHANNELS_ONE_INPUT_H

#include "f_ifilter.h"

class FilterAllChannelsOneInput: public ImageFilter
{
  // abstract base class for filters that are applied to
  // all channels separately
public:
  FilterAllChannelsOneInput(const string& name,
                            int nInput, int nOutput,
                            const std::string& help = "", int matchmode = mm_strict):
    ImageFilter(name, nInput, nOutput, help, matchmode)
  {
    if (nOutput < 5)
      throw SdpException(name, "result array too small for image");
  }
  virtual ~FilterAllChannelsOneInput() {}

protected:

  /**
   * \brief create result image
   *
   * Creates the result image on the base of the estimated size\n
   * override this if different size is needed
   */

  virtual void filterOneChannel(const Image& src, const Image& dst) = 0;

  virtual void get_data()
  {
    inp = getInputImage(0);
    calcSize(inp);
    GImage* tresult = createResult(0);
    try
      {
        for (int i = 0; i < nImg; ++i) // all channels
          {
            filterOneChannel((*inp)[i], (*tresult)[i]);
          }
      }
    catch (SdpException exc)
      {
        exc.setWhere(name);
        throw exc;
      }
  }
  const GImage* inp;
};
#endif

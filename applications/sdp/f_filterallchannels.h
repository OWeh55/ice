#ifndef FILTER_ALL_CHANNELS_H
#define FILTER_ALL_CHANNELS_H

#include "f_ifilter.h"

class FilterAllChannels: public ImageFilter
{
  // abstract base class for filters that are applied to
  // all channels separately with arbitrary number of input images
  // special classes for one and two input images exist
public:
  FilterAllChannels(const string& name,
                    int nInput, int nOutput,
                    const std::string& help = "", int matchmode = mm_strict):
    ImageFilter(name, nInput, nOutput, help, matchmode)
  {
    if (nOutput < 5)
      throw SdpException(name, "result array too small for image");
  }
  virtual ~FilterAllChannels() {}

  virtual void reset()
  {
    src.clear();
    ImageFilter::reset();
  }

protected:

  virtual void filterOneChannel(const vector<Image>& src, const Image& dst) = 0;

  virtual void get_data()
  {
    for (int i = 0; i < (int)input.size(); ++i)
      {
        const GData* inp = getInputData(i);
        // cout << i << ": " << inp << endl;
        if (inp != NULL && inp->getType().matchType(DType::image))
          {
            src.push_back(dynamic_cast<const GImage*>(inp));
          }
      }

    if (src.empty())
      throw SdpException(name, "No input image");

    try
      {
        calcSize(src[0]);

        for (int i = 1; i < (int)src.size(); ++i)
          adaptSize(src[i]);

        GImage* tresult = createResult();

        for (int i = 0; i < nImg; ++i) // all channels
          {
            vector<Image> srcimg;
            for (int k = 0; k < (int)src.size(); ++k)
              {
                srcimg.push_back((*src[k])[i]);
              }
            filterOneChannel(srcimg, (*tresult)[i]);
          }
      }
    catch (SdpException exc)
      {
        exc.setWhere(name);
        throw exc;
      }
  }
protected:
  vector<const GImage*> src;
};

#endif

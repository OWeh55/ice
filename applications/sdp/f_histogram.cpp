
#include <image.h>

#include "f_histogram.h"

void CalcHistogram::get_data()
{
  //  cout << "2pointset" << endl;
  // at the moment we only accept images
  const GImage* src = getInputImage(0);

  if (src->getSize() > 1)
    throw SdpException(getName(), "expected grayvalue image");

  GInteger* tresult = new GInteger;
  result[0] = tresult;

  Histogram h((*src)[0]);

  for (int i = 0; i < h.nClasses(); ++i)
    {
      tresult->push_back(h.getCount(i));
    }
  //  cout << "2pointset done: " << tresult->getSize() << " Punkte" << endl;
}

void Otsu::get_data()
{
  DType typin = getInputType(0);
  if (typin.matchType(DType::integer))
    {
      // input is histogram
      const GInteger* in = dynamic_cast<const GInteger*>(getInputPointer<GInteger, DType::integer>(0));
      int classes = in->getSize(); //  = classnumber
      Histogram hist(classes);
      for (int i = 0; i < classes; ++i)
        {
          hist.addValue(i, (*in)[i]);
        }

      int level = CalcThreshold(hist);

      GInteger* tresult = new GInteger;
      result[0] = tresult;
      tresult->push_back(level);
    }
  else if (typin.matchType(DType::image))
    {
      const GImage* src = getInputImage(0);

      if (src->getSize() > 1)
        throw SdpException(getName(), "expected grayvalue image");

      Histogram hist((*src)[0]);
      int level = CalcThreshold(hist);

      GInteger* tresult = new GInteger;
      result[0] = tresult;
      tresult->push_back(level);
    }
  else
    throw WrongTypeException("Otsu", DType::integer | DType::image, typin.type);
}

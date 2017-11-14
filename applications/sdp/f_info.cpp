#include "f_info.h"

void getInfos::get_data()
{
  // cout << "getInfo" <<endl;
  //  cout << getInputData(0) << endl;
  if (getInputData(0) == NULL)
    throw SdpException(getName(), "input undefined");

  result[0] = getInputData(0)->clone();
  cout << "stream type is " << result[0]->getType().getString();
  cout << ", " << result[0]->getSize() << " elements" << endl;

  result[1] = new GInteger(result[0]->getSize());

  DType rtype = result[0]->getType();
  if (rtype.type == DType::integer)
    {
      const GInteger *src = getInputPointer<GInteger, DType::integer>(0);
      // cout << src << endl;
      for (int i = 0; i < src->getSize(); ++i)
        cout << i << ": " << (*src)[i] << endl;
    }
  else if (rtype.type == DType::floatingpoint)
    {
      const GFloat *src = getInputPointer<GFloat, DType::floatingpoint>(0);
      // cout << src << endl;
      for (int i = 0; i < src->getSize(); ++i)
        cout << i << ": " << (*src)[i] << endl;
    }
  else if (rtype.type == DType::string_type)
    {
      const GString *src = getInputPointer<GString, DType::string_type>(0);
      // cout << src << endl;
      for (int i = 0; i < src->getSize(); ++i)
        cout << i << ": " << (*src)[i] << endl;
    }
  else if (rtype.type == DType::point)
    {
      const GPoint *src = getInputPointer<GPoint, DType::point>(0);
      // cout << src << endl;
      for (int i = 0; i < src->getSize(); ++i)
        cout << i << ": " << (*src)[i] << endl;
    }
  else if (rtype.type == DType::image)
    {
      cout << "image: ";
      const Image &img = getInput<Image, GImage, DType::image>(0);
      cout << img.xsize << "x" << img.ysize << " " << img.maxval << endl;
      result[2] = new GInteger(img.xsize);
      result[3] = new GInteger(img.ysize);
      result[4] = new GInteger(img.maxval);
    }
}

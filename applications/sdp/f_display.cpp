#include <image.h>

#include "f_display.h"

void display::get_data()
{
  DType type = getInputType(0);
  string displayname = getInputString(1, "display");
  const GData* src = const_cast<GData*>(getInputData(0));
  result[0] = src->clone();

  if (type.matchType(DType::image))
    {
      //    cout << getInputData(0) << endl;
      const GImage* img = getInputPointer<GImage, DType::image>(0);
      //    cout << img << endl;
      if (lastresult != NULL) // call after reset, use existing Visual
        {
          GImage* tresult = dynamic_cast<GImage*>(lastresult);
          result[0] = lastresult;
          for (int i = 0; i < img->getSize(); ++i)
            CopyImg((*img)[i], (*tresult)[i]);
        }
      else
        {
          GImage* tresult = cloneGImage(img);
          result[0] = tresult;
          int isize = tresult->getSize();
          if (isize == 1)
            {
              v = Show(ON, (*tresult)[0], displayname);
            }
          else if (isize == 2)
            {
              v = Show(_RGB, (*tresult)[0], (*tresult)[1], (*tresult)[0], displayname);
            }
          else if (isize == 3)
            {
              v = Show(_RGB, (*tresult)[0], (*tresult)[1], (*tresult)[2], displayname);
            }
        }
    }
  else if (type.matchType(DType::integer))
    {
      cout << displayname << ": " << endl;
      const GInteger* gi = dynamic_cast<const GInteger*>(result[0]);
      for (int i = 0; i < gi->getSize(); ++i)
        cout << i << ": " << (*gi)[i] << endl;
    }
  else if (type.matchType(DType::floatingpoint))
    {
      cout << displayname << ": " << endl;
      const GFloat* gf = dynamic_cast<const GFloat*>(result[0]);
      for (int i = 0; i < gf->getSize(); ++i)
        cout << i << ": " << (*gf)[i] << endl;
    }
  else if (type.matchType(DType::string_type))
    {
      cout << displayname << ": " << endl;
      const GString* gs = dynamic_cast<const GString*>(result[0]);
      for (int i = 0; i < gs->getSize(); ++i)
        cout << i << ": " << (*gs)[i] << endl;
    }
}

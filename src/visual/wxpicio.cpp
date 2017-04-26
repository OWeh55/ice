#include <wx/init.h>
#include <wx/image.h>

#include "wxpicio.h"

namespace ice
{

  bool WXReadImg(const std::string& fname, Image& img)
  {
    wxInitialize();
    wxInitAllImageHandlers();
    wxImage wximg;
    if (!wximg.LoadFile(wxString(fname.c_str(), wxConvUTF8)))
      {
        return false;
      }

    if (IsImg(img))
      {
        wximg.Resize(wxSize(img->xsize, img->ysize), wxPoint(0, 0));
      }
    else
      {
        img = NewImg(wximg.GetWidth(), wximg.GetHeight(), 255);
      }
    wxImage wxgrey = wximg.ConvertToGreyscale();
    int xs = wxgrey.GetWidth();
    int ys = wxgrey.GetHeight();

    for (int y = 0; y < ys; y++)
      for (int x = 0; x < xs; x++)
        {
          PutVal(img, x, y, 255 - wxgrey.GetGreen(x, y));
        }

    wxUninitialize();
    return true;
  }

  bool WXReadImg(const std::string& fname, Image& ir, Image& ig, Image& ib)
  {

    return true;
  }
}

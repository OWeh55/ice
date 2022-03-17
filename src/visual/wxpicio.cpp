/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
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

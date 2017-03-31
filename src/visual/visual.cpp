/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <iostream>

#include "visual/App.h"
#include "visual/ImageMgr.h"
#include "IceException.h"
#include "visual/GreyImageWindow.h"
#include "visual/GreyImageColorTableWindow.h"
#include "visual/OverlayImageWindow.h"
#include "visual/RGBImageWindow.h"
#include "macro.h"

#include "visual/visual.h"

namespace ice
{
  int OpenGraphic(const std::string& windowname)
  {
    return OK;
  }

  int SetVirtualScreen(int)
  {
    return OK;
  }

#define FNAME "Cursor"
  int Cursor(int Mode, const Image& Img)
  {
    if (!IsImg(Img))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(Img));

    // if the image is not visualized, do nothing

    if (v == NULL)
      {
        return OK;
      }

    if (Mode == ON)
      {
        v->EnableCursor(true);
        return OK;
      }

    if (Mode == OFF)
      {
        v->EnableCursor(false);
        return OK;
      }

    // if no valid Mode was specified, return some error code
    throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
    return WRONG_PARAM;
  }

  int Cursor(int Mode, const Image& Img, int x, int y)
  {
    if (!IsImg(Img))
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }

    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(Img));

    // if the image is not visualized, do nothing

    if (v == NULL)
      {
        return OK;
      }

    if (Mode == SET)
      {
        return v->SetCursor(x, y);
      }

    // if no valid Mode was specified, return some error code
    throw IceException(FNAME, M_WRONG_MODE, WRONG_PARAM);
    return WRONG_PARAM;
  }

  int Cursor(int Mode, const Image& Img, IPoint p)
  {
    return Cursor(Mode, Img, p.x, p.y);
  }

#undef FNAME

  int GetGreyColor(int, int&, int&, int&)
  {
    return ERROR;
  }

  int GetOverlayColor(int, int&, int&, int&)
  {
    return ERROR;
  }

  int Display(int option)
  {
    //TODO
    // should at least be minimized (all pics, or a specific one)
    return OK;
  }

#define FNAME "Zoom"
  int Zoom(Image Img, int ZoomFactor, int, int)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(Img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }
    return v->Zoom(ZoomFactor);
  }

  int Zoom(Image Img)
  {
    return Zoom(Img, 0);
  }
#undef FNAME
#define FNAME "SetGreyColor"
  int SetGreyColor(unsigned int Entry,
                   unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue)
  {
    RETURN_ERROR_IF_FAILED(wxGetApp().GetImageManager()->SetGreyColor(Entry, RedValue, GreenValue, BlueValue));
    return OK;
  }

  int SetGreyColor(Image img, unsigned int Entry,
                   unsigned char Red, unsigned char Green, unsigned char Blue)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(v->SetGreyColor(Entry, Red, Green, Blue));
    return OK;
  }
#undef FNAME
#define FNAME "SetGreyLut"
  int SetGreyLUT(Image img, unsigned int First, unsigned int Last)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(v->SetGreyLUT(First, Last));
    return OK;
  }

  int SetGreyLUT(unsigned int First, unsigned int Last)
  {
    RETURN_ERROR_IF_FAILED(wxGetApp().GetImageManager()->SetGreyLUT(First, Last));
    return OK;
  }

#undef FNAME
#define FNAME "SetOverlayColor"
  int SetOverlayColor(unsigned int Entry,
                      unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue)
  {
    RETURN_ERROR_IF_FAILED(wxGetApp().GetImageManager()->SetOverlayColor(Entry, RedValue, GreenValue, BlueValue));
    return OK;
  }

  int SetOverlayColor(Image img, unsigned int Entry,
                      unsigned char Red, unsigned char Green, unsigned char Blue)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }

    RETURN_ERROR_IF_FAILED(v->SetOverlayColor(Entry, Red, Green, Blue));
    return OK;
  }
#undef FNAME
#define FNAME "GetGrayColor"
  int GetGreyColor(const Image& img, unsigned int Entry,
                   unsigned char& Red, unsigned char& Green, unsigned char& Blue)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }
    RETURN_ERROR_IF_FAILED(v->GetGreyColor(Entry, Red, Green, Blue));
    return OK;
  }

#undef FNAME
#define FNAME "GetOverlayColor"
  int GetOverlayColor(const Image& img, unsigned int Entry,
                      unsigned char& Red, unsigned char& Green, unsigned char& Blue)
  {
    Visual v;
    RETURN_ERROR_IF_FAILED(v = GetVisual(img));
    if (v == NULL)
      {
        throw IceException(FNAME, M_NOT_VIS, WRONG_PARAM);
        return WRONG_PARAM;
      }
    RETURN_ERROR_IF_FAILED(v->GetOverlayColor(Entry, Red, Green, Blue));
    return OK;
  }
#undef FNAME
#define FNAME "InfVis"
  int InfVis(int Code)
  {
    // InfVis is deprecated, since every image will be displayed inside it's own window.
    // Please don't use InfVis() it anymore.

    int Width, Height;
    ::wxDisplaySize(&Width, &Height);

    switch (Code)
      {
      case MAXX:
      {
        return Width;
      }

      case VIRTUAL_X:
      {
        return Min(Width, wxGetApp().DefXSize());
      }

      case MAXY:
      {
        return Height;
      }

      case VIRTUAL_Y:
      {
        return Min(Height, wxGetApp().DefYSize());
      }

      case IMAGES:
        return 0;

      case TABLE_LEN:
        return 256;

      case REAL_TIME_COLOR:
      case REAL_TIME_ZOOM:
      case TRUECOLOR:
        return 1;

      default:
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return ERROR;
      }
  }
#undef FNAME
#define FNAME "Show"
  Visual Show(int Mode, Visual vis)
  {
    return wxGetApp().GetImageManager()->Show(Mode, vis);
  }

  Visual Show(int Mode,
              const Image& Img,
              const std::string& name)
  {
    if (!IsImg(Img))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return NULL;
      }
    return wxGetApp().GetImageManager()->Show(Mode, Img.Img(), name);
  }

  Visual Show(int Mode,
              ImageD& Img,
              const std::string& name)
  {
    return wxGetApp().GetImageManager()->Show(Mode, &Img, name);
  }

  Visual Show(int Mode,
              const Image& Image1,
              const Image& Image2,
              const std::string& name)
  {
    if ((Mode == OVERLAY) && (!IsImg(Image1)))
      {
        return Show(Mode, Image2, name);
      }

    if ((!IsImg(Image1)) ||
        (!IsImg(Image2)))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return NULL;
      }
    return wxGetApp().GetImageManager()->Show(Mode,
           Image1.Img(),
           Image2.Img(),
           name);
  }

  Visual Show(int Mode,
              const Image& Image1,
              const Image& Image2,
              const Image& Image3,
              const std::string& name)
  {
    if ((!IsImg(Image1)) ||
        (!IsImg(Image2)) ||
        (!IsImg(Image3)))
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return NULL;
      }
    return wxGetApp().GetImageManager()->Show(Mode,
           Image1.Img(),
           Image2.Img(),
           Image3.Img(), name);
  }

  Visual Show(int Mode,
              const ice::Image& Image1,
              const ice::Image& Image2,
              const ice::Image& Image3,
              const ice::Image& Image4,
              const ice::Image& Image5,
              const ice::Image& Image6,
              const std::string& name)
  {
    if (
      (!IsImg(Image1)) ||
      (!IsImg(Image2)) ||
      (!IsImg(Image3)) ||
      (!IsImg(Image4)) ||
      (!IsImg(Image5)) ||
      (!IsImg(Image6))
    )
      {
        throw IceException(FNAME, M_WRONG_IMAGE, WRONG_PARAM);
        return NULL;
      }
    return wxGetApp().GetImageManager()->Show(Mode,
           Image1.Img(),
           Image2.Img(),
           Image3.Img(),
           Image4.Img(),
           Image5.Img(),
           Image6.Img(),
           name);
  }


  Visual Show(int Mode,
              const ColorImage& Image1,
              const ColorImage& Image2,
              const std::string& name)
  {
    return wxGetApp().GetImageManager()->Show(Mode,
           Image1.redImage().Img(),
           Image1.greenImage().Img(),
           Image1.blueImage().Img(),
           Image2.redImage().Img(),
           Image2.greenImage().Img(),
           Image2.blueImage().Img(),
           name);
  }
#undef FNAME
#define FNAME "GetVisual"
  Visual GetVisual(const Image& img)
  {
    //    std::cout << "GetVisual" << std::endl;
    if (!IsImg(img))
      {
        return NULL;
      }
    //    std::cout << "GetVisual(isImage!)" << std::endl;
    return wxGetApp().GetImageManager()->GetVisual(img.Img());
  }

  Visual GetVisual(const ColorImage& img)
  {
    Visual v = NULL;
    if (!img.isValid())
      {
        return v;
      }
    v = wxGetApp().GetImageManager()->GetVisual(img.redImage().Img());
    return v;
  }

#undef FNAME

} // namespace ice

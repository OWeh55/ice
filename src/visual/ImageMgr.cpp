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

#include <wx/wx.h>

#include "message.h"
#include "macro.h"
#include "base.h"

#include "visual/App.h"

#ifdef EVENTDEBUG
#include <iostream>
using namespace std;
#endif

#include "visual/ImageMgr.h"
#include "visual/GreyImageWindow.h"
#include "visual/GreyImageColorTableWindow.h"
#include "visual/OverlayImageWindow.h"
#include "visual/Overlay2ImageWindow.h"
#include "visual/RGBImageWindow.h"
#include "visual/StereoIHImageWindow.h"
#include "visual/visual.h"

namespace ice
{

  DEFINE_EVENT_TYPE(DESTROY_WIN)
  DEFINE_EVENT_TYPE(CREATE_GREY_WIN)
  DEFINE_EVENT_TYPE(CREATE_GREY_COLORTABLE_WIN)
  DEFINE_EVENT_TYPE(CREATE_OVERLAY_WIN)
  DEFINE_EVENT_TYPE(CREATE_OVERLAY2_WIN)
  DEFINE_EVENT_TYPE(CREATE_RGB_WIN)
  DEFINE_EVENT_TYPE(CREATE_STEREO_IH_WIN)
  DEFINE_EVENT_TYPE(REFRESH_TIMER)

  BEGIN_EVENT_TABLE(ImageManager, wxEvtHandler)
    EVT_COMMAND(wxID_ANY, CREATE_GREY_WIN,    ImageManager::OnCreateGreyWin)
    EVT_COMMAND(wxID_ANY, CREATE_GREY_COLORTABLE_WIN,    ImageManager::OnCreateGreyColorTableWin)
    EVT_COMMAND(wxID_ANY, CREATE_OVERLAY_WIN, ImageManager::OnCreateOverlayWin)
    EVT_COMMAND(wxID_ANY, CREATE_OVERLAY2_WIN, ImageManager::OnCreateOverlay2Win)
    EVT_COMMAND(wxID_ANY, CREATE_RGB_WIN,     ImageManager::OnCreateRGBWin)
    EVT_COMMAND(wxID_ANY, CREATE_STEREO_IH_WIN, ImageManager::OnCreateStereoIHWin)

    EVT_COMMAND(wxID_ANY, DESTROY_WIN,        ImageManager::OnDestroyWin)

    EVT_TIMER(REFRESH_TIMER,                ImageManager::OnRefreshTimer)
  END_EVENT_TABLE()
  ;


  ImageManager::ImageManager()
    : RefreshTimer(this, REFRESH_TIMER),
      RefreshEnabled(true)
  {
    int refreshtime = wxGetApp().RefreshTime();

    if (refreshtime != 0 && refreshtime < 10)
      {
        refreshtime = 10;  // not more than 100 Hz
      }

    if (refreshtime != 0)    // DEBUG: refreshtime==0 -> no Timer
      {
        // start the refresh timer for the first time
        // This sets the time too
        // This is a one shot timer, which is restarted after processing
        // the timer event
        RefreshTimer.Start(refreshtime, true);
      }
  }

  ImageManager::~ImageManager()
  {
    // Stop update events
    RefreshTimer.Stop();

    // delete all image windows, if there are still any
    while (!WindowList.empty())
      {
        WindowList.front()->Destroy();
        WindowList.pop_front();
      }
  }

  Visual ImageManager::GetVisual(ImageBase* img) const
  {
    Visual result = NULL;
    // find Visual showing given Image
    for (std::list<ImageWindow*>::const_iterator it = WindowList.begin();
         result == NULL && it != WindowList.end() ;
         it++)
      {
        if ((*it)->showsImage(img))
          {
            result = *it;
          }
      }
    return result;
  }

  Visual ImageManager::GetVisual(ImageD* img) const
  {
    Visual result = NULL;
    // find Visual showing given Image
    for (std::list<ImageWindow*>::const_iterator it = WindowList.begin();
         result == NULL && it != WindowList.end() ;
         it++)
      {
        if ((*it)->showsImage(img))
          {
            result = *it;
          }
      }
    return result;
  }

  // this is a structure to exchange visualisation data between threads
  // -> Event.GetClientData()
  struct ImageData
  {
    ImageBase* img1;
    ImageBase* img2;
    ImageBase* img3;
    ImageBase* img4;
    ImageBase* img5;
    ImageBase* img6;
    ImageD* imgd;
    int param;
    std::string title;
    Visual v;
    ImageData(): img1(NULL), img2(NULL), img3(NULL), img4(NULL), img5(NULL), img6(NULL), imgd(NULL), v(NULL) {}
  };

  void ImageManager::OnCreateGreyWin(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    GreyImageWindow* imageWindow;

    if (id.img1 != NULL)
      {
        imageWindow = new GreyImageWindow(id.img1, id.title);
      }
    else
      {
        imageWindow = new GreyImageWindow(id.imgd, id.title);
      }
    WindowList.push_back(imageWindow);
    imageWindow->Show(true);
    id.v = imageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnCreateGreyColorTableWin(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    GreyImageColorTableWindow* ImageWindow = new GreyImageColorTableWindow(id.img1, id.title);
    WindowList.push_back(ImageWindow);
    ImageWindow->Show(true);
    id.v = ImageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnCreateOverlayWin(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    OverlayImageWindow* ImageWindow = new OverlayImageWindow(id.img1, id.img2, id.title);
    WindowList.push_back(ImageWindow);
    ImageWindow->Show(true);
    id.v = ImageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnCreateOverlay2Win(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    Overlay2ImageWindow* ImageWindow = new Overlay2ImageWindow(id.img1, id.img2, id.title, id.param);
    WindowList.push_back(ImageWindow);
    ImageWindow->Show(true);
    id.v = ImageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnCreateRGBWin(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    RGBImageWindow* ImageWindow =
      new RGBImageWindow(id.img1, id.img2, id.img3, id.title);
    WindowList.push_back(ImageWindow);
    ImageWindow->Show(true);
    id.v = ImageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnCreateStereoIHWin(wxCommandEvent& Event)
  {
    // extract the image data from the event
    ImageData& id = *(ImageData*)Event.GetClientData();

    iceStereoIHImageWindow* ImageWindow =
      new iceStereoIHImageWindow(id.img1, id.img2, id.img3,
                                 id.img4, id.img5, id.img6,
                                 id.title);
    WindowList.push_back(ImageWindow);
    ImageWindow->Show(true);
    id.v = ImageWindow;
    WakeUpUserThread();
  }

  void ImageManager::OnDestroyWin(wxCommandEvent& Event)
  {
    // extract the window to destroy from the event
    ImageWindow* imageWindow = (ImageWindow*) Event.GetEventObject();

    WindowList.remove(imageWindow);

    // we destroy the image window
    imageWindow -> Destroy();

    WakeUpUserThread();
  }

  // Changes color at "Entry" in ALL colortables
  // This send the command to all Windows. Windows without color table
  // have to ignore this (by not overwriting the methods of ImageBase)
  int ImageManager::SetGreyColor(unsigned int Entry,
                                 unsigned char RedValue,
                                 unsigned char GreenValue,
                                 unsigned char BlueValue)
  {
    // Tell all image windows to use this color definition(only if possible)
    for (std::list<ImageWindow*>::iterator it = WindowList.begin();
         it != WindowList.end();
         it++)
      {
        (*it)->SetGreyColor(Entry, RedValue, GreenValue, BlueValue);
      }
    return OK;
  }

  int ImageManager::SetGreyLUT(unsigned int First, unsigned int Last)
  {
    // Tell all image windows to use this color definition (only if possible)
    // This send the command to all Windows. Windows without color table
    // have to ignore this (by not overwriting the methods of ImageBase)
    for (std::list<ImageWindow*>::iterator it = WindowList.begin();
         it != WindowList.end();
         it++)
      {
        (*it)->SetGreyLUT(First, Last);
      }
    return OK;
  }

  int ImageManager::SetOverlayColor(unsigned int Entry,
                                    unsigned char RedValue,
                                    unsigned char GreenValue,
                                    unsigned char BlueValue)
  {
    // Tell all images to adjust their overlay color tables (only if possible)
    // This send the command to all Windows. Windows without color table
    // have to ignore this (by not overwriting the methods of ImageBase)
    for (std::list<ImageWindow*>::iterator it = WindowList.begin();
         it != WindowList.end();
         it++)
      {
        (*it)->SetOverlayColor(Entry, RedValue, GreenValue, BlueValue);
      }
    return OK;
  }

#define FNAME "Show"
  Visual ImageManager::Show(int Mode, Visual vis)
  {
    if (vis == NULL)
      {
        Message(FNAME, M_NOT_VIS, WRONG_PARAM);
        return vis;
      }

    switch (Mode)
      {
      case OFF:
      {
        // Delete the image window.
        // this has to be done in the main thread
        wxCommandEvent Event(DESTROY_WIN);
        Event.SetEventObject((wxObject*)vis);
        AddPendingEvent(Event);
        WaitForMainThread();
        return vis;
      }
      default:
        // if no valid Mode was specified, there must be some error
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }
  }

  Visual ImageManager::Show(int Mode, ImageBase* Img,
                            const std::string& name)
  {
    ImageData id;
    id.img1 = Img;
    id.title = name;
    if (id.title.empty())
      {
        id.title = Img->getTitle();
      }
    if (id.title.empty())
      {
        id.title = ICEGRAFICNAME;
      }

    switch (Mode)
      {
      case OFF:
      {
        Visual v = GetVisual(Img);
        while (v != NULL)
          {
            // Delete the corresponding image window
            // this has to be done in the main thread
            wxCommandEvent Event(DESTROY_WIN);
            Event.SetEventObject((wxObject*)v);
            AddPendingEvent(Event);
            WaitForMainThread();
            v = GetVisual(Img);
          }
        return v;
      }

      case ON:
      {
        if (Img->maxval > ColorTable::maxEntries)
          {
            Message(FNAME, M_HIGHRANGE_VIS, WRONG_PARAM);
            return NULL;
          }
        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_GREY_COLORTABLE_WIN);
        Event.SetClientData(&id);
        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;
      }

      case GRAY:
      {
        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_GREY_WIN);
        Event.SetClientData(&id);
        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;
      }

      case OVERLAY:
      {
        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_GREY_COLORTABLE_WIN);
        Event.SetClientData(&id);

        AddPendingEvent(Event);
        WaitForMainThread();
        ((GreyImageColorTableWindow*)id.v)->SetTable();
        return id.v;
      }
      }

    // if no valid Mode was specified, there must be some error
    Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
    return NULL;
  }

  Visual ImageManager::Show(int Mode, ImageD* Img,
                            const std::string& name)
  {
    ImageData id;
    id.imgd = Img;
    id.title = name;
    //    if (id.title.empty()) id.title = Img->getTitle();
    if (id.title.empty())
      {
        id.title = ICEGRAFICNAME;
      }

    switch (Mode)
      {
      case OFF:
      {
        Visual v = GetVisual(Img);
        while (v != NULL)
          {
            // Delete the corresponding image window
            // this has to be done in the main thread
            wxCommandEvent Event(DESTROY_WIN);
            Event.SetEventObject((wxObject*)v);
            AddPendingEvent(Event);
            WaitForMainThread();
            v = GetVisual(Img);
          }
        return v;
      }

      case GRAY:
      {
        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_GREY_WIN);
        Event.SetClientData(&id);
        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;
      }

      default:
        // if no valid Mode was specified, there must be some error
        Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return NULL;
      }
  }

  Visual ImageManager::Show(int Mode,
                            ImageBase* Image1,
                            ImageBase* Image2,
                            const std::string& name)
  {
    ImageData id;
    id.img1 = Image1;
    id.img2 = Image2;

    ImageBase* himg = Image1;
    if (himg == NULL)
      {
        himg = Image2;
      }
    id.title = name;
    if (id.title.empty())
      {
        id.title = himg->getTitle();
      }
    if (id.title.empty())
      {
        id.title = ICEGRAFICNAME;
      }

    switch (Mode)
      {
      case OVERLAY:
      {
        // if no base image is provided we redirect the request to the
        // "one image" Show()
        if (Image1 == NULL)
          {
            return Show(Mode, Image2, name);
          }

        // Check if the images match in size.
        if (Image1->xsize != Image2->xsize || Image1->ysize != Image2->ysize)
          {
            Message(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
            return NULL;
          }

        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_OVERLAY_WIN);
        Event.SetClientData(&id);

        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;
      }

      case OVERLAY1:
      case OVERLAY2:
      case OVERLAY3:
      {
        // Check if the images match in size.
        if (Image1->xsize != Image2->xsize || Image1->ysize != Image2->ysize)
          {
            Message(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
            return NULL;
          }
        if (Mode == OVERLAY1)
          {
            id.param = 64;
          }
        else if (Mode == OVERLAY2)
          {
            id.param = 128;
          }
        else
          {
            id.param = 192;
          }
        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_OVERLAY2_WIN);
        Event.SetClientData(&id);

        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;
      }

      } // switch(Mode)

    // if no valid Mode was specified, there must be some error
    Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
    return NULL;
  }

  Visual ImageManager::Show(int Mode,
                            ImageBase* Image1,
                            ImageBase* Image2,
                            ImageBase* Image3,
                            const std::string& name)
  {
    ImageData id;
    id.img1 = Image1;
    id.img2 = Image2;
    id.img3 = Image3;
    id.title = name;
    if (id.title.empty())
      {
        id.title = Image1->getTitle();
      }
    if (id.title.empty())
      {
        id.title = ICEGRAFICNAME;
      }

    switch (Mode)
      {
      // TODO: RGB is also a macro in wx, renamed it to _RGB(see defs.h)
      case _RGB:

        // Check if the images match in size
        if (Image1->xsize != Image2->xsize || Image1->ysize != Image2->ysize ||
            Image2->xsize != Image3->xsize || Image2->ysize != Image3->ysize)
          {
            Message(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
            return NULL;
          }

        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_RGB_WIN);
        Event.SetClientData(&id);

        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;

      } // switch(Mode)

    // if no valid Mode was specified, there must be some error
    Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
    return NULL;

  }

  Visual ImageManager::Show(int Mode,
                            ImageBase* Image1,
                            ImageBase* Image2,
                            ImageBase* Image3,
                            ImageBase* Image4,
                            ImageBase* Image5,
                            ImageBase* Image6,
                            const std::string& name)
  {
    ImageData id;
    id.img1 = Image1;
    id.img2 = Image2;
    id.img3 = Image3;
    id.img4 = Image4;
    id.img5 = Image5;
    id.img6 = Image6;
    id.title = name;
    if (id.title.empty())
      {
        id.title = Image1->getTitle() + " - " + Image4->getTitle();
      }
    if (id.title.empty())
      {
        id.title = ICEGRAFICNAME;
      }

    switch (Mode)
      {
      // TODO: RGB is also a macro in wx, renamed it to _RGB(see defs.h)
      case STEREO_IH:

        // Check if the images match in size
        if (Image1->xsize != Image2->xsize || Image1->ysize != Image2->ysize ||
            Image1->xsize != Image3->xsize || Image1->ysize != Image3->ysize ||
            Image1->xsize != Image4->xsize || Image1->ysize != Image4->ysize ||
            Image1->xsize != Image5->xsize || Image1->ysize != Image5->ysize ||
            Image1->xsize != Image6->xsize || Image1->ysize != Image6->ysize)
          {
            Message(FNAME, M_SIZES_DIFFER, WRONG_PARAM);
            return NULL;
          }

        // Windows MUST be created in main thread
        wxCommandEvent Event(CREATE_STEREO_IH_WIN);
        Event.SetClientData(&id);

        AddPendingEvent(Event);
        WaitForMainThread();
        return id.v;

      } // switch(Mode)

    // if no valid Mode was specified, there must be some error
    Message(FNAME, M_WRONG_PARAM, WRONG_PARAM);
    return NULL;

  }
#undef FNAME

  void ImageManager::OnRefreshTimer(wxTimerEvent& TimerEvent)
  {
#ifdef EVENTDEBUG
    cout << "IT ";
    cout.flush();
#endif
    // do something only if the refersh is enabled
    if (RefreshEnabled)
      {
        // send an update message to all image windows
        //    wxCommandEvent Event(REGULAR_UPDATE);
        for (std::list<ImageWindow*>::iterator it = WindowList.begin();
             it != WindowList.end();
             it++)
          {
            (*it)->RegularUpdate();
          }
      }
    // only Restart timer, time is already set
    RefreshTimer.Start(-1, true);
  }

  void DestroyWindows(ice::ImageBase* img)
  {
    Visual v;
    do
      {
        v = wxGetApp().GetImageManager()->GetVisual(img);
        if (v != NULL)
          {
            wxGetApp().GetImageManager()->Show(OFF, v);
          }
      }
    while (v != NULL);
  }
  void DestroyWindowsD(ice::ImageD* img)
  {
    Visual v;
    do
      {
        v = wxGetApp().GetImageManager()->GetVisual(img);
        if (v != NULL)
          {
            wxGetApp().GetImageManager()->Show(OFF, v);
          }
      }
    while (v != NULL);
  }
}

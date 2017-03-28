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
#ifndef __ICEIMAGEMGR_H
#define __ICEIMAGEMGR_H

#include <list>
#include <string>

#include <wx/timer.h>

#include "ImageTemplate.h"
#include "visual/ImageWindow.h"
#include "visual/attribute.h"
#include "visual/ThreadCommHelper.h"

namespace ice
{
//  class iceImageManager;

  /*
  Since the global ImageManager lives in two threads, we need some
  communication and synchronisation facilities. Therefore we have
  to make %ImageManager an event handler object by deriving it
  from wxEvtHandler privately, so it can receive messages. Since
  this class provides many methods that will be called from the user
  thread, but must be processed in the main thread, we have to derive
  ImageManager from %iceThreadCommHelper, as well
  (see iceThreadCommHelper). The image manager holds a lists of
  image windows.
  Some notes about the event handling: %ImageManager has methods
  for image window creation and destruction which will be called from
  inside the user thread. Because under wxWidgets only the main thread
  may hold GUI objects, these image windows must be created inside the
  main thread. That means, that methods for image window creation or
  destruction, that are called from inside the user thread, will send
  a corresponding event to main thread and wait until the desired
  operation has completed. Since only the main thread can wait for
  messages, the user thread must wait for the communication
  mutex that is provided via the mixin class %iceThreadCommHelper. The
  events that are defined for this purpose are:
  - DESTROY_WIN
  - CREATE_GREY_WIN
  - CREATE_OVERLAY_WIN
  - CREATE_RGB_WIN
  */

  class ImageManager: private wxEvtHandler, private ThreadCommHelper
  {
  public:
    ImageManager();
    //! constructor and destructor.
    ~ImageManager();

    /* Show can be used to control the visualization mode of images.
       There are three different versions of Show, thanks to the
       overloading mechanism. Depending on the parameter >Mode<,
       Show () needs a different number of images.
       \param Mode Specifies the kind of manipulation.

    */

    Visual Show(int Mode, Visual vis);
    Visual Show(int Mode,
                ice::ImageBase* Img,
                const std::string& name = "");
    Visual Show(int Mode,
                ice::ImageD* Img,
                const std::string& name = "");
    Visual Show(int Mode,
                ice::ImageBase* Image1,
                ice::ImageBase* Image2,
                const std::string& name = "");
    Visual Show(int Mode,
                ice::ImageBase* Image1,
                ice::ImageBase* Image2,
                ice::ImageBase* Image3,
                const std::string& name = "");

    Visual Show(int Mode,
                ice::ImageBase* Image1,
                ice::ImageBase* Image2,
                ice::ImageBase* Image3,
                ice::ImageBase* Image4,
                ice::ImageBase* Image5,
                ice::ImageBase* Image6,
                const std::string& name = "");

    // Get the visual that shows the image img, returns NULL if image
    // is not displayed
    Visual GetVisual(ImageBase* img) const;
    Visual GetVisual(ImageD* img) const;

    // Those are kept for backward compatibility. They work just like the
    // corresponding function in the subclasses of iceImageWindow, but
    // they affect all windows
    int SetGreyColor(unsigned int Entry,
                     unsigned char RedValue,
                     unsigned char GreenValue,
                     unsigned char BlueValue);
    int SetGreyLUT(unsigned int First, unsigned int Last);
    int SetOverlayColor(unsigned int Entry,
                        unsigned char RedValue,
                        unsigned char GreenValue,
                        unsigned char BlueValue);

  protected:
    // event handlers
    void OnDestroyWin(wxCommandEvent& Event);
    void OnCreateGreyWin(wxCommandEvent& Event);
    void OnCreateGreyColorTableWin(wxCommandEvent& Event);
    void OnCreateOverlayWin(wxCommandEvent& Event);
    void OnCreateOverlay2Win(wxCommandEvent& Event);
    void OnCreateRGBWin(wxCommandEvent& Event);
    void OnCreateStereoIHWin(wxCommandEvent& Event);
    //! The event handlers.
    void OnRefreshTimer(wxTimerEvent& TimerEvent);

    // The lists of all image windows
    std::list<ImageWindow*> WindowList;

    // We employ a timer that gives the image windows impulses
    // to refresh regularly, if the regular update is enabled.
    wxTimer RefreshTimer;

    // This flag tells us whether the regular update for all
    // windows is disabled, that means that ALL windows are frozen.
    // Note that single windows can be frozen, too, independent
    // of this global update behaviour.
    bool RefreshEnabled;

    DECLARE_EVENT_TABLE()
  };

  void DestroyWindows(ice::ImageBase* img);
  void DestroyWindowsD(ice::ImageD* img);
}
#endif // #ifndef __ICEIMAGEMGR_H

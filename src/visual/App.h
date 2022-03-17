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

#ifndef __ICEAPP_H
#define __ICEAPP_H

#include <wx/app.h>
#include "visual/ImageMgr.h"
#include "visual/ThreadCommHelper.h"
#include "visual/ImageMgr.h"

//#define EVENTDEBUG 1

namespace ice
{
  class iceApp;
}

// wxWindows suggests to use this macro in order to gain access to
// our (main) application via ::wxGetApp (); thus we don't need to
// hand the application's pointer around

DECLARE_APP(ice::iceApp)

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(CLOSE, 10)
END_DECLARE_EVENT_TYPES()

namespace ice
{

  //! Our version of the application class, that every wxWidgets application needs.
  /*! Fortunately, wxWidgets does all the messy OS dependent initialisation stuff, that
    means, we only have to derive our own wxApp class, and things like setting up an
    event loop, parsing the command line parameters and so on belong to the past.
    Initialization of the application has to be done in the OnInit () method.
    <BR> Some words about the general design of ICE applications. An ICE application
    has two threads: the main thread and the user thread. The main thread is the
    thread where all the GUI related things happen (e.g. the event loop). The user
    thread is the thread that only runs the Main function, which must be provided
    by the user of this library (programmer).
    <BR>For text output ICE provides some kind of console window. It is not a real
    console window like bash of cmd, but a graphical window that behaves just like
    a console window. There are some input and output methods for the console window,
    whose names and parameters resemble the native stdio functions (see iceConsoleWin).
    <BR>Typical ICE applications create and manipulate a lot of image objects. These
    image objects are maintained by the image manager.*/

  class iceApp : public wxApp, public EventHelper
  {
  public:
    //! The standard constructor.
    iceApp();

    //! Gets the pointer to the user thread (see at the detailed description).
    class iceUserThread* GetUserThread() const
    {
      return UserThread;
    }

    //! Returns the pointer to the console window. (see at the detailed description).
    class iceConsoleWin* GetConsoleWin() const
    {
      return ConsoleWin;
    }

    //! Returns the pointer to the image manager.  (see at the detailed description).
    class ImageManager* GetImageManager() const
    {
      return imageManager;
    }

    int DefXSize() const
    {
      return defxsize;
    }
    int DefYSize() const
    {
      return defysize;
    }
    int RefreshTime() const
    {
      return refreshtime;
    }

  protected:

    // Default window size
    int defxsize, defysize;

    // Refresh time for image update
    int refreshtime;

    //! A pointer to the user thread.
    class iceUserThread* UserThread;

    //! A pointer to the console window.
    class iceConsoleWin* ConsoleWin;

    //! A pointer to the image manager.
    class ImageManager* imageManager;

    //! This one is called on application startup and is a good place for the app initialization.
    /*! Initialization should be done here instead of the constructor of the application object,
      since we can return false if the initialization fails for some reason.
      We will just start the so called user thread here.*/
    bool OnInit();

    //! For clean-up.
    int OnExit();

    //! Event Handler for the user defined CLOSE event.
    /*! Since the user thread runs as a detached thread, we must respond
      to it's termination. The user thread will send a CLOSE event,
      upon which the application will shut down.*/

    void OnClose(wxEvent& Event);

    DECLARE_EVENT_TABLE()
    ;
  };
}
#endif // ifndef __ICEAPP_H

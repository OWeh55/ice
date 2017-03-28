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

#include "visual/UserThread.h"
#include "visual/ConsoleWin.h"

#include "visual/App.h"

//#include <iostream>
//using namespace std;

using namespace ice;

// this macro will create one global instance of iceApp and
// start it's event loop
IMPLEMENT_APP(iceApp)

DEFINE_EVENT_TYPE(CLOSE)

BEGIN_EVENT_TABLE(iceApp, wxApp)
  EVT_CUSTOM(CLOSE, wxID_ANY, iceApp::OnClose)
END_EVENT_TABLE();


iceApp::iceApp(): wxApp(),
  ConsoleWin(NULL),
  imageManager(NULL)
{
  char* parameter;
  defxsize = 99999;
  defysize = 99999; // very large
  refreshtime = 100; // 100 ms or 10 Hz
  parameter = getenv("ICEVISUALISATION");
  if (parameter != NULL)
    {
      sscanf(parameter, "%d,%d,%d", &defxsize, &defysize, &refreshtime);
    }
  imageManager = new ImageManager();
}

bool iceApp::OnInit()
{
  // now its time to set up the text window
  ConsoleWin = new iceConsoleWin(*this);

  // all we do at this time is to launch the user thread
  UserThread = new iceUserThread(*this);
  if (UserThread->Create() != wxTHREAD_NO_ERROR)
    {
      // throw up some kind of error message here
      std::cerr << "Error: Could not create user thread" << std::endl;
      // returning false will cause the application to terminate
      return false;
    }
  UserThread->Run();
  return true;
}

void iceApp::OnClose(wxEvent& event)
{
  delete imageManager;
  imageManager = NULL;
  ConsoleWin->Destroy();
}

int iceApp::OnExit()
{
  // remove ImageManager
  delete imageManager;

  // Wait for UserThread

  if (UserThread)
    {
      UserThread->Wait();
      delete UserThread;
    }

  return 0;
}

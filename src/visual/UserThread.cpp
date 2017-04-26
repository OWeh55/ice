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

#include <string>
using namespace std;

#include "visual/UserThread.h"

namespace ice
{
  iceUserThread::iceUserThread(iceApp& Appp):
    wxThread(wxTHREAD_JOINABLE), App(&Appp)
  {
  }

  iceUserThread::~iceUserThread()
  {
  }

  wxThread::ExitCode iceUserThread::Entry()
  {
    // convert commandline parameters from wxwidgets to classic C (for Main)
    int argc = wxGetApp().argc;

    char** argv = new char* [argc];

    for (int i = 0; i < argc; i++)
      {
        wxString sw(wxGetApp().argv[i]);
        string sc(sw.mb_str(*wxConvCurrent));
        argv[i] = new char [sc.length() + 1];
        strcpy(argv[i], sc.c_str());
      }

    long int exitcode = 1;
    // calling Main of the user
    try
      {
        exitcode = ::Main(argc, argv);
      }
    catch (const exception& ex)
      {
        cout << "Unhandled exception: " << ex.what() << endl;
        exitcode = (wxThread::ExitCode)1;
      }

    // tell the application that it can finish
    wxCommandEvent Event(CLOSE);
    App->AddPendingEvent(Event);

    for (int i = 0; i < argc; i++)
      {
        delete [] argv[i];
      }

    delete [] argv;
    return wxThread::ExitCode(exitcode);
  }
}

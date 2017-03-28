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

#ifndef __ICEUSERTHREAD_H
#define  __ICEUSERTHREAD_H

#include <wx/thread.h>
#include "visual/App.h"

namespace ice
{

  //! This class implements the user thread. The user thread will host the Main () function
  //! that has to be supplied by the programmer.
  class iceUserThread : public wxThread
  {
  public:
    iceUserThread(iceApp& App);
    ~iceUserThread();
  protected:
    //! We hold a pointer for the main application.
    iceApp* App;

    //! The user thread's entry point; this method will call the programmers Main () function.
    virtual ExitCode Entry();
  };
}
// Prototype of the main function that the user *must* supply
int Main(int argc, char* argv[]);
#endif // ifndef __ICEUSERTHREAD_H

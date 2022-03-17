#ifndef __ICETHREADCOMMHELPER_H
#define __ICETHREADCOMMHELPER_H

#  include <wx/thread.h>

namespace ice
{
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

  class ThreadCommHelper
  {
  public:
    ThreadCommHelper();
    ~ThreadCommHelper();

    /*! Wait until the main thread has finished processing.
      That could be the case if input from the main thread has
      become available, or the main thread has finished some
      GUI critical operations. This method can only be called
      from inside the user thread.*/
    void WaitForMainThread();

    /*! Wake up the user thread. This method must be called from
      inside the main thread.*/
    void WakeUpUserThread();
  private:
    //! The mutexes for communication with the user thread.
    wxMutex MutexA, MutexB;

    /*! This index tells us which Mutex should be used for the next
      communication cycle
      Each communication partner must have it's own index, else
      they could get confused */
    int CurrMutexMain;
    int CurrMutexUser;
  };

  class EventHelper: private ThreadCommHelper
  {
  public:
    // Callable from user thread: wait for event from main thread
    long WaitForEvent();
    // Callable from main thread: send event to user thread
    void SendEvent(long event);

  private:
    //! Event to transfer from main thread to user thread
    long Event;
  };
}
#endif // #ifndef __ICETHREADCOMMHELPER_H

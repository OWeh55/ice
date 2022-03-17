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
#include "visual/ThreadCommHelper.h"

#include <iostream>
using namespace std;

namespace ice
{
  ThreadCommHelper::ThreadCommHelper() :
    MutexA(),
    MutexB(),
    CurrMutexMain(1),
    CurrMutexUser(1)
  {
    // the first mutex is locked for the first communication cycle
    MutexA.Lock();
  }

  ThreadCommHelper::~ThreadCommHelper()
  {
    // unlock both mutexes so that the user thread isn't blocked anymore
    // (if it is still running)
    if (CurrMutexUser == 1)
      {
        MutexA.Unlock();
      }
    else
      {
        MutexB.Unlock();
      }
  }

  void ThreadCommHelper::WaitForMainThread()
  {
    // We wait for the current mutex, and send the user thread to sleep
    // if necessary.
    // For the next communication cycle we have to use the other mutex.
    if (CurrMutexUser == 1)
      {
        MutexA.Lock();
        MutexA.Unlock();
        CurrMutexUser = 2;
      }
    else
      {
        MutexB.Lock();
        MutexB.Unlock();
        CurrMutexUser = 1;
      }
  }

  void ThreadCommHelper::WakeUpUserThread()
  {
    // If we release the current mutex we have to lock the mutex from the
    // last communication cycle beforehand
    if (CurrMutexMain == 1)
      {
        MutexB.TryLock();
        MutexA.Unlock();
        CurrMutexMain = 2;
      }
    else
      {
        CurrMutexMain = 1;
        MutexA.TryLock();
        MutexB.Unlock();
      }
  }

// Callable from user thread: wait for event from main thread
  long EventHelper::WaitForEvent()
  {
    WaitForMainThread();
    return Event;
  }

// Callable from main thread: send event to user thread
  void EventHelper::SendEvent(long event)
  {
    Event = event;
//  cout << "sendevent " << event << endl;
    WakeUpUserThread();
  }
}

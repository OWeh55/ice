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

#include "macro.h"
#include "visual/Interaction.h"

namespace ice
{
  InteractionHandler::InteractionHandler(ImageWindow* ImageWindowP)
    : wxEvtHandler(),
      imageWindow(ImageWindowP),
      // SuccessFlag (false)
      Button(NOPRESS)
  {
    window = imageWindow->getImageRect();
  }

  IPoint InteractionHandler::makeValid(IPoint p)
  {
    if (!window.inside(p))
      {
        if (p.x < window.YI())
          {
            p.x = window.XI();
          }
        if (p.y < window.YI())
          {
            p.y = window.YI();
          }
        if (p.x > window.XA())
          {
            p.x = window.XA();
          }
        if (p.y > window.YA())
          {
            p.y = window.YA();
          }
      }
    return p;
  }

  void InteractionHandler::Cleanup()
  {
    wxCommandEvent Event(CLEANUP_INTERACTION);
#if wxMAJOR_VERSION == 2
#if wxMINOR_VERSION <= 8
    imageWindow->ProcessEvent(Event);
#else
    imageWindow->GetEventHandler()->ProcessEvent(Event);
#endif
#else
    imageWindow->GetEventHandler()->ProcessEvent(Event);
#endif
    //    ImageWindow->ProcessEvent (Event);
  }
}

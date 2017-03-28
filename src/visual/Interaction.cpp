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
    window = imageWindow->GetImageRect();
  }

  IPoint InteractionHandler::makeValid(IPoint p)
  {
    if (!window.inside(p))
      {
        if (p.x < window.YI()) p.x = window.XI();
        if (p.y < window.YI()) p.y = window.YI();
        if (p.x > window.XA()) p.x = window.XA();
        if (p.y > window.YA()) p.y = window.YA();
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

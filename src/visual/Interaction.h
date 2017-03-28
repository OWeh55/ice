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
#ifndef __ICEINTERACTION_H
#define __ICEINTERACTION_H

#include "visual/ImageWindow.h"

namespace ice
{
  /*! accepted range for coordinates outside image window
   */
  const int border = 10;

  /*! An interaction is  used by mouse activity only at the moment. Therefore we provide
   * an enumeration with the return values -1,0,1 for RightMB,MiddleMB,LeftMB as it was used
   * in the old ICE-Library. */
  enum MOUSEBUTTON { NOPRESS = -255, RIGHTMB = -1, MIDDLEMB, LEFTMB };

  /*! Interactions define the behaviour of an image window during an input operation.
    For example if the user shall select an rectanle inside the image window we have to
    draw an selection rectangle. This means we have to process events that are addressed
    to an image window. Instead of writing very large event handler methods for the
    image window class, we prefer to provide simply an interaction object, that processes
    the right events. Whenever the image window holds an interaction it forwards it's
    events to this interaction and processes only those events, that are skipped from
    the interaction. This forwarding is done by pushing the interaction object onto the
    image window's event handler stack.*/
  class InteractionHandler : public wxEvtHandler
  {
  public:
    InteractionHandler(ImageWindow* ImageWindow);

    /*! This method will be called for initialization of this interaction
      inside the main thread.*/
    virtual bool Init() = 0;


    //! Tells us whether the interaction has been finished successfully. This method is useful to evaluate the interaction.
    //bool IsSuccessful () const;

    //! Return the corresponding button that was pressed (or NOPRESS if nothing was pressed)
    MOUSEBUTTON Pressed() const;

  protected:
    //! The window that hosts this interaction.
    ImageWindow* imageWindow;
    Window window;

    IPoint makeValid(IPoint p);

    /*! This method must be called after the interaction has finished. Since this method
      should function like a destructor, each subclass must call the Cleanup () method
      of it's base class.*/
    void Cleanup();


    //! This flag tells us, whether this interaction has been finished successfully.
    //bool SuccessFlag;

    MOUSEBUTTON Button;
  };



//////////////////////////////////////////////////////////////////////////
//
//            inline implementations
//

  /*
    inline bool InteractionHandler::IsSuccessful () const
    {
    return SuccessFlag;
    }*/

  inline MOUSEBUTTON InteractionHandler::Pressed() const
  {
    return Button;
  }
}
#endif // #ifndef __ICEINTERACTION_H

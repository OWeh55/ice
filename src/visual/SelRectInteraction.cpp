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

#include <wx/wx.h>
#include <wx/dcclient.h>  // needed for wxGTK >= 2.5.x 
//#include <iostream>

using namespace std;

#include "visual/SelRectInteraction.h"
#include "macro.h"  // for min/max templates

namespace ice
{
  BEGIN_EVENT_TABLE(iceSelectRectInteraction, wxEvtHandler)
    EVT_PAINT(iceSelectRectInteraction::OnPaint)
    EVT_LEFT_UP(iceSelectRectInteraction::OnLeftMouseButtonUp)
    EVT_RIGHT_UP(iceSelectRectInteraction::OnRightMouseButtonUp)
    EVT_MIDDLE_UP(iceSelectRectInteraction::OnMiddleMouseButtonUp)
    EVT_MOTION(iceSelectRectInteraction::OnMouseMove)
  END_EVENT_TABLE()

  iceSelectRectInteraction::iceSelectRectInteraction(ImageWindow* imagewindow)
    : InteractionHandler(imagewindow),
      mode(select),
      FirstPoint(0, 0),
      SecondPoint(0, 0)
  {
  }

  iceSelectRectInteraction::iceSelectRectInteraction(ImageWindow* imagewindow,
      const Window& w, windowSelectionMode modep)
    : InteractionHandler(imagewindow),
      mode(modep),
      FirstPoint(w.XI(), w.YI()),
      SecondPoint(w.XA(), w.YA())
  {
  }

  bool iceSelectRectInteraction::Init()
  {
    if (mode != select)
      {
        DrawSelectedRect();
      }
    return true;
  }

  void iceSelectRectInteraction::DrawSelectedRect()
  {
    if (mode == select)
      {
        return;
      }

    // We use logical function wxXOR (see explanation at iceSelectPointInteraction::DrawCursor).
    wxClientDC ClientDC(imageWindow);
    ClientDC.SetLogicalFunction(wxXOR);
    ClientDC.SetPen(wxPen(wxColor(128, 128, 128),    // const wxColour& colour
                          1, // int width
                          wxSOLID)); // int style
    ClientDC.SetBrush(*wxTRANSPARENT_BRUSH);

    // determine the upper left corner of the rectangle that is determined by FirstPoint
    // and SecondPoint
    wxPoint TopLeftCornerWin =
      imageWindow->translateImage2WinPos(IPoint(min<int> (FirstPoint.x, SecondPoint.x),
                                         min<int> (FirstPoint.y, SecondPoint.y)));
    ClientDC.DrawRectangle(TopLeftCornerWin.x,  // wxCoord x
                           TopLeftCornerWin.y, // wxCoord y
                           (abs(FirstPoint.x - SecondPoint.x) + 1) * imageWindow->getZoomFactor(),   // wxCoord width
                           (abs(FirstPoint.y - SecondPoint.y) + 1) * imageWindow->getZoomFactor());   // wxCoord height
  }

  void iceSelectRectInteraction::OnPaint(wxPaintEvent& PaintEvent)
  {
    // If the window must be painted, we must paint our selection cursor
    // as well, but we have to paint it AFTER the window contents has
    // been painted. We can't just call Event.Skip, because this would
    // only cause the window to repaint after this OnPaint handler has finished.
    GetNextHandler()->ProcessEvent(PaintEvent);
    DrawSelectedRect();
  }

  void iceSelectRectInteraction::OnMouseMove(wxMouseEvent& MouseEvent)
  {
    // we only draw the selection if the first corner has already been selected
    if (select == mode)
      {
        return;
      }

    // paint the new position only if it lies within the image
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        return;
      }

    MousePos = makeValid(MousePos);

    // delete the old selection
    DrawSelectedRect();
    switch (mode)
      {
      case setlowerright:
        // update the second corner of the selected rect and paint the new selection
        SecondPoint = MousePos;
        break;
      case shiftupperleft:
        SecondPoint.x += MousePos.x - FirstPoint.x;
        SecondPoint.y += MousePos.y - FirstPoint.y;
        FirstPoint = MousePos;
        break;
      case shiftlowerright:
        FirstPoint.x += MousePos.x - SecondPoint.x;
        FirstPoint.y += MousePos.y - SecondPoint.y;
        SecondPoint = MousePos;
        break;
      default:
        break;
      }
    DrawSelectedRect();
  }

  void iceSelectRectInteraction::OnLeftMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // we accept a left or middle  mouse button click only, if it was issued while being inside the image
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }

    MousePos = makeValid(MousePos);

    switch (mode)
      {
      case select:
        FirstPoint = SecondPoint = MousePos;
        DrawSelectedRect();
        mode = setlowerright;
        break;

      case shiftupperleft:
      case setlowerright:
      case shiftlowerright:
        Button = LEFTMB;
        Cleanup();
        break;
      default:
        break;
      }
  }

  void iceSelectRectInteraction::OnRightMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // this interaction is finished, so we have to tidy up
    Button = RIGHTMB;
    Cleanup();
  }

  void iceSelectRectInteraction::OnMiddleMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // we accept a left or middle  mouse button click only, if it was issued while being inside the image
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }

    MousePos = makeValid(MousePos);

    switch (mode)
      {
      case select:
        FirstPoint = SecondPoint = MousePos;
        DrawSelectedRect();
        mode = setlowerright;
        break;

      case shiftupperleft:
      case setlowerright:
      case shiftlowerright:
        Button = LEFTMB;
        Cleanup();
        break;

      default:
        break;
      }
  }

  Window iceSelectRectInteraction::GetSelectedRect() const
  {
    return Window(min<int> (FirstPoint.x, SecondPoint.x),  // top left corner x
                  min<int> (FirstPoint.y, SecondPoint.y),  // top left corner y
                  max<int> (FirstPoint.x, SecondPoint.x),  // bottom right corner x
                  max<int> (FirstPoint.y, SecondPoint.y)); // bottom right corner y

  }

  void iceSelectRectInteraction::Cleanup()
  {
    // delete the selection rectangle from the window, if it has been painted
    if (mode != select)
      {
        DrawSelectedRect();
      }

    // call base class cleanup (see class specification)
    InteractionHandler::Cleanup();
  }
}

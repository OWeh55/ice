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

#include "visual/SelPntInteraction.h"
#include "macro.h"  // for min/max templates

namespace ice
{
  BEGIN_EVENT_TABLE(iceSelectPointInteraction, wxEvtHandler)
    EVT_LEFT_UP(iceSelectPointInteraction::OnLeftMouseButtonUp)
    EVT_RIGHT_UP(iceSelectPointInteraction::OnRightMouseButtonUp)
    EVT_MIDDLE_UP(iceSelectPointInteraction::OnMiddleMouseButtonUp)
    EVT_MOTION(iceSelectPointInteraction::OnMouseMove)
    EVT_PAINT(iceSelectPointInteraction::OnPaint)
  END_EVENT_TABLE()

  iceSelectPointInteraction::iceSelectPointInteraction(ImageWindow* imagewindow)
    : InteractionHandler(imagewindow),
      SelectedPoint(0, 0)
  {
  }

  bool iceSelectPointInteraction::Init()
  {
    // show the cursor for the first time
    imageWindow->DrawCursor(SelectedPoint);
    return true;
  }

  void iceSelectPointInteraction::OnPaint(wxPaintEvent& PaintEvent)
  {
    // If the window must be painted, we must paint our selection cursor
    // as well, but we have to paint it AFTER the window contents has
    // been painted. We can't just call Event.Skip, because this would
    // only cause the window to repaint after this OnPaint handler has finished.
    GetNextHandler()->ProcessEvent(PaintEvent);
    imageWindow->DrawCursor(SelectedPoint);
  }

  void iceSelectPointInteraction::OnMouseMove(wxMouseEvent& MouseEvent)
  {
    // paint the new position only if it lies within the accepted window
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        return;
      }

    IPoint currentPoint = makeValid(MousePos);

    // Delete the old cursor:  Restoring the original contents of the window
    // could be done in two ways: either repaint the area of the old cursor position
    // (which would be quite time-consuming) or paint the selection cursor in such a
    // fashion that it could be erased easily by redrawing it at the same position.
    // See ImageWindow::DrawCursor
    imageWindow->DrawCursor(SelectedPoint);

    // copy the new cursor value and paint the new cursor
    SelectedPoint = currentPoint;
    imageWindow->DrawCursor(SelectedPoint);
  }

  void iceSelectPointInteraction::OnLeftMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // we accept a left mouse button click only, if it was issued while being inside the image
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }
    SelectedPoint = makeValid(MousePos);
    // this interaction is finished, so we have to tidy up
    Button = LEFTMB;
    Cleanup();
  }

  void iceSelectPointInteraction::OnRightMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // this interaction is finished, so we have to tidy up
    Button = RIGHTMB;
    Cleanup();
  }

  void iceSelectPointInteraction::OnMiddleMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // behave like left mb was pressed
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return ;
      }
    SelectedPoint = makeValid(MousePos);
    Button = MIDDLEMB;
    Cleanup();
  }

  void iceSelectPointInteraction::Cleanup()
  {
    // delete the selection cursor
    imageWindow->DrawCursor(SelectedPoint);

    // call base class cleanup method
    InteractionHandler::Cleanup();
  }

  IPoint iceSelectPointInteraction::GetSelectedPoint() const
  {
    return IPoint(SelectedPoint.x, SelectedPoint.y);
  }
}

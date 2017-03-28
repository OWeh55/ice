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
#include <wx/dcclient.h>  // needed for wxGTK >= 2.5.x 

#include "visual/SelLineInteraction.h"
#include "macro.h"  // for min/max templates

namespace ice
{
  BEGIN_EVENT_TABLE(iceSelectLineInteraction, wxEvtHandler)
    EVT_PAINT(iceSelectLineInteraction::OnPaint)
    EVT_LEFT_UP(iceSelectLineInteraction::OnLeftMouseButtonUp)
    EVT_RIGHT_UP(iceSelectLineInteraction::OnRightMouseButtonUp)
    EVT_MIDDLE_UP(iceSelectLineInteraction::OnMiddleMouseButtonUp)
    EVT_MOTION(iceSelectLineInteraction::OnMouseMove)
  END_EVENT_TABLE()

  iceSelectLineInteraction::iceSelectLineInteraction(ImageWindow* imagewindow)
    : InteractionHandler(imagewindow),
      FirstPoint(0, 0),
      SecondPoint(0, 0),
      FirstSelectedFlag(false)
  {
  }

  iceSelectLineInteraction::iceSelectLineInteraction(ImageWindow* imagewindow, IPoint p1)
    : InteractionHandler(imagewindow),
      FirstPoint(p1.x, p1.y),
      SecondPoint(p1.x, p1.y),
      FirstSelectedFlag(true)
  {
    DrawSelectedLine();
  }

  bool iceSelectLineInteraction::Init()
  {
    // nothing to initialize
    return true;
  }

  void iceSelectLineInteraction::DrawSelectedLine()
  {
    if (!FirstSelectedFlag)
      return;

    // We use logical function wxXOR (see explanation at iceSelectPointInteraction::DrawCursor).
    wxClientDC ClientDC(imageWindow);
    ClientDC.SetLogicalFunction(wxXOR);
    unsigned int LineWidth = ice::max<int> (1, imageWindow->GetZoomFactor());
    wxPen Pen(wxColor(128, 128, 128),  // const wxColour& colour
              LineWidth, // int width
              wxSOLID); // int style
    Pen.SetCap(wxCAP_PROJECTING);
    ClientDC.SetPen(Pen);
    ClientDC.SetBrush(*wxTRANSPARENT_BRUSH);

    wxPoint FirstPointWin = imageWindow->translateImage2WinPos(FirstPoint);
    wxPoint SecondPointWin = imageWindow->translateImage2WinPos(SecondPoint);

    ClientDC.DrawLine(FirstPointWin.x + LineWidth / 2,  // wxCoord x1
                      FirstPointWin.y + LineWidth / 2, // wxCoord y1
                      SecondPointWin.x + LineWidth / 2, // wxCoord x2
                      SecondPointWin.y + LineWidth / 2); // wxCoord y2
  }

  void iceSelectLineInteraction::OnPaint(wxPaintEvent& PaintEvent)
  {
    // as well, but we have to paint it AFTER the window contents has
    // been painted. We can't just call Event.Skip, because this would
    // only cause the window to repaint after this OnPaint handler has finished.
    GetNextHandler()->ProcessEvent(PaintEvent);
    DrawSelectedLine();
  }

  void iceSelectLineInteraction::OnMouseMove(wxMouseEvent& MouseEvent)
  {
    IPoint currentPoint = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    // cout << currentPoint.x << " " << currentPoint.y << endl;
    // we only draw the selection if the first corner has already been selected
    if (!FirstSelectedFlag)
      return;


    //    IPoint currentPoint = ImageWindow->TranslateWin2ImagePos(MouseEvent.GetPosition());

    // paint the new position only if it lies within the image
    if (!window.inside(currentPoint))
      return;

    currentPoint = makeValid(currentPoint);

    // delete the old selection
    DrawSelectedLine();

    // update the second corner of the selected line and paint the new selection
    SecondPoint = currentPoint;
    DrawSelectedLine();
  }

  void iceSelectLineInteraction::OnLeftMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // we accept a left mouse button click only, if it was issued while being inside the image
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }

    MousePos = makeValid(MousePos);

    // determine if the chosen corner is the first or the second corner
    if (!FirstSelectedFlag)
      {
        FirstSelectedFlag = true;
        FirstPoint = SecondPoint = MousePos;
        DrawSelectedLine();
      }
    else
      {
        // the user has chosen the second corner, thus the interaction has ended successfully
        SecondPoint = MousePos;
        Button = LEFTMB;
        Cleanup();
      }
  }

  void iceSelectLineInteraction::OnRightMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // this interaction is finished, so we have to tidy up
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());

    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }

    SecondPoint = makeValid(MousePos);

    Button = RIGHTMB;
    Cleanup();
  }

  void iceSelectLineInteraction::OnMiddleMouseButtonUp(wxMouseEvent& MouseEvent)
  {
    // behave like the right button was pressed
    IPoint MousePos = imageWindow->translateWin2ImagePos(MouseEvent.GetPosition());
    if (!window.inside(MousePos))
      {
        ::wxBell();
        return;
      }

    // determine if the chosen corner is the first or the second corner
    if (!FirstSelectedFlag)
      {
        FirstSelectedFlag = true;
        FirstPoint = SecondPoint = makeValid(MousePos);
        DrawSelectedLine();
      }
    else
      {
        // the user has chosen the second corner, thus the interaction has ended successfully
        SecondPoint = makeValid(MousePos);
        Button = MIDDLEMB;
        Cleanup();
      }
  }

  void iceSelectLineInteraction::GetSelectedLine(IPoint& p, IPoint& q) const
  {
    p = IPoint(FirstPoint.x, FirstPoint.y);
    q = IPoint(SecondPoint.x, SecondPoint.y);
  }

  void iceSelectLineInteraction::Cleanup()
  {

    // delete the selected line from the window, if it has been painted
    if (FirstSelectedFlag)
      DrawSelectedLine();

    // call base class cleanup (see class specification)
    DrawSelectedLine();
    InteractionHandler::Cleanup();
  }
}

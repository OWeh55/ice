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
#ifndef __ICESELLINEINTERACTION_H
#define __ICESELLINEINTERACTION_H

#include "visual/Interaction.h"
#include "Point.h"

namespace ice
{
  class iceSelectLineInteraction : public InteractionHandler
  {
  public:
    iceSelectLineInteraction(ImageWindow* imageWindow);
    iceSelectLineInteraction(ImageWindow* imageWindow, IPoint p1);

    void GetSelectedLine(IPoint& p, IPoint& q) const;

    // pure virtual implementation
    virtual bool Init();

  protected:
    // event handllers
    void OnPaint(wxPaintEvent& PaintEvent);
    void OnMouseMove(wxMouseEvent& MouseEvent);
    void OnLeftMouseButtonUp(wxMouseEvent& MouseEvent);
    void OnRightMouseButtonUp(wxMouseEvent& MouseEvent);
    void OnMiddleMouseButtonUp(wxMouseEvent& MouseEvent);

    // The two selected points, that make up the selected line.
    IPoint FirstPoint, SecondPoint;

    // This flag indicates, that the first point has already been selected.
    bool FirstSelectedFlag;

    // Draws the current Selection.
    void DrawSelectedLine();

    // see InteractionHandler::Cleanup
    void Cleanup();

    DECLARE_EVENT_TABLE()
  };
}
#endif // #ifndef __ICESELLINEINTERACTION_H

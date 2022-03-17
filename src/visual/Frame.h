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
#ifndef __ICEFRAME_H
#define __ICEFRAME_H

#include <string>
#include <vector>
#include <map>

#include <wx/frame.h>

namespace ice
{
  class iceFrame : public wxFrame
  {
  public:
    iceFrame(const iceFrame&) = delete; // disable copy constructor

    iceFrame(const wxString& WindowTitle);
    iceFrame(const wxString& WindowTitle, long WindowStyle);

    bool SetChildWindow(wxWindow& ChildWindow);

    virtual bool Destroy();

    virtual bool AddMenuItem(const std::string& menu, const std::string& item, long userid);

  protected:
    void OnClose(wxCloseEvent& Event);
    void OnUserItem(wxCommandEvent& event);

    std::vector<std::string> menuname;
    std::vector<wxMenu*> Menu;

    std::map<long, long> userid;

    //! The child window
    wxWindow* ChildWindow;

    wxMenuBar* MenuBar;

    DECLARE_EVENT_TABLE();
  };
} // namespace ice
#endif // #ifndef __ICEFRAME_H

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

#include <iostream>

#include "visual/App.h"
#include "visual/Frame.h"

using namespace std;
using namespace ice;

BEGIN_EVENT_TABLE(iceFrame, wxFrame)
  EVT_CLOSE(iceFrame::OnClose)
END_EVENT_TABLE()
;

iceFrame::iceFrame(const wxString& WindowTitle)
  : wxFrame(NULL,  // window parent
            wxID_ANY, // window id, we don't need one
            WindowTitle, // window title
            wxPoint(-1, -1),  // window position, we don't care
            wxSize(100, 100)),  // window size, we don't care yet
    ChildWindow(NULL)
{
}

iceFrame::iceFrame(const wxString& WindowTitle, long WindowStyle)
  : wxFrame(NULL,  // window parent
            wxID_ANY, // window id, we don't need one
            WindowTitle, // window title
            wxPoint(-1, -1),  // window position, we don't care
            wxSize(100, 100),
            WindowStyle), // window size, we don't care yet
    ChildWindow(NULL),
    MenuBar(NULL)
{
//  AddMenuItem("File","Open",11);
//  AddMenuItem("File","Close",12);

  // AddMenuItem("Display","On",21);
  // AddMenuItem("Display","Off",22);
  // AddMenuItem("Display","About",24);
  // AddMenuItem("Display","Fullscreen",25);
}

bool iceFrame::SetChildWindow(wxWindow& ChildWindowP)
{
  // if we already host one window, there is something wrong
  if (this->ChildWindow != NULL)
    {
      return false;
    }

  ChildWindow = &ChildWindowP;

  // if we have the focus, we pass it on to the child window

  if (FindFocus() == this)
    {
      ChildWindow->SetFocus();
    }
  return true;
}

bool iceFrame::AddMenuItem(const std::string& menu,
                           const std::string& item, long itemuserid)
{
  if (MenuBar == NULL)
    {
      MenuBar = new wxMenuBar;
      SetMenuBar(MenuBar);
    }

  unsigned int menupos = 0;
  wxMenu* m;
  while (menupos < menuname.size() && menuname[menupos] != menu)
    {
      menupos++;
    }
  if (menupos >= menuname.size())
    {
      wxString wxmenu = wxString(menu.c_str(), wxConvLibc);
      m = new wxMenu;
      Menu.push_back(m);
      menuname.push_back(menu);
      MenuBar->Append(m, wxmenu);
    }
  else
    {
      m = Menu[menupos];
    }

  wxString wxitem = wxString(item.c_str(), wxConvLibc);
  long id = wxNewId();
  userid[id] = itemuserid;
  m->Append(id, wxitem);
  Connect(id, wxEVT_COMMAND_MENU_SELECTED,
          wxCommandEventHandler(iceFrame::OnUserItem));
  return true;
}

void iceFrame::OnClose(wxCloseEvent& Event)
{
  // we don't close the window, but iconize it
  if (Event.CanVeto())
    {
      Event.Veto();
      Iconize(true);
    }
  else
    {
      Destroy();
    }
}

void iceFrame::OnUserItem(wxCommandEvent& event)
{
  long id = event.GetId();
  wxGetApp().SendEvent(userid[id]);
//    wxFrame::ShowFullScreen(true, wxFULLSCREEN_ALL);
}

bool iceFrame::Destroy()
{
  ChildWindow = NULL;
  return wxFrame::Destroy();
}

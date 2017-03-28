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

#include "visual/ConsoleWin.h"
#include "visual/App.h"
#include "visual/screen.h"
#include "visual/xio.h"
#include "defs.h"

namespace ice
{
  void OpenAlpha(const string& windowname)
  {
    wxGetApp().GetConsoleWin()->Open(windowname);
  }

  void CloseAlpha()
  {
    wxGetApp().GetConsoleWin()->Close();
  }

  void Alpha(int mode)
  {
    wxGetApp().GetConsoleWin()->Alpha(mode);
  }

  void PushAlpha()
  {
    wxGetApp().GetConsoleWin()->PushContents();
  }

  void PopAlpha()
  {
    wxGetApp().GetConsoleWin()->PopContents();
  }

  void SetAlphaCursor(int x, int y)
  {
    wxGetApp().GetConsoleWin()->SetCursorPos(iceCursor(x, y));
  }

  void GetAlphaCursor(int* x, int* y)
  {
    iceCursor CursorPos = wxGetApp().GetConsoleWin()->GetCursorPos();
    *x = CursorPos.x;
    *y = CursorPos.y;
  }

  void GetAlphaCursor(int& x, int& y)
  {
    iceCursor CursorPos = wxGetApp().GetConsoleWin()->GetCursorPos();
    x = CursorPos.x;
    y = CursorPos.y;
  }

  void ClearAlpha()
  {
    wxGetApp().GetConsoleWin()->Clear();
  }

  void Bell()
  {
    ::wxBell();
  }

  int PrintErrorAlpha(const std::string& msg, int code, int mode)
  {
    // mode tells us whether we should display the error code (if mode >= 0)
    // if mode is zero, the occured error is fatal and can't be ignored
    // if mode is one, the occured error may be ignored
    if (mode >= 0)
      Printf("%s (%d)\n", msg.c_str(), code);
    else
      Printf("%s\n", msg.c_str());

    // if the error may be ignored go ask the user whether to ignore it or not
    if (mode == 1)
      {
        Printf("Type 'I' to ignore or 'A' to abort\n");
        int Response = toupper(GetChar());
        while (Response != 'A' && Response != 'I')
          Response = toupper(GetChar());
        return (Response == 'I');
      }

    // if a fatal error has occured tell the user so
    if (mode == 0)
      {
        Printf("Press any key to abort\n");
        GetChar();
        return FALSE;
      }

    // if none of the above holds true the error can be ignored anyway
    return TRUE;
  }

  int AlphaSizeX()
  {
    return wxGetApp().GetConsoleWin()->GetScreenSize().x;
  }

  int AlphaSizeY()
  {
    return wxGetApp().GetConsoleWin()->GetScreenSize().y;
  }


  int PutChar(int c)
  {
    return wxGetApp().GetConsoleWin()->PutChar(c);
  }

  int GetChar()
  {
    return wxGetApp().GetConsoleWin()->GetChar();
  }

  int GetCharW()
  {
    return wxGetApp().GetConsoleWin()->GetCharW();
  }

  int GetKey()
  {
    return wxGetApp().GetConsoleWin()->GetKey();
  }

  int GetKeyW()
  {
    return wxGetApp().GetConsoleWin()->GetKeyW();
  }

  void SetAttribute(int FgColor, int BgColor, int Inv, int High)
  {
    wxGetApp().GetConsoleWin()->SetAttribute(FgColor, BgColor, Inv, High);
  }

  void GetAttribute(int& FgColor, int& BgColor, int& Inv, int& High)
  {
    wxGetApp().GetConsoleWin()->GetAttribute(FgColor, BgColor, Inv, High);
  }

  void SetTextAttributes(TextAttribs TextAttribs)
  {
    wxGetApp().GetConsoleWin()->SetTextAttributes(TextAttribs);
  }

  TextAttribs GetTextAttributes()
  {
    return wxGetApp().GetConsoleWin()->GetTextAttributes();
  }
}

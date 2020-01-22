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
    GetAlphaCursor(*x, *y);
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

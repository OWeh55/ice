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
#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include <wx/string.h>

using namespace std;

#include "visual/attribute.h"

namespace ice
{
  // this is the functionality that has to be provided for the manipulation
  // of the alphanumerical window (will be called AlphaWin)
  // The AlphaWin is basically an ordinary graphical window, that means
  // its appearance as a console window is only simulated. The contents of the alphanumerical
  // window can be manipulated by a set of functions that is defined in xio.h.
  // These functions look very much like the common stdio functions (e.g. Printf,
  // Scanf, ...). The whole content of the AlphaWin can be saved on a stack
  // and restored at any time, a convenient feature for building menues.

  // OpenAlpha shows the console window and sets it's caption to >windowname<
  // (if it is already shown OpenAlpha just changes it's caption)
  void OpenAlpha(const string& windowname);
  void CloseAlpha();

  // minimized or maximizes the console window depending on mode
  // mode == ON -> maximize, mode == OFF -> minimize
  void Alpha(int mode);

  // PushAlpha saves the current contents of the console window onto it's internal
  // stack; PopAlpha restores the last saved contents (if there is none nothing happens)
  void PushAlpha();
  void PopAlpha();

  // sets and gets the cursor position inside the console window
  void SetAlphaCursor(int x, int y);
  void GetAlphaCursor(int* x, int* y);
  void GetAlphaCursor(int& x, int& y);

  // well, clears the console
  void ClearAlpha();

  void Bell();

  // get the size (in characters) of the console window
  int AlphaSizeX();
  int AlphaSizeY();

  // prints the character >c< at the current cursor position and moves to the next
  // position (scrolls if necessary)
  int PutChar(int c);

  // wait until the next character will be typed and returns this
  int GetChar(); // as char
  int GetCharW(); // as wchar_t

  // look if a character has been typed;
  // if so return this character, else return 0
  int GetKey(); // as char
  int GetKeyW(); // as wchar_zt

  // setting the back- and foreground colors and text appearance attributes
  // inv==1: text will be displayed inverted (exchanges back- and foreground colors)
  // high==1: text will be displayed highlighted (simply with a brighter color)
  // If you don't want to change the text attributes, you can set the corresponding
  // parameter to -1 (value will be unchanged)
  void SetAttribute(int FgColor, int BgColor, int Inverted, int Highlighted);
  void GetAttribute(int& FgColor, int& BgColor, int& Inverted, int& Highlighted);

  // the same as above, but with more readable parameter names
  void SetTextAttributes(TextAttribs TextAttributes);
  TextAttribs GetTextAttributes();
} // namespace ice
#endif

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

#include <errno.h>

#include "visual/App.h"
#include "visual/screen.h"
#include "visual/ConsoleWin.h"
#include "strtool.h"

#include "visual/xio.h"

#ifdef _MSC_VER
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

namespace ice
{

  // this function is used by all alphaterminal inputs and outputs to activate
  // visualisation if needed;

  int Printf(const char* format, ...)
  {
    // we need a buffer to let the vsprintf do all the work for us
    char buffer[400];

    // extracting the arguments with va_ macros
    va_list Arguments;
    va_start(Arguments, format);
    vsnprintf(buffer, sizeof(buffer), format, Arguments);
    return Print(std::string(buffer));
  }

  int Print(const std::string& s)
  {
    return wxGetApp().GetConsoleWin()->Print(s);
  }

  std::string InputS(const std::string& prompt)
  {
    std::string res;

    Print(prompt);
    InputString(res, false);
    Print("\n");
    return res;
  }

  int InputString(std::string& str, bool control, const std::string& defstr)
  {
    int rc;
    wxGetApp().GetConsoleWin()->InputString(str, control, defstr, rc);
    return rc;
  }

  int Input(const char* prompt)
  {
    return Input(std::string(prompt));
  }

  int Input(const char* prompt, int defValue)
  {
    return Input(std::string(prompt), defValue);
  }

  int Input(const std::string& prompt, int defValue, bool def)
  {
    int startx, starty;
    long int result;
    string defs;
    if (def)
      defs = NumberString(defValue);

    Print(prompt);
    GetAlphaCursor(startx, starty);
    bool ready = false;
    while (!ready)
      {
        string ins;
        SetAlphaCursor(startx, starty);
        int scrolled = InputString(ins, false, defs);
        starty -= scrolled;
        char* last;
        errno = 0;
        result = strtol(ins.c_str(), &last, 10);
        if (errno == 0) ready = true;
      }
    return result;
  }


  int Input(const std::string& prompt)
  {
    return Input(prompt, 0, false);
  }

  int Input(const std::string& prompt, int defaultValue)
  {
    return Input(prompt, defaultValue, true);
  }

  double InputD(const char* prompt)
  {
    return InputD(string(prompt));
  }

  double InputD(const char* prompt, double defaultValue)
  {
    return InputD(string(prompt), defaultValue);
  }

  double InputD(const std::string& prompt, double defaultValue, bool def)
  {
    int startx, starty;
    double result;
    string defs;
    if (def)
      defs = NumberString(defaultValue);

    Print(prompt);
    GetAlphaCursor(startx, starty);
    bool ready = false;
    while (!ready)
      {
        string ins;
        SetAlphaCursor(startx, starty);
        int scrolled = InputString(ins, false, defs);
        starty -= scrolled;
        char* last;
        errno = 0;
        result = strtod(ins.c_str(), &last);
        if (errno == 0) ready = true;
      }
    return result;
  }

  double InputD(const std::string& prompt)
  {
    return InputD(prompt, 0.0, false);
  }

  double InputD(const std::string& prompt, double defaultValue)
  {
    return InputD(prompt, defaultValue, true);
  }

} // namespace ice

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

/*************************************************************/
/* Extended IO                                               */
/*************************************************************/

#ifndef __ICEXIO_H
#define __ICEXIO_H

#include <string>
#include <wx/defs.h>

// defining characters for displaying text boxes
#define R_LO    '*' // 0x8d
#define R_OU    '-' // 0x92
#define R_LU    '*' // 0x8e
#define R_LR    '|' // 0x99
#define R_RU    '*' // 0x8b
#define R_RO    '*' // 0x8c

#define K_Unknown   0xfff

// key code definitions
#define K_Return               WXK_RETURN
#define K_Escape               WXK_ESCAPE
#define K_F1                   WXK_F1
#define K_F2                   WXK_F2
#define K_F3                   WXK_F3
#define K_F4                   WXK_F4
#define K_F5                   WXK_F5
#define K_F6                   WXK_F6
#define K_F7                   WXK_F7
#define K_F8                   WXK_F8
#define K_F9                   WXK_F9
#define K_F10                  WXK_F10
#define K_Home                 WXK_HOME
#define K_Up                   WXK_UP
#ifdef WXK_PRIOR
#define K_PageUp               WXK_PRIOR
#else
#define K_PageUp               WXK_PAGEUP
#endif
#define K_Left                 WXK_LEFT
#define K_Right                WXK_RIGHT
#define K_End                  WXK_END
#define K_Down                 WXK_DOWN
#ifdef WXK_NEXT
#define K_PageDown             WXK_NEXT
#else
#define K_PageDown             WXK_PAGEDOWN
#endif
#define K_Insert               WXK_INSERT
#define K_Delete               WXK_DELETE

namespace ice
{
  /*
   *
   * functions for reading and writing text in the AlphaWin
   *
   */

  int Printf(const char* format, ...);
  int Print(const std::string& s);

  // Reads an int from the AlphaWin and writes the prompt,
  // will repeat reading if input isn't valid

  int Input(const char* Prompt);
  int Input(const char* Prompt, int DefValue);
  int Input(const std::string& Prompt);
  int Input(const std::string& Prompt, int DefValue);

  // same as above, only with doubles
  double InputD(const char* Prompt);
  double InputD(const char* Prompt, double DefValue);
  double InputD(const std::string& Prompt);
  double InputD(const std::string& Prompt, double DefValue);

  std::string InputS(const std::string& Prompt);

  int InputString(std::string& str, bool control = false,
                  const std::string& defstr = "");
}  // namespace ice

#endif // #ifndef __ICEXIO_H

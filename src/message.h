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
/*************************************************************************/
/* Message.h                                                             */
/* Funktion: Hilfsfunktionen für Fehlernachrichten und zum Debuggen      */
/*************************************************************************/
#ifndef _MESSAGE_H  /* verhindern mehrfacher Verarbeitung in einem */
#define _MESSAGE_H  /* Programm */

#include <string>

#include "defs.h"

namespace ice
{
// Standard message - function name, message, error code
  void Message(const std::string& func, const std::string& msg, int code);
// extended message - additional string
  void Message(const std::string& func, const std::string& msg, const std::string& emsg, int code);
// extended message - additional int value
  void Message(const std::string& func, const std::string& msg, int eval, int code);

  int GetError();
  std::string GetErrorString();
  void SetOk();

  void OnMessage();
  void OffMessage();

  extern int ErrorCode;
}

#endif

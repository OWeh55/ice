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
 *
 */

/*
 * ------------------------
 * Message.c
 * System-Meldungen, Fehlerbehandlung
 * ------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <iostream>

#include "strtool.h"
#include "message.h"

#undef Message

namespace ice
{
  using namespace std;

// Fehlerkode
  int ErrorCode;

// Flag "Anzeigen der Nachrichten"
// Werte größer 0 schalten aus
// Wird mit OffMessage und OnMessage geschaltet
// muss immer paarweise verwendet werden (OffMessage...OnMessage);
  int e_message;

  static string e_msg;
  static string dmessage;

  void Message(const string& loc, const string& msg, int code)
  {
    //    printf("%s\n",msg.c_str());
    string message;

    if (msg != "")   // normale Meldung
      {
        /* don't use error message from previous level */
        if (code != OK)
          {
            ErrorCode = 1 - code;
          }
        else
          {
            ErrorCode = OK;
          }

        e_msg = msg;
      }
    else
      {
        // durchgereichte Meldung. Hier wird der Fehlerkode ignoriert!
      }

    message = loc + " - " + e_msg; // Meldung zusammensetzen

    if (e_message <= 0)   // Soll Anzeige erfolgen
      {
        cerr << "Error: " << message << endl;
      }
  }

  void Message(const string& loc, const string& msg, const string& emsg, int code)
  {
    string xmsg;
    size_t pos = msg.find("%s");

    if (pos != string::npos)
      {
        xmsg = msg.substr(0, pos) + emsg + msg.substr(pos + 2);
      }
    else
      {
        xmsg = msg + ": " + emsg;
      }

    Message(loc, xmsg, code);
  }

  void Message(const string& loc, const string& msg, int error, int code)
  {
    string xmsg;
    size_t pos = msg.find("%d");

    if (pos != string::npos)
      {
        xmsg = msg.substr(0, pos) + NumberString(error) + msg.substr(pos + 2);
      }
    else
      {
        xmsg = msg + ": " + NumberString(error);
      }

    Message(loc, xmsg, code);
  }

  void OnMessage()
  {
    if (e_message > 0)
      {
        e_message--;
      }
  }

  int GetError()
  {
    if (ErrorCode == OK)
      {
        return OK;
      }

    return 1 - ErrorCode;
  }

  string GetErrorString()
  {
    return e_msg;
  }

  void SetOk()
  {
    ErrorCode = OK;
  }

  void OffMessage()
  {
    if (e_message >= 0)
      {
        e_message++;
      }
  }
}

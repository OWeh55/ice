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
#ifndef _SERIAL_H
#define _SERIAL_H

#include <string>
using namespace std;
/* -------------------------------------------------------- */
/*       Steuerung der seriellen Schnittstellen             */
/* -------------------------------------------------------- */

namespace ice
{
// ErrorMessages
// All error messages have codes < 0
#define srlOK 0
#define srlNODATA -1
#define srlWINERROR -2
#define srlSYSERROR -2
#define srlBUFFER -3
#define srlNOTOPEN -4
#define srlSTRINGTOOLONG -5
#define srlWRONGPARAMETER -6

// FlowControl
#define srlFLOWNONE 0
#define srlFLOWSOFT 1
#define srlFLOWHARD 2
#define srlFLOWHARDNODTR 3

// StopBits
#define srlONESTOPBIT   0
#define srlONE5STOPBITS 1
#define srlTWOSTOPBITS  2

// string length
#define STRINGLENGTH 300

  int srlOpen(int port,
              int bits = 8,
              int parity = 0,
              int stop = 1,
              int baud = 9600,
              int flow = srlFLOWHARD,
              int timeout = 10);

  int srlWrite(int port, int byte);
  int srlWriteString(int port, const char* s, int eol = 0);
  int srlWriteString(int port, const string& s, int eol = 0);
  int srlRead(int port);
  int srlReadString(int port, char*, int n, int timeout = 60);
  int srlReadString(int port, string& s, int timeout = 60);
  int srlUnRead(int port, int byte);
  int srlClose(int port);
}
#endif

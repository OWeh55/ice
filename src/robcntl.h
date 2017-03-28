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
#ifndef _ROBCNTL_H
#define _ROBCNTL_H
/* -------------------------------------------------------- */
/*                 Robotersteuerung                         */
/* -------------------------------------------------------- */
#include <string>

#include "frames.h"
#include "Vector.h"

using namespace std;

namespace ice
{
#define ROB_DEVICE  0

  int RobOpen();
  void RobClose();

  int RobSendCommand(const string& cmd);

  int RobInit();
  int RobReset();
  int RobSetSpeed(int speed);
  int RobGripper(int stat);
  int RobTeach(const string& prompt);
  int RobMove(double par[6], int mode, int mode2 = 0);
  int RobMove(const Vector& par, int mode);
  int RobMoveFrame(Frame* f, int mode);
  int RobWaitStop();
  int RobWaitStop(int fine);
  int RobCP(int on);
  int RobGetPos(double par[6], int mode = 0);
  int RobGetFrame(Frame* f);
  int RobSetTool(Frame* f);
  int RobSetTool(double rpar[6]);
  int RobSetGripper(int value);
}
#endif


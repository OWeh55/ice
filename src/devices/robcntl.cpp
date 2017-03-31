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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "IceException.h"
#include "robcntl.h"
#include "serial.h"

namespace ice
{
#define TIMEOUT_Q  600      /* Timeout für Quittungssignal bei Befehlen */
#define TIMEOUT_M  600      /* Timeout für warten auf Beendigung von Move */

  static char answer[800];
  /*****************************************************/
#define FNAME "RobSendCommand"
  int RobSendCommand(const string& cmd)
  {
    int ok = false;
//  cout << "Kommando: " << cmd << endl;
    if (srlWriteString(ROB_DEVICE, cmd, 0) != srlOK)
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
      }

    do
      {
        if (srlReadString(ROB_DEVICE, answer, 800) != srlOK)
          {
            throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
          }

//    cout << answer << " - " << cmd << endl;

        if (toupper(answer[1]) == toupper(cmd[0]))
          {
            ok = answer[0] == '+';
          }
      }
    while (toupper(answer[1]) != toupper(cmd[0]));

    if (ok)
      {
        return OK;
      }
    return ERROR;
  }
#undef FNAME
  /*****************************************************/
  int RobOpen(void)
  {
    if (srlOpen(ROB_DEVICE, 8, 0, 1, 9600, srlFLOWHARD) != srlOK)
      {
        return -1;
      }
    if (RobInit() != OK)
      {
        srlClose(ROB_DEVICE);
        return -1;
      }
    return 0;
  }
  /*****************************************************/
  void RobClose(void)
  {
    srlClose(ROB_DEVICE);
    return;
  }
  /*****************************************************/
  int RobInit(void)
  {
    char cmd[80];
    sprintf(cmd, "Init\n");
    return RobSendCommand(cmd);
  }
  /*****************************************************/
  int RobReset(void)
  {
    char cmd[80];
    sprintf(cmd, "Reset\n");
    return RobSendCommand(cmd);
  }
  /*****************************************************/
#define FNAME "RobSetSpeed"
  int RobSetSpeed(int speed)
  {
    char cmd[80];
    if (speed < 0 || speed > 100)
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }
    sprintf(cmd, "Velocity %d\n", speed);
    return RobSendCommand(cmd);
  }
#undef FNAME
  /*****************************************************/
#define FNAME "RobGripper"
  int RobGripper(int stat)
  {
    char cmd[80];

    if (stat == 0)
      {
        sprintf(cmd, "OpenGripper\n");
      }
    else if (stat == 1)
      {
        sprintf(cmd, "closeGripper\n");
      }
    else if (stat == -1)
      {
        sprintf(cmd, "AussenGripper\n");
      }
    else
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
      }
    return RobSendCommand(cmd);
  }
#undef FNAME
  /******************************************************/
#define FNAME "RobSetOpen"
  int RobSetGripper(int value)
  {
    char cmd[80];

    if ((value > 0) && (value < 5000))
      {
        sprintf(cmd, "GripperValue %d\n", value);
      }
    else
      {
        throw IceException(FNAME, M_WRONG_PARAM, WRONG_PARAM);
        return WRONG_PARAM;
      }
    return RobSendCommand(cmd);
  }
#undef FNAME
  /*****************************************************/
  int RobTeach(const string& prompt)
  {
    return RobSendCommand("Handbetrieb " + prompt + "\n");
  }
  /*****************************************************/
  int RobMove(double par[6], int mode, int mode2)
  {
    char cmd[80];
    int code, rc;
    if (mode == 0)
      {
        if (mode2 == 1)
          {
            sprintf(cmd, "a\n");
            if (RobSendCommand(cmd) != OK)
              {
                return ERROR;
              }
          }
        sprintf(cmd, "Move %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
                par[0], par[1], par[2], par[3], par[4], par[5]);
      }
    else
      {
        if (mode2 == 1)
          {
            RobMove(par, 0, 1);
          }
        else
          {
            sprintf(cmd, "Straight %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
                    par[0], par[1], par[2], par[3], par[4], par[5]);
          }
      }
    if ((rc = RobSendCommand(cmd)) != ERROR)
      {
        return rc;
      }
    sscanf(answer, "%*s%d", &code);
    return code;
  }

  int RobMove(const Vector& par, int mode)
  {
    char cmd[80];
    int code, rc;
    if (mode == 0)
      sprintf(cmd, "Move %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
              par[0], par[1], par[2], par[3], par[4], par[5]);
    else
      sprintf(cmd, "Straight %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
              par[0], par[1], par[2], par[3], par[4], par[5]);
    if ((rc = RobSendCommand(cmd)) != ERROR)
      {
        return rc;
      }
    sscanf(answer, "%*s%d", &code);
    return code;
  }

  /*****************************************************/
  int RobMoveFrame(Frame* f, int mode)
  {
    int i;
    double epar[2][6], rpar[6];
    MakeEuler(3, f, epar[0], epar[1]);
    /* y/p/r/X/Y/Z - Repräsentation erzeugen */
    for (i = 0; i < 3; i++)
      {
        rpar[i + 3] = Degree(epar[0][i]);
        rpar[i] = epar[0][i + 3];
      }
    return RobMove(rpar, mode);
  }
  /*****************************************************/
  int RobWaitStop(int fine)
  {
    char cmd[20];
    if (fine)
      {
        sprintf(cmd, "Waitstop\n");
      }
    else
      {
        sprintf(cmd, "Break\n");
      }
    return RobSendCommand(cmd);
  }

  int RobWaitStop(void)
  {
    return RobWaitStop(true);
  }
  /*****************************************************/
  int RobCP(int on)
  {
    if (on)
      {
        return RobSendCommand("CPON\n");
      }
    return RobSendCommand("CPOFF\n");
  }
  /*****************************************************/
#define FNAME "RobGetPos"
  int RobGetPos(double par[6], int mode)
  {
    char cmd[20];
    if (mode == 0)
      {
        sprintf(cmd, "Position\n");
        if (RobSendCommand(cmd) != OK)
          {
            return ERROR;
          }
      }
    else
      {
        sprintf(cmd, "a\n");
        if (RobSendCommand(cmd) != OK)
          {
            return ERROR;
          }
        sprintf(cmd, "p\n");
        if (RobSendCommand(cmd) != OK)
          {
            return ERROR;
          }
      }
    cout << answer << endl;
    if (sscanf(answer, "%*c%*c%le%le%le%le%le%le", &par[0], &par[1], &par[2], \
               &par[3], &par[4], &par[5]) < 6)
      {
        throw IceException(FNAME, M_WRONG_FILE, WRONG_FILE);
      }
    return OK;
  }
#undef FNAME
  /*****************************************************/
  int RobGetFrame(Frame* f)
  {
    int i;
    double epar[6], rpar[6];
    if (RobGetPos(rpar) != OK)
      {
        return ERROR;
      }
    for (i = 0; i < 3; i++)                  /* X/Y/Z/y/p/r  -->  y/p/r/X/Y/Z */
      {
        epar[i] = Arcus(rpar[i + 3]);
        epar[i + 3] = rpar[i];
      }
    MakeFrame(3, epar, f);
    return OK;
  }
  /*****************************************************/
  int RobSetTool(Frame* f)
  {
    int i;
    char cmd[80];
    double epar[2][6], rpar[6];
    MakeEuler(3, f, epar[0], epar[1]);  /* y/p/r/X/Y/Z - Repräsentation erzeugen */
    for (i = 0; i < 3; i++)
      {
        rpar[i + 3] = Degree(epar[0][i]);
        rpar[i] = epar[0][i + 3];
      }
    sprintf(cmd, "Tool %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
            rpar[0], rpar[1], rpar[2], rpar[3], rpar[4], rpar[5]);
    return RobSendCommand(cmd);
  }

  int RobSetTool(double rpar[6])
  {
    char cmd[80];
    sprintf(cmd, "Tool %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\n", \
            rpar[0], rpar[1], rpar[2], rpar[3], rpar[4], rpar[5]);
    return RobSendCommand(cmd);
  }
}

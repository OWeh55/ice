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

#include "defs.h"
#include "serial.h"
#include "kreuzt.h"

namespace ice
{
#define KT_DEVICE  3

#define STEPPMM  80
#define MAXSTEP  8000
#define TIMEOUT  600

#define KRRESET   "Z "
#define KRGOHOME  "MR400 MN A3 V50 GH-2 "
#define KRSETTINGS  "MR400 MN A3 V50 "
#define KRPOSZ    "PZ MPA "
#define KREXIT    " "
#define KRSET_X   "7D"
#define KRSET_Y   "8D"
#define KRGO_X    " 7G "
#define KRGO_Y    " 8G "
#define KRGET_X   "7PR "
#define KRGET_Y   "8PR "
#define KRRQSTAT_X  "7R "
#define KRRQSTAT_Y  "8R "

  /*****************************************************/
  int KtOpen()
  {
    if (srlOpen(KT_DEVICE, 8, 0, 1, 2400, srlFLOWHARD) != srlOK)
      {
        return -1;
      }
    if (KtInit() != OK)
      {
        return -1;
      }
    return 0;
  }
  /*****************************************************/
  void KtClose()
  {
    srlClose(KT_DEVICE);
    return;
  }
  /*****************************************************/
  int KtInit()
  {
    if (srlWriteString(KT_DEVICE, "MR400 MN A4 V50 OSB1 OSC0 OSH1 ", 0) != srlOK)
      {
        return ERROR;
      }
    /*   if(SendCommand(fd,"7MN 7MR400 7A4 7V50 7OSB1 7OSC0 7OSH1 ")!=OK) return(ERROR); */
    /*   if(SendCommand(fd,"8MN 8MR400 8A4 8V50 8OSB1 8OSC0 8OSH1 ")!=OK) return(ERROR); */
    if (KtWaitStop() != OK)
      {
        return ERROR;
      }
    return OK;
  }
  /*****************************************************/
  int KtMove(double x, double y)
  {
    int sx, sy;
    char dx[30], dy[30];
    sx = (int)(x * STEPPMM);
    sy = (int)(y * STEPPMM);
    if ((sx < 0) || (sx > MAXSTEP))
      {
        return ERROR;
      }
    if ((sy < 0) || (sy > MAXSTEP))
      {
        return ERROR;
      }
    sprintf(dx, "7D%d 7G ", sx);
    sprintf(dy, "8D%d 8G ", sy);
    if (srlWriteString(KT_DEVICE, dx, 0) != srlOK)
      {
        return ERROR;
      }
    if (srlWriteString(KT_DEVICE, dy, 0) != srlOK)
      {
        return ERROR;
      }
    return OK;
  }
  /*****************************************************/
  int KtHome(void)
  {
    if (srlWriteString(KT_DEVICE, "GH-3 ", 0) != srlOK)
      {
        return ERROR;
      }
    if (KtWaitStop() != OK)
      {
        return ERROR;
      }
    return OK;
  }
  /*****************************************************/
  int KtWaitStop()
  {
    char line[200];
    if (srlWriteString(KT_DEVICE, "7LF ", 0) != srlOK)
      {
        return ERROR;
      }
    if (srlReadString(KT_DEVICE, line, 200) != srlOK)
      {
        return ERROR;
      }
    if (srlWriteString(KT_DEVICE, "8LF ") != srlOK)
      {
        return ERROR;
      }
    if (srlReadString(KT_DEVICE, line, 200) != srlOK)
      {
        return ERROR;
      }
    return OK;
  }
  /*****************************************************/
}

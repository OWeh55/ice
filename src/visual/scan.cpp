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
// Scan-Modul-Verwaltung
#include <iostream>
using namespace std;

#ifndef WIN32
#include <stdlib.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#include <stdio.h>
#undef ERROR
#include <scan_clipboard.h>
#include <scan_dll.h>
#endif

#include "macro.h"
#include "defs.h"
#include "IceException.h"
#include "visual/screen.h"
#include "strtool.h"

#include "visual/scan_program.h"
#include "visual/scan_file.h"

//#ifndef NOSCANV4L
//#include "scan_v4l/scan_v4l.h"
//#endif
#ifdef SCANFW
#include "scan_fw/scan_fw.h"
#endif

#ifdef _MSC_VER
#include "scan_twain.h"
#include "scan_xfp.h"
#include "scan_vfw.h"
#endif

#include "visual/scan.h"

namespace ice
{
  int is_init = FALSE;
  int channelnum = 0;

  ScanChannelDesc Channel[MAXSCANCHANNEL + 1];

#ifdef _MSC_VER

#define MAXRETSTR 500
  char retstr[MAXRETSTR + 1];
#define CHANNELSTRING(i) GetChannelRegistryStr(i)
#define REG_PATH_2_CHANNEL_DEFS "Software\\FSU Jena\\ICE\\Devices"
#else
#define CHANNELSTRING(i) getenv(("ICESCANDEVICE"+NumberString(i)).c_str())
#endif

#ifdef _MSC_VER
  char* GetChannelRegistryStr(int channel)
  {
    DWORD  Type;  // value type
    DWORD  DataSize = MAXRETSTR; // data buffer size
    HKEY   hkey;  // handle of open key
    int    rc;

    char key[100];
    sprintf(key, "C%04d", channel);

    rc = RegOpenKeyExA(HKEY_CURRENT_USER,
                       REG_PATH_2_CHANNEL_DEFS,
                       0, KEY_READ, &hkey);
    if (rc != ERROR_SUCCESS)
      {
        return FALSE;
      }
    rc = RegQueryValueExA(hkey,
                          key,
                          NULL,
                          &Type,
                          (byte*)retstr,
                          &DataSize);
    RegCloseKey(hkey);
    if (rc != ERROR_SUCCESS || Type != REG_SZ || retstr[0] == 0)
      {
        return NULL;
      }
    return retstr;
  }

  int SetChannelRegistryStr(int channel, char* str)
  {
    if (str == NULL)   // Eintrag löschen
      {

        HKEY   hkey;  // handle of open key
        int    rc;

        char key[100];
        sprintf(key, "C%04d", channel);

        rc = RegOpenKeyExA(HKEY_CURRENT_USER,
                           REG_PATH_2_CHANNEL_DEFS,
                           0, KEY_ALL_ACCESS, &hkey);

        if (rc != ERROR_SUCCESS)
          {
            return FALSE;
          }

        char nullstr[3] = {0};
        rc = RegSetValueExA(hkey,
                            key,
                            0,
                            REG_SZ,
                            (byte*)nullstr,
                            1);

        RegCloseKey(hkey);
        if (rc != ERROR_SUCCESS)
          {
            return FALSE;
          }
      }
    else
      {
        DWORD  Type = REG_SZ; // value type
        DWORD  DataSize = strlen(str) + 1; // data buffer size
        HKEY   hkey;  // handle of open key
        int    rc;
        char key[100];
        sprintf(key, "C%04d", channel);
        rc = RegCreateKeyA(HKEY_CURRENT_USER,
                           REG_PATH_2_CHANNEL_DEFS,
                           &hkey);

        if (rc != ERROR_SUCCESS)
          {
            return FALSE;
          }
        RegCloseKey(hkey);
        rc = RegOpenKeyExA(HKEY_CURRENT_USER,
                           REG_PATH_2_CHANNEL_DEFS,
                           0, KEY_WRITE, &hkey);

        if (rc != ERROR_SUCCESS)
          {
            return FALSE;
          }

        rc = RegSetValueExA(hkey,
                            key,
                            0,
                            Type,
                            (byte*)str,
                            DataSize);
        RegCloseKey(hkey);

        if (rc != ERROR_SUCCESS)
          {
            return FALSE;
          }
      }
    return true;
  }
#endif
#ifdef _MSC_VER
  init_func* initfunctions[] = {&init_program, & init_file, & init_clipboard,
                                & init_twain, & init_dll, & init_xfp, & init_vfw, 0
                               };
#else
  init_func* initfunctions[] = {&init_program, & init_file,
                                //#ifndef NOSCANV4L
                                //           &init_v4l,
                                //#endif
#ifdef SCANFW
                                & init_fw,
#endif
                                0
                               };
#endif

#define FNAME "ScanInit"
  int ScanInit()
  {
    if (is_init)
      {
        return OK;
      }
    int i = 0, ifunc, found;
    string hs;
    string type;
    char* es;

    channelnum = 0;
    while (i < MAXSCANCHANNEL)
      {
        //    cout << CHANNELSTRING(i) << endl;
        if ((es = CHANNELSTRING(i)) != NULL)
          {
            hs = es;
            ifunc = 0;
            found = FALSE;

            while ((initfunctions[ifunc]) != 0)
              if ((*(initfunctions[ifunc]))(i, hs))
                {
                  found = true;
                  break;
                }
              else
                {
                  ifunc++;
                }

            if (found)
              {
                channelnum++;
              }
            else
              {
                cout << "Ignoring scan channel definition " << es << endl;
              }
          }
        i++;
      }
    while (i < MAXSCANCHANNEL)
      {
        Channel[i++].flags = 0;
      }
    is_init = TRUE;
    return OK;
  }
#undef FNAME
#define FNAME "ScanInfo"
  int ScanInfo(int ch, int& xm, int& ym, int& maxval, int& channels,
               int& flags, string& descr)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());
    channels = channelnum;
    //  cout << "info channels " << channels << endl;
    if (ch < 0 || ch >= channels)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);
    //  cout << "info" << endl;
    return (*(Channel[ch].info))(ch, xm, ym, maxval, flags, descr);
  }
#undef FNAME
#define FNAME "ScanWindow"
  int ScanWindow(int ch, int x1, int y1, int x2, int y2)
  {
    int h;
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);
    if (x1 > x2)
      {
        h = x1;
        x1 = x2;
        x1 = h;
      }
    if (y1 > y2)
      {
        h = y1;
        y1 = y2;
        y1 = h;
      }
    if (Channel[ch].xsize > 0)
      {
        if ((x1 < 0) || (y1 < 0) || (x2 > Channel[ch].xsize) || (y2 > Channel[ch].ysize))
          throw IceException(FNAME, M_INVALID_WINDOW);
      }
    Channel[ch].wxi = x1;
    Channel[ch].wyi = y1;
    Channel[ch].wxa = x2;
    Channel[ch].wya = y2;
    return OK;
  }
#undef FNAME
#define FNAME "ScanDialog"
  int ScanDialog(int ch)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    if ((Channel[ch].flags & SC_DIALOG) != 0)
      {
        return (*(Channel[ch].dialog))(ch);
      }
    return OK;
  }
#undef FNAME
#define FNAME "ScanPreview"
  int ScanPreview(int ch, int on)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    if ((Channel[ch].flags & SC_PREVIEW) != 0)
      {
        return (*(Channel[ch].preview))(ch, on);
      }
    else
      throw IceException(FNAME, M_NO_PREVIEW);
  }
#undef FNAME
#define FNAME "ScanSetProperties"
  int ScanSetProperties(int ch,
                        int brightness, int contrast,
                        int saturation, int hue)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);
    return (*(Channel[ch].setproperties))(ch,
                                          brightness, contrast,
                                          saturation, hue);
  }
#undef FNAME
#define FNAME "ScanGetProperties"
  int ScanGetProperties(int ch,
                        int& brightness, int& contrast,
                        int& saturation, int& hue)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    return (*(Channel[ch].getproperties))(ch,
                                          brightness, contrast,
                                          saturation, hue);
  }
#undef FNAME
#define FNAME "ScanImg"
  int ScanImg(int ch, Image& pi, int interactive)
  {
    ibuffer ib;
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    if (!IsImg(pi))
      throw IceException(FNAME, M_WRONG_IMAGE);
    ib.width = pi->xsize;
    ib.height = pi->ysize;
    ib.maxval = pi->maxval;
    ib.planes = 1;

    if (interactive)
      {
        if ((Channel[ch].flags & SC_PREVIEW) > 0)
          {
            (*(Channel[ch].preview))(ch, TRUE);
            while (GetChar() != 13);
            (*(Channel[ch].preview))(ch, FALSE);
            interactive = FALSE;
          }
      }

    do
      {
        if ((*(Channel[ch].grab))(ch, ib) != OK)
          throw IceException(FNAME, M_SCAN_ERROR);
        Buffer2Image(ib, pi, IB_SCALE);
        if (interactive)
          if (GetKey() == 13)
            {
              interactive = FALSE;
            }
      }
    while (interactive);
    return OK;
  }

  int ScanImg(int ch, Image& pr, Image& pg, Image& pb, int interactive)
  {
    int xs, ys;
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    RETURN_ERROR_IF_FAILED(MatchImg(pr, pg, pb, xs, ys));

    ibuffer ib;

    ib.width = xs;
    ib.height = ys;
    ib.maxval = pr->maxval;
    ib.planes = 3;
    ib.byteorder = IB_LSB_FIRST;

    if (interactive)
      {
        if ((Channel[ch].flags & SC_PREVIEW) > 0)
          {
            (*(Channel[ch].preview))(ch, TRUE);
            while (GetChar() != 13);
            (*(Channel[ch].preview))(ch, FALSE);
            interactive = FALSE;
          }
        else
          {
            if (!IsImg(pr))
              {
                interactive = FALSE;  // interactive nicht ohne sichtbares Bild
              }
          }
      }

    do
      {
        if ((*(Channel[ch].grab))(ch, ib) != OK)
          throw IceException(FNAME, M_SCAN_ERROR);
        Buffer2Image(ib, pr, pg, pb, IB_SCALE);
        if (interactive)
          if (GetKey() == 13)
            {
              interactive = FALSE;
            }
      }
    while (interactive);
    return OK;
  }
#undef FNAME

#define FNAME "ScanSetExtData"
  int ScanSetExtData(int ch, int datanr, int val, int issubchannel)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    if ((Channel[ch].flags & SC_GLOBALDRIVER) != 0)
      {
        return (*(Channel[ch].setdriverextdata))(ch, datanr, val, issubchannel);
      }

    return OK;
  }
#undef FNAME
#define FNAME "ScanGetExtData"
  int ScanGetExtData(int ch, int datanr, int& val, int& issubchannel, string& desc)
  {
    RETURN_ERROR_IF_FAILED(ScanInit());

    if (ch < 0 || ch >= channelnum)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    if ((Channel[ch].flags & SC_SCAN) == 0)
      throw IceException(FNAME, M_NO_SCANDEVICE);

    if ((Channel[ch].flags & SC_EXTDATA) != 0)
      {
        return (*(Channel[ch].getdriverextdata))(ch, datanr, val, issubchannel, desc);
      }

    throw IceException(FNAME, "No global driver");
  }

  int ScanGetExtData(int ch, int datanr, int& val)
  {
    string sdummy;
    int idummy;

    RETURN_ERROR_IF_FAILED(ScanGetExtData(ch, datanr, val, idummy, sdummy));
    return OK;
  }
#undef FNAME
}

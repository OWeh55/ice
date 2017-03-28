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
#ifndef SCANDRV_H
#define SCANDRV_H

#define SC_SCAN 1
#define SC_RGB (SC_SCAN<<1)
#define SC_PREVIEW (SC_RGB<<1)
#define SC_DIALOG (SC_PREVIEW<<1)
#define SC_GLOBALDRIVER (SC_DIALOG<<1)
#define SC_EXTDATA SC_GLOBALDRIVER

#define MAXSCANCHANNEL 200

#include <string>
#include "ibuffer.h"

using namespace std;

namespace ice
{
// Treiber-Funktionen
  typedef int init_func(int ch, string config);
  typedef int info_func(int ch, int& x, int& y, int& maxval,
                        int& flags, std::string& descr);
  typedef int preview_func(int ch, int on);
  typedef int dialog_func(int ch);
  typedef int grab_func(int ch, ibuffer& ib);
  typedef int getproperties_func(int ch, int& brightness, int& contrast,
                                 int& saturation, int& hue);
  typedef int setproperties_func(int ch, int brightness, int contrast,
                                 int saturation, int hue);

  typedef int getdriverextdata_func(int ch, int datanr,
                                    int& val, int& issubchannel, std::string& desc);
  typedef int setdriverextdata_func(int ch, int datanr,
                                    int val, int issubchannel);

  typedef struct scan_channel_desc
  {
    int flags;
    info_func* info;
    preview_func* preview;
    dialog_func* dialog;
    grab_func* grab;
    getproperties_func* getproperties;
    setproperties_func* setproperties;
    getdriverextdata_func* getdriverextdata;
    setdriverextdata_func* setdriverextdata;
    int xsize, ysize, maxval;
    int wxi, wyi, wxa, wya; // aktuelles Fenster
    void* driverdata;
  } ScanChannelDesc;

// zusätzliche Treiber-Function, um Environment-Einträge zu erzeugen
  typedef int add_func(int ch, ScanChannelDesc* desc);

  extern ScanChannelDesc Channel[MAXSCANCHANNEL + 1];
  extern int channelnum;

// Öffentliche (Anwender-) Funktionen
  int ScanInit();
  int ScanInfo(int ch, int& xm, int& ym, int& maxval, int& channels,
               int& flags, std::string& descr);
  int ScanWindow(int ch, int x1, int y1, int x2, int y2);
  int ScanDialog(int ch);
  int ScanPreview(int ch, int on);
  int ScanSetProperties(int ch,
                        int brightness, int contrast,
                        int saturation, int hue);
  int ScanGetProperties(int ch,
                        int& brightness, int& contrast,
                        int& saturation, int& hue);
  int ScanImg(int ch, Image& pi, int interactive = true);
  int ScanImg(int ch, Image& pr,
              Image& pg, Image& pb, int interactive = true);

// optional functions to set or get extended data for channel (settings etc)
// datanr consists of "featurenumber" (0..1023) and option (n*1024)

#define OPTION_BASE 1024
// request for current value must be 0 !
#define FEATURE_CURRENT 0*OPTION_BASE
#define FEATURE_GETMIN 1*OPTION_BASE
#define FEATURE_GETMAX 2*OPTION_BASE
#define FEATURE_CANSET 3*OPTION_BASE
#define FEATURE_CANREAD 4*OPTION_BASE
#define FEATURE_HASAUTO 5*OPTION_BASE
#define FEATURE_ISAUTO 6*OPTION_BASE
#define FEATURE_AUTO 7*OPTION_BASE

  int ScanSetExtData(int ch, int datanr, int val, int issubchannel = 0);
  int ScanGetExtData(int ch, int datanr, int& val, int& issubchannel, std::string& desc);
  int ScanGetExtData(int ch, int datanr, int& val);

#ifdef WIN32

  char* GetChannelRegistryStr(int channel);
  int SetChannelRegistryStr(int channel, char* str);

#endif
}

#endif

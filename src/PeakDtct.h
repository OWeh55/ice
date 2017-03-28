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
#ifndef _PEAKDETECTION_H
#define _PEAKDETECTION_H
/*
 * Funktionen zur Peakdetektion
 * ttbb 6'1998
 * Wolfgang Ortmann 2016
 */

#include "base.h"
#include "Contur.h"
#include "contlist.h"
#include "PeakVal.h"

// Neue Struktur zum Aufbau einer Peakliste
// (x,y) Position des Peak-Maximums im Bild
// grw   Maximaler Grauwert des Peaks
// pl_max,pl_min momentan unbenutzt
// prev, next sind Zeiger zur Listenverkettung

namespace ice
{
  typedef struct _PeakList
  {
    int     grw;
    ConturList contur;
    ConturList contur_max;
    double  sx, sy, sxy, x, y;
    _PeakList* prev, *next;
  }* PeakList;


// Anlegen einer neuen _PeakList-Struktur, initialisieren der Member und
// Rückgabe eines Zeigers auf diese Struktur
// Bei Fehler (Speichermangel) ist Rückgabe == NULL
  PeakList NewPeakList();

// Sucht Peaks in einem Grauwertbild in monoton fallender Folge ihrer Maxima
// maxanz bestimmt die maximale Anzahl der Peaks, die gesucht werden soll
// mingrw bestimmt den Mindestgrauwert eines lokalen Maximum, um als Peak zu gelten
// zykl legt fest, ob die Bilder dem 2fach zyklischen Modell entsprechen
// noise legt die Höhe von Nebenpeaks fest, um als Rauschen zu gelten

#define IPL_MINCONTUR 1
#define IPL_MAXCONTUR 2
#define IPL_STANDARD  4

#define IPL_NOFEAT  0
#define IPL_ALL     0xFFFF

  PeakList ImgPeakList(const Image&, const Image&, int maxanz = 0, int mingrw = 0,
                       int zykl = false, int noise = 0, int feat = IPL_NOFEAT,
                       int gnull = 0, int mode = PN_NORMAL);

  // Gibt den gesamten Speicher einer Peakliste frei
  void FreePeakList(PeakList);

  ConturList PeakConturList(const Image&, int x, int y, int flag = IPL_MINCONTUR,
                            int zykl = false, int noise = 0);
}
#endif

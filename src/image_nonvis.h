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
#ifndef _IMAGE_NONVIS_H
#define _IMAGE_NONVIS_H

/*
 * image_nonvis.h
 * Funktion: Generelles Include File fuer ICE, welches alle fuer ICE
 * erforderlichen Header-Dateien außer der Visualisierung einschliesst.
 * Diese Datei sollte nur fuer eigene _einfache_ Programme verwendet werden.
 * Wiederverwendbare Funktionen (Funktionen fuer den Einbau in die Bibliothek)
 * sollten nur die notwendigen Header-Dateien einschliessen, um den Overhead
 * bei der Neu-Uebersetzung zu verringern.
 * Alle ICE-Funktionen muessen im namespace ice angesiedelt sein. Dies muss
 * in den einzelnen Header-Dateien erfolgen!
 */

// *********************
// * Headerfiles       *
// *********************

#include "ice_common.h"

// nur diese Header-Datei darf die using namespace-Anweisung enthalten!
using namespace ice;

#endif /*#ifndef _IMAGE_NONVIS_H */

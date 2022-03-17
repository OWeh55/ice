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
/*****************************************************************************/
/*                                                                           */
/* segment.h                                                                 */
/*  Segmentierung von Punktlisten                                            */
/*   (c) by FeB                                     nbe@mipool.uni-jena.de   */
/*   14.1.97                                                                 */
/*                                                                           */
/*  SegmentPointList - Segmentiert die Punkliste pl je nach mode in Linien-  */
/*                     Kreis- & Ellipsensegmente.                            */
/*                     mode : 0 - Linien & Kreise & Ellipsen (DEFAULT)       */
/*                            1 - Linien & Kreise                            */
/*                            2 - Linien                                     */
/*                     Das zur¸ckgegebene Segment ist das erste einer        */
/*                     doppelt verketten Liste des Typs Segment.             */
/*                     Im Fehlerfall wird ein NULL-Pointer zur¸ckgegeben.    */
/*****************************************************************************/
#ifndef _SEGMENT_H
#define _SEGMENT_H

#include "fit.h"

namespace ice
{
  Segment SegmentPointList(PointList pl, int mode);

  /* ***************************************************************************

  Release 03.06.1997 - ttbb - Macht interne Rosin-Funktionen zugaenglich

  DetermineSegment - Passt genau ein Segment an den Aussschnitt [pa,pe]
  der Punktliste pl an.
  Als Segmenttyp stehen
  type==1(DS_LINE)     -> Liniensegment
  type==2(DS_CIRCLE)   -> Kreissegment
  type==4(DS_ELLIPSE)  -> Ellipsensegment
  oder eine ODER-Verknuepfung dieser Typen zur VerfÅgung.
  Bei einer ODER-Verknuepfung wird in Abhaengigkeit vom
  maximalen Abstand der beste Segmenttyp ausgewaehlt.
  Das neue Segment wird an die in sg_liste uebergebene
  Segment-Liste angehaengt. Ist sg_liste==NULL, wird
  eine einelementige Liste erzeugt.

  Rueckgabe: Zeiger auf das neue Segment in der Segment-Liste
  (bzw. NULL bei aufgetretenem Fehler)
  Auf max_dev steht der maximale Abstand und
  auf max_adr der zugehoerige Punkt zur Verfuegung.
  */

#define DS_LINE   ((unsigned int)1)
#define DS_CIRCLE ((unsigned int)2)
#define DS_ELLIPSE  ((unsigned int)4)

  Segment DetermineSegment(
    PointList pl, int pa, int pe,
    unsigned int type,
    Segment sg_liste,
    int* max_adr,
    double* max_dev
  );

  /* ***************************************************************************

  Release 03.06.1997 - ttbb

  SegmentPointListEx - Segmentiert eine Punktliste mit dem CSegment-
  Algortihmus (DIAS)

  Parameterliste/Rueckgabe analog SegmentPointList
  Zusaetzlich kann der maximal zulaessige Abstand
  vorgegeben werden.
  Die Modi SPL_LINE,SPL_LINECIRC,SPL_ALL schliessen
  sich gegenseitig aus, koennen jedoch mit
  SPL_NOCLOSE ODER-verknuepft werden, wenn bei
  geschlossenen Punktlisten das erste/letzte Segment
  am ersten/letzten Punktlistenpunkt anfangen/
  aufhoeren sollen.

  Release 10.06.1997 - Bidirektionale Segmentierung mit Modus SPL_BIDIRECT
  */

#define SPL_LINE   ((unsigned int)2)
#define SPL_LINECIRC ((unsigned int)1)
#define SPL_ALL      ((unsigned int)0)

#define SPL_BIDIRECT ((unsigned int)64)
#define SPL_NOCLOSE  ((unsigned int)128)

  Segment SegmentPointList(PointList pl, unsigned int mode, double max_dev);
}
#endif /* _SEGMENT_H */

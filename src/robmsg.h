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
/******************************************
 *  Fehlermeldungen Robotvision           *
 ******************************************/

#ifndef _ROBMSG_H
#define _ROBMSG_H

#define  RM_NO_BASIS               "basis vectors are coplanar"
#define  RM_COLINEAR               "collinear points"
#define  RM_NOT_ENOUGHT_POINTS     "not enought point pairs"
#define  RM_NOT_ENOUGHT_CAMERAS    "not enought cameras"
#define  RM_WONG_POINT_CONFIG      "wrong point configuration"
#define  RM_TWO_CAMERAS            "needs at least two cameras"
#define  RM_WRONG_VERTEX           "wrong vertex"
#define  RM_WRONG_EDGE             "wrong edge"
#define  RM_WRONG_MESH             "wrong mesh"
#define  RM_WRONG_OBJECT           "wrong object"
#define  RM_WRONG_VERTEX_AT_EDGE   "inconsistence: wrong vertex at edge"
#define  RM_WRONG_EDGE_AT_VERTEX   "inconsistence: wrong edge at vertex"
#define  RM_WRONG_EDGE_AT_MESH     "inconsistence: wrong edge at mesh"
#define  RM_WRONG_MESH_AT_EDGE     "inconsistence: wrong mesh at edge"
#define  RM_NOEMPTY_VERTEX         "tryed to delete noempty vertex"
#define  RM_INCOSISTENCE           "inconsistent description structure"

#endif

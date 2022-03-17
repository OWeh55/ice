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
#ifndef FOURIERDESCRIPTOR_H
#define FOURIERDESCRIPTOR_H

// we use vector point as source == list of Points
// we use vector point as result == complex FD
#include <vector>
#include "Point.h"

void computeFourier(const std::vector<ice::Point>& v1,
                    std::vector<ice::Point>& v2,
                    bool forward = true);

void zeroPaddingFD(const std::vector<ice::Point>& v1, int newsize,
                   std::vector<ice::Point>& v2);

void invertOrderFD(std::vector<ice::Point> fk, std::vector<ice::Point>& fd);

void scaleFD(std::vector<ice::Point> fk, double s,
             std::vector<ice::Point>& fd);

void shiftFD(std::vector<ice::Point> fk, ice::Point sh,
             std::vector<ice::Point>& fd);

void rotateFD(std::vector<ice::Point> fk, double phi,
              std::vector<ice::Point>& fd);

void normalizeFDOrder(std::vector<ice::Point> fk, std::vector<ice::Point>& fd);

void normalizeFDScaling(std::vector<ice::Point> fk,
                        std::vector<ice::Point>& fd);

void normalizeFDShift(std::vector<ice::Point> fk,
                      std::vector<ice::Point>& fd);

void normalizeFDIShift(std::vector<ice::Point> fk,
                       std::vector<ice::Point>& fd);
void normalizeFDRotation(std::vector<ice::Point> fk,
                         std::vector<ice::Point>& fd);
void normalizeFDIShiftRotation(std::vector<ice::Point> fk,
                               std::vector<ice::Point>& fd);

void normalizeFDEuclidian(std::vector<ice::Point> fk,
                          std::vector<ice::Point>& fd);

#endif

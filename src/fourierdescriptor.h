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

void scaleFD(std::vector<ice::Point> fk, double s,
             std::vector<ice::Point>& fd);

void shiftFD(std::vector<ice::Point> fk, ice::Point sh,
             std::vector<ice::Point>& fd);

void rotateFD(std::vector<ice::Point> fk, double phi,
              std::vector<ice::Point>& fd);

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

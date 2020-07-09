#ifndef FOURIERDESCRIPTOR_H
#define FOURIERDESCRIPTOR_H

// we use vector point as source == list of Points
// we use vector point as result == complex FD
#include <vector>
#include "Point.h"

void computeFourier(const std::vector<ice::Point>& v1,
                    std::vector<ice::Point>& v2,
                    bool forward = true);

void normalizeFDScaling(const std::vector<ice::Point>& fk,
                        std::vector<ice::Point>& fd);
void normalizeFDShift(const std::vector<ice::Point>& fk,
                      std::vector<ice::Point>& fd);

void normalizeFDIShift(const std::vector<ice::Point>& fk,
                       std::vector<ice::Point>& fd);
void normalizeFDRotation(const std::vector<ice::Point>& fk,
                         std::vector<ice::Point>& fd);
void normalizeFDIShiftRotation(const std::vector<ice::Point>& fk,
                               std::vector<ice::Point>& fd);

void normalizeFDEuclidian(const std::vector<ice::Point>& fk,
                          std::vector<ice::Point>& fd);

#endif

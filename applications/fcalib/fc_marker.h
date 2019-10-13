#ifndef FC_MARKER_H
#define FC_MARKER_H

#include "Point.h"
#include "base.h"

//int FindMarker(const ice::Image& oimg, const ice::Image& mark, ice::Point& p);
bool MarkerSearch(const ice::Image& oimg, const ice::Image& mark,
                  ice::Trafo& tr, ice::Distortion& dist,
                  std::vector<ice::Point>& markers, std::vector<ice::Point>& reference);

#endif

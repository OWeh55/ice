#ifndef FC_POLY_H
#define FC_POLY_H

#include <vector>

#include <Point.h>
#include <polygon.h>
#include <Distortion.h>

ice::Polygon ReducePolygon(const ice::Polygon& p, int nr, int mode = 1);
void PointlistWithOffset(const std::vector<ice::Point>& src, int offset, std::vector<ice::Point>& dst);
double PointlistError(const std::vector<ice::Point>& pl1, const std::vector<ice::Point>& pl2, double minerror);
bool findPoly(const ice::Image& oimg, const ice::Image& mark, ice::Trafo& tr);

#endif

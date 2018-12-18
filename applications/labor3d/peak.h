#ifndef PEAK
#define PEAK

#include "matrix3d.h"
#include <image3d.h>

double PeakEvaluation(const ice::Matrix3d &m, double xm, double ym, double zm);
int markMax(const ice::Image3d<ice::Image> &val, ice::Image3d<ice::Image> &mark, ice::IVector3d p);
int markMax(const ice::Matrix3d &val, ice::IMatrix3d &mark, ice::IVector3d p);
bool isMax(const ice::Image3d<ice::Image> &val, ice::IVector3d p);
bool isMax(const ice::IMatrix3d &val, ice::IVector3d p);
#endif

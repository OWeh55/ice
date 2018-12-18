#ifndef SDR_H
#define SDR_H

#include <image3d.h>
#include "matrix3d.h"

bool InvConvolution(const ice::Image3d<ice::Image> &img1,
                    const ice::Image3d<ice::Image> &img2,
                    int x, int y, int z,
                    int wsizex, int wsizey, int wsizez,
                    double beta, int resfac,
                    ice::Matrix3d &res);

bool InvConvolution(const ice::Image3d<ice::Image> &img1,
                    const ice::Image3d<ice::Image> &img2,
                    const ice::IVector3d &p,
                    int wsizex, int wsizey, int wsizez,
                    double beta, int resfac,
                    ice::Matrix3d &res);
#endif

#ifndef FFT3D_H
#define FFT3D_H

#include "matrix3d.h"

void fft(ice::Matrix3d &real, ice::Matrix3d &imag, bool forward = true);
void fft(const ice::Matrix3d &src, ice::Matrix3d &real, ice::Matrix3d &imag, bool forward = true);

#endif

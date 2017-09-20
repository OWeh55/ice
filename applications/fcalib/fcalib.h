#ifndef FCALIB_H
#define FCALIB_H

const int v_step = 1;
const int v_opt = 2;
const int v_trafo = 4;
const int v_marker = 8;
const int v_assign = 16;
extern int Verbose;
extern int wait;

const int output_distortion = 1;
const int output_intrinsic = 2;
const int output_distdb = 4;

extern const int pattern_corners;
extern const double pattern_x[];
extern const double pattern_y[];

extern const int pattern_cols;
extern const double pattern_x0;
extern const double pattern_dx;

extern const int pattern_rows;
extern const double pattern_y0;
extern const double pattern_dy;

#endif

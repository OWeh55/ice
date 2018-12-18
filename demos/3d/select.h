#ifndef SELECT_H
#define SELECT_H

#include <image.h>

#define ZOOMFAK 4
#define SELWINDOWSIZE 32
#define ZOOMSIZE ((SELWINDOWSIZE)*2*(ZOOMFAK))

int MyMarker(int mode, int x, int y, int col, int size, const Image &img);
int MySelPoint(int mode,const Image &img, Image &ov,int p[2]);

#endif

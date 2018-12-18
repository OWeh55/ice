#ifndef DEPTH_H
#define DEPTH_H
#include <image.h>
#include "image3d.h"

void DepthImage(const Image3d<Image>& src, int level, Image& depth, Image& depthvalid, int depthfactor = 1);

void DepthImage(const Image3d<Image>& src, int level, IVector3d current,
                Image& depth, Image& depthvalid, char dir, int depthfactor = 1);

void ObjImage(const Image3d<Image>& src, int level, IVector3d current,
              Image3d<Image>& obj);

#endif

/*
 * ICE - Library for image processing in C++
 *
 * Copyright (C) 1992..2018 FSU Jena, Digital Image Processing Group
 * Copyright (C) 2019..2022 Wolfgang Ortmann
 * Contact: ice@ortmann-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VELOCIMETRYUTIL
#define VELOCIMETRYUTIL

#include "based.h"
#include "fourier.h"
#include "convolution_fft.h"

#include "drawline.h"
#include "filter.h"
#include "darith.h"
#include "arith.h"
#include "paint.h"

#include <cstdlib>
#include <cmath>
#include <float.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <queue>

#include <list>

namespace ice
{

  int directCrossCorrelation(ImageD img1, ImageD img2, ImageD img3, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);
  int directCrossCorrelation(ImageD img1, ImageD img2, ImageD img3, int x, int y, int w, int h, int offsetX, int offsetY, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);
  int directCrossCorrelation(Image img1, Image img2, Image img3, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);
  int directCrossCorrelation(Image img1, Image img2, Image img3, int x, int y, int w, int h, int offsetX, int offsetY, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);

  double directCrossCorrelation(Image img1, Image img2, int x1, int y1, int w, int h, int xd, int yd);

  int directSquaredError(ImageD img1, ImageD img2, ImageD img3, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY) ;
  int directSquaredError(ImageD img1, ImageD img2, ImageD img3, int x1, int y1, int w, int h, int offsetX, int offsetY, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);

  int directSquaredError(Image img1, Image img2, Image img3, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY) ;
  int directSquaredError(Image img1, Image img2, Image img3, int x1, int y1, int w, int h, int offsetX, int offsetY, int minDisplacementX, int minDisplacementY, int maxDisplacementX, int maxDisplacementY);

  ImageD crossCorrelationImgD(ImageD is1, ImageD is2, ImageD id, int mode = MD_USE_BIAS);

  ImageD phaseCorrelationD(ImageD is1, ImageD is2, ImageD id, double noise = 0.0, int mode = MD_USE_BIAS);
  Image phaseCorrelation(Image img1, Image img2, Image img3, double beta = 0);

// HornSchunck/Optischer Fluß

  int horn(Image img1, Image img2, ImageD xDelta, ImageD yDelta, double lambda = 1, int iterationNumber = 100);
  int showOpticalFlow(Image img, ImageD xdelta, ImageD ydelta, int subSampleSize = 4, double vectorScale = 1, int color = 1);

// Hilfsfunktionen

  void Flip(Image img);
  void drawArrow(int x1, int y1, int x2, int y2, int val, int size, Image img);
  int  parseParaFile(const char* filename, std::map<std::string, std::string>& paraMap);
  void shiftWindowD(ImageD imgD1, ImageD imgD2, int shiftX);
  void shiftWindow(Image img1, Image img2, int shiftX);
  Image pivRenormImg(Image img1, Image img2);
  Image pivMexicanImg(Image img1, int size, double sigma1, double sigma2, Image img2);
  void makePositivImgD(ImageD img);
}
#endif


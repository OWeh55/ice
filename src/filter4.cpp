/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "defs.h"
#include "macro.h"

#include "lsifilter.h"
#include "filter.h"
#include "util.h"

namespace ice
{
#define FNAME "OrientedSmearImg"
  int OrientedSmearImg(const Image& pic,
                       const Image& dir,
                       const Image& dest,
                       int filter_size, int filter_length, int filter_width)
  {
    int dimx, dimy;
    ReturnErrorIfFailed(MatchImg(pic, dir, dest, dimx, dimy));

    int dirsteps = dir->maxval + 1;
    double dirfac = M_PI / dirsteps;

    // Filter-Bank Smear
    vector<LSIFilter> fsmear;

    for (int i = 0; i < dirsteps; ++i)
      {
        double fi = i * dirfac + M_PI * 0.5;
        fsmear.push_back(mkOrientedSmearFilter(filter_size, fi, filter_length, filter_width));
      }

#ifdef OPENMP
    #pragma omp parallel for schedule(dynamic,20)
#endif

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          int richtungs_index = GetVal(dir, x, y);
          int g = GetVal(pic, x, y, fsmear[richtungs_index]);
          PutVal(dest, x, y, limited(g, dest));
        }

    return 0;
  }

  int OrientedSmearImg(const ColorImage& src,
                       const Image& dir,
                       const ColorImage& dest,
                       int filter_size, int filter_length, int filter_width)
  {
    ReturnErrorIfFailed(src.match(dest));
    ReturnErrorIfFailed(OrientedSmearImg(src.redImage(), dir, dest.redImage(), filter_size, filter_length, filter_width));
    ReturnErrorIfFailed(OrientedSmearImg(src.greenImage(), dir, dest.greenImage(), filter_size, filter_length, filter_width));
    ReturnErrorIfFailed(OrientedSmearImg(src.blueImage(), dir, dest.blueImage(), filter_size, filter_length, filter_width));
    return OK;
  }


#undef FNAME
#define FNAME "OrientedEdgeImg"
  int OrientedEdgeImg(const Image& pic,
                      const Image& dir,
                      const Image& dest,
                      int filter_size, int filter_rad)
  {
    int dimx, dimy;
    ReturnErrorIfFailed(MatchImg(pic, dir, dest, dimx, dimy));

    int dirsteps = dir->maxval + 1;
    double dirfac = 2 * M_PI / dirsteps;

    // Filter-Bank Smear
    vector<LSIFilter> fedge;

    for (int i = 0; i < dirsteps; ++i)
      {
        double fi = i * dirfac;
        fedge.push_back(mkOrientedEdgeFilter(filter_size, fi, filter_rad));
      }

#ifdef OPENMP
    #pragma omp parallel for schedule(dynamic,20)
#endif

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          int richtungs_index = GetVal(dir, x, y);
          int g = GetVal(pic, x, y, fedge[richtungs_index]) + dest->maxval / 2;
          PutVal(dest, x, y, limited(g, dest));
        }

    return 0;
  }

  int OrientedEdgeImg(const ColorImage& src,
                      const Image& dir,
                      const ColorImage& dest,
                      int filter_size, int filter_rad)
  {
    ReturnErrorIfFailed(src.match(dest));
    ReturnErrorIfFailed(OrientedEdgeImg(src.redImage(), dir, dest.redImage(), filter_size, filter_rad));
    ReturnErrorIfFailed(OrientedEdgeImg(src.greenImage(), dir, dest.greenImage(), filter_size, filter_rad));
    ReturnErrorIfFailed(OrientedEdgeImg(src.blueImage(), dir, dest.blueImage(), filter_size, filter_rad));
    return OK;
  }

#undef FNAME
#define FNAME "OrientedDoBImg"
  int OrientedDoBImg(const Image& pic,
                     const Image& dir,
                     const Image& dest,
                     int filter_size, int filter_length, int filter_width)
  {
    int dimx, dimy;
    ReturnErrorIfFailed(MatchImg(pic, dir, dest, dimx, dimy));

    int dirsteps = dir->maxval + 1;
    double dirfac = M_PI / dirsteps;

    // Filter-Bank DoB
    vector<LSIFilter> fdob;

    for (int i = 0; i < dirsteps; ++i)
      {
        double fi = i * dirfac + 0.5 * M_PI;
        fdob.push_back(mkOrientedDoBFilter(filter_size, fi, filter_length, filter_width));
      }

#ifdef OPENMP
    #pragma omp parallel for schedule(dynamic,20)
#endif

    for (int y = 0; y < dimy; y++)
      for (int x = 0; x < dimx; x++)
        {
          int richtungs_index = GetVal(dir, x, y);
          int g = GetVal(pic, x, y, fdob[richtungs_index]) + dest->maxval / 2;
          PutVal(dest, x, y, limited(g, dest));
        }

    return 0;
  }

  int OrientedDoBImg(const ColorImage& src,
                     const Image& dir,
                     const ColorImage& dest,
                     int filter_size, int filter_length, int filter_width)
  {
    ReturnErrorIfFailed(src.match(src, dest));
    ReturnErrorIfFailed(OrientedDoBImg(src.redImage(), dir, dest.redImage(), filter_size, filter_length, filter_width));
    ReturnErrorIfFailed(OrientedDoBImg(src.greenImage(), dir, dest.greenImage(), filter_size, filter_length, filter_width));
    ReturnErrorIfFailed(OrientedDoBImg(src.blueImage(), dir, dest.blueImage(), filter_size, filter_length, filter_width));
    return OK;
  }

#undef FNAME

}

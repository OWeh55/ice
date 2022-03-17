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
#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <limits.h>

#ifndef WIN32
#include <stdlib.h>
#endif

#include "defs.h"
#include "IceException.h"
#include "macro.h"
#include "WindowWalker.h"
#include "histogram.h"
#include "histogramequalization.h"

using namespace std;

namespace ice
{
  /**********************************************************************

    Histogrammausgleich

  ***********************************************************************/

#define FNAME "HistogramEqualLUT"
  int HistogramEqualizationLUT(const Histogram& h, std::vector<int>& lut,
                               int maxv,
                               double reg)
  {
    // creates a lookup table for the histogram equalization of img
    // the value at index i in lut is the result value that a pixel
    // with value i in source has to get

    int lutSize = h.nClasses();

    if (maxv < 0)   // default
      {
        maxv = lutSize - 1;
      }

    lut.resize(lutSize);

    vector<double> rel(lutSize);
    for (int i = 0; i < lutSize; ++i)
      {
        rel[i] = h.getRelative(i);
      }

    // Cumulative distribution function
    vector<double> cdf(lutSize);

    //    cout << "lutSize: " << lutSize << endl;
    if (reg != 0.0)   // regularisation == smooth histogram
      {
        double sigma = reg * lutSize / 2;
        // the histogram will be extended
        // ext = extension on both sides
        int ext = RoundInt(sigma * 1.5); // filter width
        // cout << "Ext: " << ext <<endl;
        vector<double> filteredRel(rel.size() + ext * 2, 0.0);
        for (int i = 0; i < lutSize; ++i)
          {
            //      cout << rel[i] << " " ;
            double val = rel[i];
            for (int k = -ext; k <= ext; ++k)
              {
                filteredRel[i + k + ext] += val * gaussian(k, 0.0, sigma);
              }
          }

        //cout << endl << "........................................." << endl;
        // for (int i=0;i<filteredRel.size();++i)
        //  cout << filteredRel[i] << " " ;
        // cout << endl << "........................................." << endl;

        // calculate Cumulative distribution function
        double sum = 0.0;
        for (int k = 0; k < ext; ++k)
          {
            sum += filteredRel[k];
          }
        //   cout << "Summe: " << sum << endl;
        for (int i = 0; i < lutSize; ++i)
          {
            cdf[i] = sum;
            sum += filteredRel[i + ext];
          }
        // cout << "Summe: " << sum << endl;
        for (int k = 0; k < ext; ++k)
          {
            sum += filteredRel[lutSize + ext + k];
          }

        // cout << "Summe: " << sum << endl;
        // sum should be one in case of ideal filtering,
        // we correct it
        if (sum != 1.0 && sum != 0.0)
          {
            for (int i = 0; i < lutSize; ++i)
              {
                cdf[i] = cdf[i] / sum;
              }
          }
      }
    else
      {
        double sum = 0.0;
        for (int i = 0; i < lutSize; ++i)
          {
            cdf[i] = sum;
            sum += rel[i];
          }
      }

    cdf.push_back(1.0);

    // for (int i=0;i<lutSize+1;++i)
    //   cout << cdf[i] << " " ;
    // cout << endl;

    for (int i = 0; i < lutSize; i++)
      {
        // cout << h[i] << " " << cdf[i] << " " << cdf[i+1] << endl;
        // get "average" value for grayvalue
        lut[i] = RoundInt((cdf[i] + cdf[i + 1]) / 2 * maxv);
      }
    // cout << endl;
    return OK;
  }

  int HistogramEqualizationLUT(const Image& img, std::vector<int>& lut,
                               int maxv,
                               double reg)
  {
    // creates a lookup table for the histogram equalization of img
    // the value at index i in lut is the result value that an pixel with value i in source
    // has to get

    if (!IsImg(img))
      throw IceException(FNAME, M_WRONG_IMAGE);

    Histogram h(img);
    return HistogramEqualizationLUT(h, lut, maxv, reg);
  }

#undef FNAME

#define FNAME "HistogramEqual"
  int HistogramEqualization(const Image& img, const Image& imgd, double reg)
  {
    int dx, dy;
    RETURN_ERROR_IF_FAILED(MatchImg(img, imgd, dx, dy));

    vector<int> histx;
    HistogramEqualizationLUT(img, histx, imgd.maxval, reg);

    WindowWalker w(imgd);
    for (w.init(); !w.ready(); w.next())
      {
        int oldValue = img.getPixel(w);
        imgd.setPixel(w, histx[oldValue]);
      }
    return OK;
  }

  int HistogramEqualization(const Image& imgd, double reg)
  {
    return HistogramEqualization(imgd, imgd, reg);
  }

#undef FNAME
}

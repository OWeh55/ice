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
#ifndef _MAX_SEARCH_STRUCT_
#define _MAX_SEARCH_STRUCT_

#include "IceException.h"
#include "defs.h"

#include "base.h"

namespace ice
{
  typedef struct HIST_
  {

    HIST_* next_area;
    HIST_* prev_area;
    int    area_num;
    int    hfkt;
  } HIST;

  typedef HIST* LPHIST;

  typedef struct MaxSearch_
  {

    int      grwanz, areaanz, lastgrw, amin, amax;
    LPHIST*  hists;
    LPHIST   allhist;
  }* MaxSearch;

  MaxSearch NewMaxSearch(Image img);

  void FreeMaxSearch(MaxSearch);

#define RemovePointFromMaxSearch(ms,x,g) if ((--((ms)->hists[x][g].hfkt))==0) { HIST *prev,*next=(ms)->hists[x][g].next_area; if ((prev=(ms)->hists[x][g].prev_area)!=NULL) prev->next_area=next; if (next!=NULL) next->prev_area=prev; }

  int GetMaximum(MaxSearch ms, Image img, Image imgo, int& x, int& y, int& grw);

// Andere Maximasuche (liefert Markierungsbild)

  Image GetAllMaxima(Image);
}
#endif

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
// Schnelle Maximumsuche - 1998 Torsten Baumbach

#include "MaxSrch.h"

#define FNAME "NewMaxSearch"

namespace ice
{

  MaxSearch NewMaxSearch(Image img)
  {
    if (!IsImg(img))
      throw IceException(FNAME, M_NO_MEM);

    // Struktur initialisieren

    MaxSearch ms = new MaxSearch_;

    if (ms == nullptr)
      throw IceException(FNAME, M_NO_MEM);

    ms->grwanz = img.maxval + 1;
    ms->amin = ms->areaanz = img->xsize;
    ms->lastgrw = img.maxval;
    ms->amax = 0;

    int a, aa, x, y, g;

    ms->hists = new LPHIST[ms->areaanz];

    if (ms->hists == nullptr)
      {
        throw IceException(FNAME, M_NO_MEM);
        delete ms;
        return nullptr;
      }

    for (a = 0; a < ms->areaanz; a++)
      {
        ms->hists[a] = new HIST[ms->grwanz];

        if (ms->hists[a] == nullptr)
          {
            throw IceException(FNAME, M_NO_MEM);

            for (aa = 0; aa < a; aa++)
              {
                delete [](ms->hists[aa]);
              }

            delete [](ms->hists);
            delete ms;
            return nullptr;
          }

        for (g = 0; g < ms->grwanz; g++)
          {
            ms->hists[a][g].hfkt = 0;
            ms->hists[a][g].next_area = ms->hists[a][g].prev_area = nullptr;
            ms->hists[a][g].area_num = a;
          }
      }

    ms->allhist = (LPHIST)new HIST[ms->grwanz];

    if (ms->allhist == nullptr)
      {
        throw IceException(FNAME, M_NO_MEM);

        for (aa = 0; aa < ms->areaanz; aa++)
          {
            delete [](ms->hists[aa]);
          }

        delete [](ms->hists);
        delete ms;
        return nullptr;
      }

    for (g = 0; g < ms->grwanz; g++)
      {
        ms->allhist[g].next_area = ms->allhist[g].prev_area = nullptr;
      }

    // Struktur mit Daten aus Bild füllen

    // Spalten-Grauwerthistogramme aufbauen

    for (x = 0; x < ms->areaanz; x++)
      {
        for (y = 0; y < img->ysize; y++)
          {
            int g = GetVal(img, x, y);
            ms->hists[x][g].hfkt++;
          }
      }

    // Histogramme verketten

    for (g = 0; g < ms->grwanz; g++)
      {
        LPHIST prev = &(ms->allhist[g]);

        for (x = 0; x < ms->areaanz; x++)
          {
            if (ms->hists[x][g].hfkt > 0)   // in der Spalte x kommt der Grauwert g vor
              {
                ms->hists[x][g].prev_area = prev;
                prev->next_area = &(ms->hists[x][g]);
                prev = &(ms->hists[x][g]);
              }
          }
      }

    return ms;
  }

#undef FNAME

#define FNAME "FreeMaxSearch"

  void FreeMaxSearch(MaxSearch ms)
  {
    if (ms == nullptr)
      throw IceException(FNAME, M_WRONG_PTR);

    delete [](ms->allhist);

    for (int a = 0; a < ms->areaanz; a++)
      {
        delete [](ms->hists[a]);
      }

    delete [](ms->hists);
    delete ms;
  }

#undef FNAME

#define FNAME "GetMaximum"
  int GetMaximum(MaxSearch ms, Image img, Image imgo, int& x, int& y, int& grw)
  {

    if (ms == nullptr || !IsImg(img) || !IsImg(imgo))
      throw IceException(FNAME, M_WRONG_PARAMETER);

    while (ms->allhist[ms->lastgrw].next_area == nullptr)
      {
        ms->lastgrw--;

        if (ms->lastgrw < 0)
          {
            return false;
          }
      }

    x = ms->allhist[(grw = ms->lastgrw)].next_area->area_num;

    for (y = 0; y < img->ysize; y++)
      {
        if (GetVal(img, x, y) == grw &&
            (!IsImg(imgo) || GetVal(imgo, x, y) == 0))
          {
            return true;
          }
      }

//!!! Fehler, dieser punkt sollte wohl nicht erreicht werden, wird aber
// erreicht. Fehlermeldung deaktiviert, Rückgabe false:nicht gefunden
//!!    Message(FNAME,M_INTERN,ERROR);
    return false;
  }

#undef FNAME
}

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
#include <stdlib.h>

#include "defs.h"
#include "IceException.h"
#include "Contur.h"
#include "contlist.h"

namespace ice
{

  ConturList::ConturList() : number(conturs)
  {
    conturs = 0;
    datalen = CONTURLISTBLOCKSIZE;
    data = (Contur**)malloc(datalen * sizeof(Contur*));
  }

  ConturList::ConturList(const ConturList& cl): number(conturs)
  {
    int i;
    data = (Contur**)malloc(cl.datalen * sizeof(Contur*));
    datalen = cl.datalen;
    conturs = cl.conturs;

    for (i = 0; i < conturs; i++)
      {
        data[i] = new Contur(*(cl.data[i]));
      }
  }

  ConturList& ConturList::operator=(const ConturList& cl)
  {
    int i;

    for (i = 0; i < conturs; i++)
      {
        delete data[i];
      }

    free(data);
    data = (Contur**)malloc(cl.datalen * sizeof(Contur*));
    datalen = cl.datalen;
    conturs = cl.conturs;
    {
      for (i = 0; i < conturs; i++)
        {
          data[i] = new Contur(*(cl.data[i]));
        }
    }
    return *this;
  }

#define FNAME "ConturList::Add"
  int ConturList::add(const Contur& c)
  {
    Contur** hdata;

    if (conturs >= datalen)
      {
        hdata = (Contur**)realloc(data, (datalen + CONTURLISTBLOCKSIZE) * sizeof(Contur*));

        if (hdata == NULL)
          throw IceException(FNAME, M_NO_MEM);

        datalen += CONTURLISTBLOCKSIZE;
        data = hdata;
      }

    data[conturs] = new Contur(c);
    conturs++;
    return OK;
  }
#undef FNAME
#define FNAME "ConturList::Sub"
  int ConturList::sub(int i)
  {
    if ((i < 0) || (i >= conturs))
      throw IceException(FNAME, M_WRONG_INDEX);

    delete data[i];

    for (i = i + 1; i < conturs; i++)
      {
        data[i - 1] = data[i];
      }

    conturs--;
    return OK;
  }
#undef FNAME
#define FNAME "ConturList::getContur"
  Contur* ConturList::getContur(int i)
  {
    if (i == conturs)
      {
        return NULL;  // No more conturs, but no Error
      }

    if ((i < 0) || (i > conturs))
      throw IceException(FNAME, M_WRONG_INDEX);

    return data[i];
  }
#undef FNAME
  ConturList::~ConturList()
  {
    int i;

    for (i = 0; i < conturs; i++)
      {
        delete data[i];
      }

    free(data);
  }
}

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
  int ConturList::Add(const Contur& c)
  {
    Contur** hdata;

    if (conturs >= datalen)
      {
        hdata = (Contur**)realloc(data, (datalen + CONTURLISTBLOCKSIZE) * sizeof(Contur*));

        if (hdata == NULL)
          {
            throw IceException(FNAME, M_NO_MEM, NO_MEM);
          }

        datalen += CONTURLISTBLOCKSIZE;
        data = hdata;
      }

    data[conturs] = new Contur(c);
    conturs++;
    return OK;
  }
#undef FNAME
#define FNAME "ConturList::Sub"
  int ConturList::Sub(int i)
  {
    if ((i < 0) || (i >= conturs))
      {
        throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
      }

    delete data[i];

    for (i = i + 1; i < conturs; i++)
      {
        data[i - 1] = data[i];
      }

    conturs--;
    return OK;
  }
#undef FNAME
#define FNAME "ConturList::Contur"
  Contur* ConturList::GetContur(int i)
  {
    if (i == conturs)
      {
        return NULL;  // No more conturs, but no Error
      }

    if ((i < 0) || (i > conturs))
      {
        throw IceException(FNAME, M_WRONG_INDEX, WRONG_PARAM);
      }

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

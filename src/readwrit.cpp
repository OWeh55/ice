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
#include <stdio.h>

#include "IceException.h"
#include "defs.h"
#include "macro.h"

#include "readwrit.h"

using namespace std;

namespace ice
{
  int WriteArrayD(double* ptrd, int number, char* name)
  {
    FILE* fp;
    int i;

    if ((fp = fopen(name, "w")) == NULL)
      {
        throw IceException("WriteArrayD", "Cannot open file");
      }

    fprintf(fp, "%d\n", number);

    for (i = 0; i < number; i++)
      {
        fprintf(fp, "%1.20e\n", *ptrd++);
      }

    fclose(fp);
    return (OK);
  }

  int ReadArrayD(char* name, int number, double* ptrd)
  {
    int nbr;
    int i;
    FILE* fp;

    if ((fp = fopen(name, "r")) == NULL)
      {
        throw IceException("ReadArrayD", "File nicht gefunden");
      }

    if (fscanf(fp, "%d\n", &nbr) == 0)
      {
        printf("scanf fehlerhaft\n");
      }

    for (i = 0; i < min<int>(number, nbr); i++)
      {
        if (fscanf(fp, "%le\n", ptrd++) == 0)
          {
            printf("scanf fehlerhaft\n");
          }
      }

    fclose(fp);
    return (OK);
  }
}

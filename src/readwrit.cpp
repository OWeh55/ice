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
#include <stdio.h>

#include "message.h"
#include "defs.h"
#include "macro.h"

#include "readwrit.h"

namespace ice
{

  int WriteArrayD(double* ptrd, int number, char* name)
  {
    FILE* fp;
    int i;

    if ((fp = fopen(name, "w")) == NULL)
      {
        Message("WriteArrayD", "Kann File nicht Oeffnen", WRONG_FILE);
        return (ERROR);
      }

    fprintf(fp, "%d\n", number);

    for (i = 0; i < number; i++)
      fprintf(fp, "%1.20e\n", *ptrd++);

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
        Message("ReadArrayD", "File nicht gefunden", FILE_NOT_FOUND);
        return (FILE_NOT_FOUND);
      }

    if (fscanf(fp, "%d\n", &nbr) == 0)
      printf("scanf fehlerhaft\n");

    for (i = 0; i < min<int>(number, nbr); i++)
      {
        if (fscanf(fp, "%le\n", ptrd++) == 0)
          printf("scanf fehlerhaft\n");
      }

    fclose(fp);
    return (OK);
  }
}

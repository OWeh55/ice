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
/********************************************************************
  header zu "matrix.c"
  funktionen zur matrixrechnung
  abraham 11/95

  Nicht mehr verwenden!
  Klasse matrix als Ersatz!
********************************************************************/

#ifndef _MATDEF_H
#define _MATDEF_H
namespace ice
{
  typedef struct MatrixStruct_
  {
    int type;
    int rsize, csize;
    double** data;
    int** datai;
    unsigned char** datac;

    struct MatrixStruct_* prev, * next;
  }*  MatrixStruct;

  /* Verwaltung der Matrizen */
  MatrixStruct NewMatrix(int type, int xsize, int ysize);
  int FreeMatrix(MatrixStruct mat);
  int IsMatrix(MatrixStruct mat);
  MatrixStruct MoveMat(MatrixStruct m1, MatrixStruct m2);
  MatrixStruct TranspMat(MatrixStruct m1, MatrixStruct m2);
  MatrixStruct MulMat(MatrixStruct m1, MatrixStruct m2, MatrixStruct m3);
  int MulMatVec(MatrixStruct A, double* b, double* x);

#define MAT_DOUBLE 0
#define MAT_INT 1
#define MAT_CHAR 2
}
#endif

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
  verwendet double arrays als Matrix. Nicht mehr verwenden!
  Neuer Typ Matrix
  neubauer 10/95
********************************************************************/

#ifndef _MATRIX_H
#define _MATRIX_H

namespace ice
{

  /*** Matrixalgebra ************************************************/
  double* DefMatrix(double* m, int row, int col, int option);

  double* SetMatrix(double* m, int row, int col, double val);

  double* MoveMatrix(double* m1, int row, int col, double* m2);

  double* AddMatrix(double* m1, double* m2, int row, int col, double* m3);

  double* SubMatrix(double* m1, double* m2, int row, int col, double* m3);

  double* TranspMatrix(double* m1, int row, int col, double* m2);

  double* ScaleMatrix(double* m1, int row, int col, double fac, double* m2);

  double* MulMatrix(double* m1, double* m2, int row1, int col1, int col2, double* m3);

  double* InvertMatrix(double* m1, int dim, double* m2);

  int NormMatrix(double* a, int row, int col, int mode, double* norm);

  int DetMatrix(double* a, int row, int col, double* det, int* rank);

  double* ChangeMatrixRow(double* m, int row1, int row2, int col);

  double* ChangeMatrixCol(double* m, int col1, int col2, int row, int col);

  double* OrthoMatrix(double* m1, int row, int col, double* m2);

  int GenOrthoSystem(double n[3], double m[3][3]);

  int IsMatrixZero(double* a, int row, int col, double* eps);

  int IsMatrixUnit(double* a, int dim, double* eps);
  int IsMatrixOrtho(double* a, int dim, double* eps);
  int IsMatrixDiag(double* a, int dim, double* eps);
  int IsMatrixSymm(double* a, int dim, double* eps);
  int IsMatrixAnti(double* a, int dim, double* eps);
  int IsMatrixRegular(double* a, int dim, double* eps);
  int IsMatrixPosDef(double* a, int dim);
}
#endif

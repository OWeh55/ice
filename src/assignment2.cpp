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
// **************************************************************************
// Lösung des Zuordnungsproblems mit timewarping
// Gegeben m Quellenpunkte und n Zielpunkte
// sowie eine Kostenmatrix, die  Kosten enthält.
// *************************************************************************

#include <limits.h>
#include <stdio.h>
#include <malloc.h>

#include "macro.h"
#include "defs.h"
#include "IceException.h"
#include "numbase.h"
#include "Matrix.h"
#include "assignment.h"

namespace ice
{

#define FNAME "TimeWarp"
  static int MyMod(int a, int b)
  {
    // special (optimized) modulo
    // works only, if b>0
    // and a  within [-b..2b)
    if (a < 0)
      {
        return a + b;
      }

    if (a >= b)
      {
        return a - b;
      }

    return a;
  }

  typedef double* dptr;
  typedef signed char** dirptr;

  static int AllocAll(dptr& min1, dptr& min2, dirptr& dir, int nx, int ny)
  {
    int x, i;

    min1 = new double[ny];
    min2 = new double[ny];

    if ((min1 == NULL) || (min2 == NULL))
      {
        delete [] min1;
        delete [] min2;
        return ERROR;
      }

    dir = new signed char* [nx];

    if (dir == NULL)
      {
        delete [] min1;
        delete [] min2;
        return ERROR;
      }

    for (x = 0; x < nx; x++)
      {
        dir[x] = new signed char[ny];

        if (dir[x] == NULL)
          {
            for (i = 0; i < x; i++)
              {
                delete [] dir[i];
              }

            delete [] dir;
            delete [] min1;
            delete [] min2;
            return ERROR;
          }
      }

    return OK;
  }

  static int CheapestWayNWSO(const Matrix& abstand, int ystart, IMatrix& ref)
  {
    int ny = abstand.cols();
    int nx = abstand.rows();
    int d;
    int ct;
    //  double ret;

    dptr min1;
    dptr min2;
    dirptr dir;

    if (AllocAll(min1, min2, dir, nx, ny) != OK)
      throw IceException(FNAME, M_NO_MEM);

    int x, x1;
    int y, y1, y2;
    double v1, v2, v3;

    min1[ystart] = abstand[0][ystart];
    dir[0][ystart] = 0;

    for (y = 1; y < ny; y++)   // first column: only one direction possible
      {
        y2 = MyMod(y + ystart, ny);
        y1 = MyMod(y2 - 1, ny);
        min1[y2] = min1[y1] + abstand[0][y1] + abstand[0][y2];
        dir[0][y2] = 2;
      }

    for (x = 1; x < nx; x++)   // all columns
      {
        x1 = x - 1;
        // only one direction for first row
        y1 = ystart;
        y2 = MyMod(y1 + 1, ny);
        min2[y1] = min1[y1] + abstand[x1][y1] + abstand[x][y1];
        dir[x][y1] = 0;

        for (y = 1; y < ny; y++)   // all other rows
          {
            // y1, y2 already set
            v1 = min1[y1] + abstand[x1][y1]; // upper left
            v2 = min1[y2] + abstand[x1][y2]; // left
            v3 = min2[y1] + abstand[x][y1]; // upper

            if (v1 < v2)
              {
                if (v1 < v3)
                  {
                    min2[y2] = v1 + abstand[x][y2];
                    dir[x][y2] = 1;
                  }
                else
                  {
                    min2[y2] = v3 + abstand[x][y2];
                    dir[x][y2] = 2;
                  }
              }
            else
              {
                // v2<v1
                if (v2 < v3)
                  {
                    min2[y2] = v2 + abstand[x][y2];
                    dir[x][y2] = 0;
                  }
                else
                  {
                    min2[y2] = v3 + abstand[x][y2];
                    dir[x][y2] = 2;
                  }
              }

            min2[y2] += 0.01 * fabs((double)y - (double)x * double(ny) / double(nx));
            y1 = y2;
            y2 = MyMod(y2 + 1, ny);
          }

        for (ct = 0; ct < ny; ct++)
          {
            min1[ct] = min2[ct];
          }
      }

    x1 = nx - 1;
    y1 = MyMod(ystart - 1, ny);

    ct = 1;

    while ((x1 != 0) || (y1 != ystart))
      {
        d = dir[x1][y1]; // "direction" to go

        switch (d)
          {
          case 2:
            y1 = MyMod(y1 - 1, ny);
            break;
          case 1:
            y1 = MyMod(y1 - 1, ny);
            x1--;
            break;
          case 0:
            x1--;
            break;
          }

        ct++;
      }

    ref = IMatrix(ct, 2);

    x1 = nx - 1;
    y1 = MyMod(ystart - 1, ny);

    ct = 0;
    ref[0][0] = x1;
    ref[0][1] = y1;
    ct++;

    while ((x1 != 0) || (y1 != ystart))
      {
        d = dir[x1][y1]; // "direction" to go

        switch (d)
          {
          case 2:
            y1 = MyMod(y1 - 1, ny);
            break;
          case 1:
            y1 = MyMod(y1 - 1, ny);
            x1--;
            break;
          case 0:
            x1--;
            break;
          }

        ref[ct][0] = x1;
        ref[ct][1] = y1;
        ct++;
      }

    delete [] min1;
    delete [] min2;

    for (x = 0; x < nx; x++)
      {
        delete [] dir[x];
      }

    delete [] dir;
    return OK;
  }

  static int CheapestWaySWNO(const Matrix& abstand, int ystart, IMatrix& ref)
  {
    // find cheapest way
    // abstand.cols() muß größer als abstand.rows() sein

    int ny = abstand.cols();
    int nx = abstand.rows();
    int d;
    int ct;

    double* min1;
    double* min2;
    signed char** dir;

    if (AllocAll(min1, min2, dir, nx, ny) != OK)
      throw IceException(FNAME, M_NO_MEM);

    int x, x1;
    int y, y1, y2;
    double v1, v2, v3;

    min1[ystart] = abstand[0][ystart];
    dir[0][ystart] = 0;

    for (y = 1; y < ny; y++)   // first column: only one direction possible
      {
        y2 = MyMod(ystart - y, ny);
        y1 = MyMod(y2 + 1, ny);
        min1[y2] = min1[y1] + abstand[0][y1] + abstand[0][y2];
        dir[0][y2] = 6;
      }

    for (x = 1; x < nx; x++)   // all columns
      {
        x1 = x - 1;
        // only one direction for first row
        y1 = ystart;
        y2 = MyMod(y1 - 1, ny);
        min2[y1] = min1[y1] + abstand[x1][y1] + abstand[x][y1];
        dir[x][y1] = 0;

        for (y = 1; y < ny; y++)   // all other rows
          {
            // y1, y2 already set
            v1 = min1[y1] + abstand[x1][y1]; // lower left
            v2 = min1[y2] + abstand[x1][y2]; // left
            v3 = min2[y1] + abstand[x][y1]; // lower

            if (v1 < v2)
              {
                if (v1 < v3)
                  {
                    min2[y2] = v1 + abstand[x][y2];
                    dir[x][y2] = 7;
                  }
                else
                  {
                    min2[y2] = v3 + abstand[x][y2];
                    dir[x][y2] = 6;
                  }
              }
            else
              {
                // v2<v1
                if (v2 < v3)
                  {
                    min2[y2] = v2 + abstand[x][y2];
                    dir[x][y2] = 0;
                  }
                else
                  {
                    min2[y2] = v3 + abstand[x][y2];
                    dir[x][y2] = 6;
                  }
              }

            min2[y2] += 0.01 * fabs((double)y / (double)x - double(ny) / double(nx));
            y1 = y2;
            y2 = MyMod(y2 - 1, ny);
          }

        for (ct = 0; ct < ny; ct++)
          {
            min1[ct] = min2[ct];
          }
      }

    x1 = nx - 1;
    y1 = MyMod(ystart + 1, ny);

    ct = 1;

    while ((x1 != 0) || (y1 != ystart))
      {
        d = dir[x1][y1]; // "direction" to go

        switch (d)
          {
          case 0:
            x1--;
            break;
          case 7:
            y1 = MyMod(y1 + 1, ny);
            x1--;
            break;
          case 6:
            y1 = MyMod(y1 + 1, ny);
            break;
          }

        ct++;
      }

    ref = IMatrix(ct, 2);

    x1 = nx - 1;
    y1 = MyMod(ystart + 1, ny);

    ct = 0;
    ref[0][0] = x1;
    ref[0][1] = y1;
    ct++;

    while ((x1 != 0) || (y1 != ystart))
      {
        d = dir[x1][y1]; // "direction" to go

        switch (d)
          {
          case 0:
            x1--;
            break;
          case 7:
            y1 = MyMod(y1 + 1, ny);
            x1--;
            break;
          case 6:
            y1 = MyMod(y1 + 1, ny);
            break;
          }

        ref[ct][0] = x1;
        ref[ct][1] = y1;
        ct++;
      }

    delete [] min1;
    delete [] min2;

    for (x = 0; x < nx; x++)
      {
        delete [] dir[x];
      }

    delete [] dir;

    return OK;
  }

  static int CheapestWayUpDown(const Matrix& abstand, int maindir, IMatrix& ref)
  {
    // find cheapest way
    // vereinfachter Algorithmus, der einen Weg von oben (erste Zeile)
    // nach unten (letzte Zeile) findet
    // Eine Wegsuchrichtung wird mit maindir festgelegt:
    // maindir = -1 nach links oder gerade
    // maindir =  1 nach rechts oder gerade
    // Die Funktion arbeitet zyklisch bezüglich des Spaltenindex
    //  BEGIN(3,"CheapestWay");
    int ny = abstand.cols();
    int nx = abstand.rows();

    if (nx < ny)
      {
        // abstand.cols() muß kleiner als abstand.rows() sein
        // wenn nicht erfüllt: transponieren
        int ret = CheapestWayUpDown(!abstand, maindir, ref);

        if (ret == OK)
          {
            for (int i = 0; i < ref.rows(); i++)
              {
                std::swap(ref[i][0], ref[i][1]);
              }
          }

        return ret;
      }

    int minarray;

    signed char** dir;
    double* min1;
    double* min2;

    if (AllocAll(min1, min2, dir, nx, ny) != OK)
      throw IceException(FNAME, M_NO_MEM);

    for (int y = 0; y < ny; y++)   // first row, all columns
      {
        min1[y] = abstand[0][y];
      }

    minarray = 1;

    int dy1 = 1;
    int dy2 = 2;

    if (maindir > 0)
      {
        dy1 = -1;
        dy2 = -2;
      }

    for (int x = 1; x < nx; x++)   // all columns
      {
        // erster Schritt min1 -> min2
        for (int y = 0; y < ny; y++)   // all rows
          {
            // erster Versuch: 45°
            int y1 = MyMod(y + dy1, ny);
            min2[y] = min1[y1];
            int dy = dy1;

            // zweiter Versuch: senkrecht
            if (min1[y] < min2[y])
              {
                min2[y] = min1[y];
                dy = 0;
              }

            // dritter Versuch: Schrittweite 2
            int y2 = MyMod(y + dy2, ny);
            if (min1[y2] < min2[y])
              {

                min2[y] = min1[y2];
                dy = dy2;
              }

            min2[y] += abstand.at(x).at(y); // addition aktueller Kosten

            dir[x][y] = dy; // Richtung merken
          }

        minarray = 2;
        x++;
        // zweiter Schritt min2 -> min1
        if (x < nx)
          {
            for (int y = 0; y < ny; y++)   // all rows
              {
                // erster Versuch: 45°
                int y1 = MyMod(y + dy1, ny);
                min1[y] = min2[y1];
                int dy = dy1;

                // zweiter Versuch: senkrecht
                if (min2[y] < min1[y])
                  {
                    min1[y] = min2[y];
                    dy = 0;
                  }

                // dritter Versuch: Schrittweite 2
                int y2 = MyMod(y + dy2, ny);
                if (min2[y2] < min1[y])
                  {
                    min1[y] = min2[y2];
                    dy = dy2;
                  }

                min1[y] += abstand.at(x).at(y);

                dir[x][y] = dy;
              }

            minarray = 1;
          }
      }

    if (minarray == 1)
      {
        for (int i = 0; i < ny; i++)
          {
            min2[i] = min1[i];
          }
      }

    double minimum = min2[0];
    int miny = 0;

    for (int y = 1; y < ny; y++)
      {
        if (min2[y] < minimum)
          {
            minimum = min2[y];
            miny = y;
          }
      }

    ref = IMatrix(nx, 2);

    for (int x = nx - 1; x >= 0; x--)
      {
        ref[x][0] = x;
        ref[x][1] = miny;
        int dy = dir[x][miny]; // "direction" to go
        miny = MyMod(miny + dy, ny); // -> next point (x-1,miny)
      }

    delete [] min1;
    delete [] min2;

    for (int x = 0; x < nx; x++)
      {
        delete [] dir[x];
      }

    delete [] dir;

    return OK;
  }

  static int TimeWarpNormal(const Matrix& distance,
                            IMatrix& ref, int mode)
  {
    double cost1, costm;
    RETURN_ERROR_IF_FAILED(CheapestWayNWSO(distance, 0, ref));
    costm = ReferenceCosts(distance, ref);

    if ((mode & TW_SEARCHSTART) > 0)
      {
        int ny = distance.cols();

        for (int y = 1; y < ny; y++)
          {
            IMatrix ref2;
            RETURN_ERROR_IF_FAILED(CheapestWayNWSO(distance, y, ref2));
            cost1 = ReferenceCosts(distance, ref2);

            if (cost1 < costm)
              {
                costm = cost1;
                ref = ref2;
              }
          }
      }

    if ((mode & TW_BIDIRECTIONAL) > 0)
      {
        // auch entgegengesetzte Diasgonalenrichtung untersuchen
        int ny = distance.cols();
        IMatrix ref2;
        RETURN_ERROR_IF_FAILED(CheapestWaySWNO(distance, ny - 1, ref2));
        cost1 = ReferenceCosts(distance, ref2);

        if (cost1 < costm)
          {
            costm = cost1;
            ref = ref2;
          }

        if ((mode & TW_SEARCHSTART) > 0)
          {
            for (int y = 0; y < ny - 1; y++)
              {
                RETURN_ERROR_IF_FAILED(CheapestWaySWNO(distance, y, ref2));
                cost1 = ReferenceCosts(distance, ref2);

                if (cost1 < costm)
                  {
                    costm = cost1;
                    ref = ref2;
                  }
              }
          }
      }

    return OK;
  }

  static int TimeWarpReduced(const Matrix& distance,
                             IMatrix& ref, int mode)
  {
    IMatrix ref2;
    double cost1, cost2;
    RETURN_ERROR_IF_FAILED(CheapestWayUpDown(distance, 1, ref));

    if ((mode & TW_BIDIRECTIONAL) > 0)
      {
        cost1 = ReferenceCosts(distance, ref);
        RETURN_ERROR_IF_FAILED(CheapestWayUpDown(distance, -1, ref2));
        cost2 = ReferenceCosts(distance, ref2);

        if (cost2 < cost1)
          {
            ref = ref2;
          }
      }

    return OK;
  }

  int TimeWarp(const Matrix& distance,
               IMatrix& ref, int mode)
  {
    switch (mode & TW_MODEMASK)
      {
      case TW_NORMAL:
        RETURN_ERROR_IF_FAILED(TimeWarpNormal(distance, ref, mode));
        break;
      case TW_REDUCED:
        RETURN_ERROR_IF_FAILED(TimeWarpReduced(distance, ref, mode));
        break;
      default:
        throw IceException(FNAME, M_WRONG_PARAM);
      }

    return OK;
  }
#undef FNAME
}

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
/*******************************************************************
 numbase.c - Numerische Grundfunktionen:
*******************************************************************/

#include <math.h>
#include <stdlib.h>
#include <ctime>

#include "IceException.h"
#include "defs.h"

#include "dtime.h"

#include "numbase.h"

namespace ice
{
  /*
   * Mod - "echte" Modulo-Rechnung, Resultat immer positiv
   */
#define FNAME "Mod"
  int Mod(int d, int div)
  {
    if (div == 0)
      throw IceException(FNAME, M_DIVISION_ZERO);

    div = abs(div);

    int res = d % div;

    if (res < 0)
      {
        res += div;
      }

    return res;
  }
#undef FNAME
  /*
   * FMod
   * Gleitkomma-Modulo-Rechnung, liefert stets positive Werte
   */
#define FNAME "FMod"
  double FMod(double v, double div)
  {
    if (div == 0.0)
      throw IceException(FNAME, M_DIVISION_ZERO);

    double adiv = fabs(div);
    double result = fmod(v, adiv);

    if (result < 0)
      {
        result += adiv;
      }

    return result;
  }
#undef FNAME
  /*******************************************************************/
  /*** Kubikwurzel ***/
  double CubRoot(double val)
  {
    if (val == 0.0)
      {
        return 0.0;
      }

    if (val < 0)
      {
        return -exp(log(-val) / 3.0);
      }
    else
      {
        return exp(log(val) / 3.0);
      }
  }

  /*******************************************************************/
  /*** Vorzeichen eines "int"-Wertes ***/
  int Sign(int val)
  {
    if (val < 0)
      {
        return -1;
      }
    if (val > 0)
      {
        return 1;
      }
    return 0;
  }

  /*** Vorzeichen eines "double"-Wertes ***/
  int Sign(double val)
  {
    if (val < 0.0)
      {
        return -1;
      }
    if (val > 0.0)
      {
        return 1;
      }
    return 0;
  }

  /*** double-Vorzeichen eines "double"-Wertes ***/
  double SignD(double val)
  {
    if (val < 0.0)
      {
        return -1.0;
      }
    if (val > 0.0)
      {
        return 1.0;
      }
    return 0.0;
  }

  /*******************************************************************/
  void Randomize()
  {
    srand(time(nullptr));
  }

  /*******************************************************************/
  /* "Int"-Zafallszahl zwischen 0 und "val" */
#define FNAME "Random"
  int Random(int val)
  {
    if (val > RAND_MAX)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    return rand() % (val + 1);
    //    return (int)((double)(val + 1) * rand() / RAND_MAX);
  }
#undef FNAME

  /*******************************************************************/
  /* double random number */
  double RandomD(double max)
  {
    return drand48() * max;
  }

  /*
   * Normalverteilten Zufallswert mit vorgegebenem
   * Mittelwert und Streuung
   *    (C) m.schubert
   * Gleichverteilung: sigma=(b-a)/sqrt(12)
   * fuer Summenverteilung gilt: sigmas=sqrt(N)*sigma
   * mit N=12,a=0 gilt dann b=sigmas
   */

  double GaussRandom(double sigma)
  {
    constexpr int n = 6;

    double val = 0;

    for (int i = 0; i < n * 2; i++)
      {
        val += RandomD(sigma);
      }

    val = (val - n * sigma);
    return val;
  }

  //=====================================================================
  double gaussian(double x, double my, double sigma)
  {
    double xr = x - my;
    return M_SQRT1_2 / sigma * exp(- (xr) * (xr) / (2 * sigma * sigma));
  }

  bool Solve2(double a1, double b1, double i1, // erste Gleichung
              double a2, double b2, double i2, // zweite Gleichung
              double& x1, double& x2)        // Lösungen
  {
    double det = a1 * b2 - a2 * b1;

    if (det == 0)
      {
        return false;
      }

    x1 = (i1 * b2 - i2 * b1) / det;
    x2 = (a1 * i2 - a2 * i1) / det;
    return true;
  }
  /*******************************************************************/
}

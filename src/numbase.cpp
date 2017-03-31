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
/*******************************************************************
 numbase.c - Numerische Grundfunktionen:
*******************************************************************/

#include <math.h>
#include <stdlib.h>
#include <ctime>

#include "message.h"
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
      {
        Message(FNAME, M_DIVISION_ZERO, WRONG_PARAM);
        return 0;
      }

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
      {
        Message(FNAME, M_DIVISION_ZERO, WRONG_PARAM);
        return 0;
      }

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
      {
        Message(FNAME, M_WRONG_VAL, WRONG_PARAM);
        return WRONG_PARAM;
      }

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
  double gauss(double my, double sigma, double x)
  {
    return M_SQRT1_2 / sigma * exp(- x * x / (2 * sigma * sigma)) + my;
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

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
//************************************************************
// Berechnung und Transformation von Momenten                *
// Berechnung von Invarianten                                *
//                                                           *
//************************************************************
//    M.Schubert 3/1995
//    W.Ortmann 10/1999, 9/2005, 1/2007
//
#include <math.h>
#include <stdlib.h>
#include <stddef.h>
#include <float.h>

#include "IceException.h"
#include "macro.h"
#include "root.h"
#include "gentrans.h"
#include "matrix_function.h"

#include "momente.h"

namespace ice
{
//****************************************
// Berechnung der Momente für ein Polygon
//****************************************

//#define POTENCES

  // const double x2=x*x;
  // const double x3=x2*x;
  // const double x4=x2*x2;
  // const double y2=y*y;
  // const double y3=y2*y;
  // const double y4=y2*y2;

#define FNAME "MomentPolygon"

  static void AddSegment(double m[15], double x1, double y1,
                         double x2, double y2)
  {
    double a = (x1 * y2 - x2 * y1) / 2;
    double x12 = x1 * x1;
    double x22 = x2 * x2;
    double y12 = y1 * y1;
    double y22 = y2 * y2;

    m[i00] += a;
    m[1] += (x1 + x2) * a;
    m[2] += (y1 + y2) * a;
    m[3] += (x1 * (x1 + x2) + x22) * a;
    m[4] += (y1 * (x1 + x1 + x2) + y2 * (x1 + x2 + x2)) * a;
    m[5] += (y1 * (y1 + y2) + y22) * a;
    m[6] += (x12 + x22) * (x1 + x2) * a;
    m[7] += (y1 * (x1 * (3 * x1 + 2 * x2) + x22) + y2 * (x12 + x2 * (2 * x1 + 3 * x2))) * a;
    m[8] += (x1 * (y1 * (3 * y1 + 2 * y2) + y22) + x2 * (y12 + y2 * (2 * y1 + 3 * y2))) * a;
    m[9] += (y12 + y22) * (y1 + y2) * a;
    m[10] += ((((x1 + x2) * x1 + x22) * x1 + x22 * x2) * x1 + x22 * x22) * a;
    m[11] += ((y1 + 4 * y2) * x22 * x2 + (2 * y1 + 3 * y2) * x1 * x22 + \
              (3 * y1 + 2 * y2) * x12 * x2 + (4 * y1 + y2) * x12 * x1) * a;
    m[12] += ((2 * y12 + 6 * y1 * y2 + 12 * y22) * x22 + (6 * (y12 + y22) + 8 * y1 * y2) * x1 * x2 + \
              (12 * y12 + 6 * y1 * y2 + 2 * y22) * x12) * a;
    m[13] += ((x1 + 4 * x2) * y22 * y2 + (2 * x1 + 3 * x2) * y1 * y22 + \
              (3 * x1 + 2 * x2) * y12 * y2 + (4 * x1 + x2) * y12 * y1) * a;
    m[14] += ((((y1 + y2) * y1 + y22) * y1 + y22 * y2) * y1 + y22 * y22) * a;
    // m[15]+=((((x1+x2)*x1+x22)*x1+x22*x2)*x1+x22*x22)*x1+x22*x22*x2;
    // m[16]+=(y1+5*y1)*x22*x22+(2*y1+4*y2)*x1*x2*x22+3*(y1+y2)*x12+x22+
    //  (4*y1+2*y2)*x12*x1*x2+(5*y1+y2)*x12*x12;
    // m[17]+=(2*y12+8*y1*y2+20*y22)*x22*x2+(6*y12+12*y2*(y1+y2))*x1*x22
    //  (12*y1(y1+y2)+6*y22)*x12*x2+(20*y12+8*y1*y2+2*y22)*x12*x1;
    // m[18]+=(2*x12+8*x1*x2+20*x22)*y22*y2+(6*x12+12*x2*(x1+x2))*y1*y22
    //  (12*x1(x1+x2)+6*x22)*y12*y2+(20*x12+8*x1*x2+2*x22)*y12*y1;
    // m[19]+=(x1+5*x1)*y22*y22+(2*x1+4*x2)*y1*y2*y22+3*(x1+x2)*y12+y22+
    //  (4*x1+2*x2)*y12*y1*y2+(5*x1+x2)*y12*y12;
    // m[20]+=((((y1+y2)*y1+y22)*y1+y22*y2)*y1+y22*y22)*y1+y22*y22*y2;
  }

  static void CorrectMoments(double m[15])
  {
    m[1]  /= 3;
    m[2] /= 3;
    m[3]  /= 6;
    m[4] /= 12;
    m[5] /= 6;
    m[6]  /= 10;
    m[7] /= 30;
    m[8] /= 30;
    m[9]  /= 10;
    m[10] /= 15;
    m[11] /= 60;
    m[12] /= 180;
    m[13] /= 60;
    m[14] /= 15;
    //  m[15]/=21; m[16]/=105; m[17]/=420; m[18]/=420; m[19]/=105; m[20]/=21;
  }

  int MomentPolygon(PointList pl, double m[15], double centre[2])
  {
    int points = pl->lng;

    for (int i = 0; i < 15; i++)
      {
        m[i] = 0;
      }

    for (int i = 0; i < pl->lng; i++)
      {
        int j = (i + 1) % points;

        double x1 = pl->xptr[i];
        double y1 = pl->yptr[i];
        double x2 = pl->xptr[j];
        double y2 = pl->yptr[j];

        AddSegment(m, x1, y1, x2, y2);
      }

    CorrectMoments(m);

    if (fabs(m[i00]) < EPSILON)
      {
        return ERROR;
      }

    centre[0] = m[1] / m[i00];
    centre[1] = m[2] / m[i00];
    return OK;
  }

  int MomentPolygon(const std::vector<Point>& pl,
                    double m[15], double centre[2])
  {
    int points = pl.size();

    for (int i = 0; i < 15; i++)
      {
        m[i] = 0;
      }

    for (int i = 0; i < points; i++)
      {
        int j = (i + 1) % points;
        AddSegment(m, pl[i].x, pl[i].y, pl[j].x, pl[j].y);
      }

    CorrectMoments(m);

    if (fabs(m[i00]) < EPSILON)
      {
        return ERROR;
      }

    centre[0] = m[1] / m[i00];
    centre[1] = m[2] / m[i00];
    return OK;
  }

  int MomentPolygon(const Matrix& pl, double m[15], double centre[2])
  {
    int points = pl.rows();

    for (int i = 0; i < 15; i++)
      {
        m[i] = 0;
      }

    for (int i = 0; i < pl.rows(); i++)
      {
        int j = (i + 1) % points;

        double x1 = pl[i][0];
        double y1 = pl[i][1];
        double x2 = pl[j][0];
        double y2 = pl[j][1];

        AddSegment(m, x1, y1, x2, y2);
      }

    CorrectMoments(m);

    if (fabs(m[i00]) < EPSILON)
      {
        return ERROR;
      }

    centre[0] = m[1] / m[i00];
    centre[1] = m[2] / m[i00];
    return OK;
  }
#undef FNAME
//**********************************************
// Berechnung der Momente für
// ein Polygon als Ausschnitt einer Punktliste
//**********************************************
#define FNAME "PointListMoment"
  int PointListMoment(PointList pl, int a1, int a2, double m[15], double centre[2])
  {
    int i, j, a3;
    double x1, y1, x2, y2;

    if (a1 < 0 || a2 < 0 || a1 >= pl->lng || a2 >= pl->lng)
      throw IceException(FNAME, M_WRONG_INDEX);

    for (i = 0; i < 15; i++)
      {
        m[i] = 0;
      }

    a3 = a2 + 1;

    i = a1;
    j = a1 + 1;

    do
      {
        if (i == pl->lng)
          {
            i = 0;
          }

        if (j == a3)
          {
            j = a1;
          }

        if (j == pl->lng)
          {
            j = 0;
          }

        x1 = pl->xptr[i];
        y1 = pl->yptr[i];
        x2 = pl->xptr[j];
        y2 = pl->yptr[j];
        AddSegment(m, x1, y1, x2, y2);
        i++;
        j++;
      }
    while (i != a3);

    CorrectMoments(m);

    if (fabs(m[i00]) < 1e-20)
      {
        return ERROR;
      }

    centre[0] = m[1] / m[i00];
    centre[1] = m[2] / m[i00];
    return OK;
  }
#undef FNAME
//
  void CopyMoments(const double ms[15], double md[15])
  {
    for (unsigned int i = 0; i < 15; i++)
      {
        md[i] = ms[i];
      }
  }

  int PosSign(const double ms[15], double md[15])
  {
    if (ms[i00] < 0)
      for (unsigned int i = 0; i < 15; i++)
        {
          md[i] = -ms[i];
        }
    else
      {
        CopyMoments(ms, md);
      }

    return OK;
  }

  int PosSign(double ms[15])
  {
    if (ms[i00] < 0)
      for (unsigned int i = 0; i < 15; i++)
        {
          ms[i] = -ms[i];
        }

    return OK;
  }

  double DiffMoments2(const double m1[15], const double m2[15])
  {
    double dd;
    double sdif = 0.0;

    for (int i = 0; i < 15; i++)
      {
        dd = m1[i] - m2[i];
        sdif += dd * dd;
      }

    return dd;
  }

  double DiffMoments(const double m1[15], const double m2[15])
  {
    return sqrt(DiffMoments2(m1, m2));
  }

//***********************************************
//* Transformation der Momente bei Translation  *
//***********************************************
#define FNAME "TranslateMoments"
  int TranslateMoments(const double m1[15], double x, double y, double m2[15])
  {
    double m[15];

    CopyMoments(m1, m); // falls m1==m2
    CopyMoments(m1, m2);

    const double x2 = x * x;
    const double x3 = x2 * x;
    const double x4 = x2 * x2;
    const double y2 = y * y;
    const double y3 = y2 * y;
    const double y4 = y2 * y2;
    /*
        std::cout << x << " " << x2 << std::endl;
        std::cout << y << " " << y2 <<  std::endl;
        std::cout << m2[3] << std::endl;
        std::cout << m2[5] << std::endl;
    */
    m2[1]  += x * m[i00];
    m2[2]  += y * m[i00];
    m2[3]  += 2 * x * m[1] + x2 * m[i00];
    m2[4]  += y * m[1] + x * m[2] + x * y * m[i00];
    m2[5]  += 2 * y * m[2] + y2 * m[i00];
    /*
        std::cout << m2[3] << std::endl;
        std::cout << m2[5] << std::endl;
    */
    m2[6]  += 3 * x * m[3] + 3 * x2 * m[1] + x3 * m[i00];
    m2[7]  += 2 * x * m[4] + y * m[3] + x2 * m[2] + 2 * x * y * m[1] + x2 * y * m[i00];
    m2[8]  += x * m[5] + 2 * y * m[4] + 2 * x * y * m[2] + y2 * m[1] + x * y2 * m[i00];
    m2[9]  += 3 * y * m[5] + 3 * y2 * m[2] + y3 * m[i00];
    m2[10] += 4 * x * m[6] + 6 * x2 * m[3] + 4 * x3 * m[1] + x4 * m[i00];
    m2[11] += 3 * x * m[7] + y * m[6] + 3 * x2 * m[4] + 3 * x * y * m[3]
              + x3 * m[2] + 3 * x2 * y * m[1] + x3 * y * m[i00];
    m2[12] += 2 * x * m[8] + 2 * y * m[7] + x2 * m[5] + 4 * x * y * m[4] + y2 * m[3]
              + 2 * x2 * y * m[2] + 2 * x * y2 * m[1] + x2 * y2 * m[i00];
    m2[13] += x * m[9] + 3 * y * m[8] + 3 * x * y * m[5] + 3 * y2 * m[4] + 3 * x * y2 * m[2]
              + y3 * m[1] + x * y3 * m[i00];
    m2[14] += 4 * y * m[9] + 6 * y2 * m[5] + 4 * y3 * m[2] + y4 * m[i00];

    return OK;
  }
#undef FNAME
//**********************************************
//* Transformation der Momente bei X-Scherung  *
//**********************************************
#define FNAME "XShearMoments"
  int XShearMoments(const double m1[15], double a, double m2[15])
  {
    double mx[15];

    CopyMoments(m1, mx); // falls m1==m2
    CopyMoments(m1, m2);

    m2[1]  += mx[2] * a;
    m2[3]  += (mx[5] * a + 2 * mx[4]) * a;
    m2[4]  += mx[5] * a;
    m2[6]  += ((mx[9] * a + 3 * mx[8]) * a + 3 * mx[7]) * a;
    m2[7]  += (mx[9] * a + 2 * mx[8]) * a;
    m2[8]  += mx[9] * a;
    m2[10] += (((mx[14] * a + 4 * mx[13]) * a + 6 * mx[12]) * a + 4 * mx[11]) * a;
    m2[11] += ((mx[14] * a + 3 * mx[13]) * a + 3 * mx[12]) * a;
    m2[12] += (mx[14] * a + 2 * mx[13]) * a;
    m2[13] += mx[14] * a;
    return OK;
  }
#undef FNAME
//*************************************************
//* Transformation der Momente bei Y-Scherung     *
//*************************************************
#define FNAME "YShearMoments"
  int YShearMoments(const double m1[15], double b, double m2[15])
  {
    double mx[15];

    CopyMoments(m1, mx); // falls m1==m2
    CopyMoments(m1, m2);

    m2[4]  += mx[3] * b;
    m2[5]  += (mx[3] * b + 2 * mx[4]) * b;
    m2[7]  += mx[6] * b;
    m2[8]  += (mx[6] * b + 2 * mx[7]) * b;
    m2[9]  += ((mx[6] * b + 3 * mx[7]) * b + 3 * mx[8]) * b;
    m2[11] += mx[10] * b;
    m2[12] += (mx[10] * b + 2 * mx[11]) * b;
    m2[13] += ((mx[10] * b + 3 * mx[11]) * b + 3 * mx[12]) * b;
    m2[14] += (((mx[10] * b + 4 * mx[11]) * b + 6 * mx[12]) * b + 4 * mx[13]) * b;
    return OK;
  }
#undef FNAME
//*************************************************
//* Transformation der Momente bei X-Spiegelung   *
//*************************************************
#define FNAME "XReflectMoments"
  int XReflectMoments(double m[15])
  {
    m[i00] = -m[i00];
    m[3] = -m[3];
    m[5] = -m[5];
    m[7] = -m[7];
    m[9] = -m[9];
    m[10] = -m[10];
    m[12] = -m[12];
    m[14] = -m[14];
    return OK;
  }
#undef FNAME
//****************************************************
//* Transformation der Momente bei Y-Spiegelung      *
//****************************************************
#define FNAME "YReflectMoments"
  int YReflectMoments(double m[15])
  {
    m[i00] = -m[i00];
    m[3] = -m[3];
    m[5] = -m[5];
    m[6] = -m[6];
    m[8] = -m[8];
    m[10] = -m[10];
    m[12] = -m[12];
    m[14] = -m[14];
    return OK;
  }
#undef FNAME
//**************************************************************
// Transformation der Momente bei anisotroper Skalierung
//**************************************************************
#define FNAME "ScaleMoments"
  int ScaleMoments(const double m1[15],
                   double a, double b,
                   double m2[15])
  {
    double a2, a3, a4, a5;
    double b2, b3, b4, b5;

    CopyMoments(m1, m2);

    a2 = a * a;
    a3 = a2 * a;
    a4 = a3 * a;
    a5 = a4 * a;
    b2 = b * b;
    b3 = b2 * b;
    b4 = b3 * b;
    b5 = b4 * b;

    m2[i00] *= a * b;
    m2[1] *= a2 * b;
    m2[2] *= b2 * a;
    m2[3] *= a3 * b;
    m2[4] *= a2 * b2;
    m2[5] *= a * b3;
    m2[6] *= a4 * b;
    m2[7] *= a3 * b2;
    m2[8] *= a2 * b3;
    m2[9] *= a * b4;
    m2[10] *= a5 * b;
    m2[11] *= a4 * b2;
    m2[12] *= a3 * b3;
    m2[13] *= a2 * b4;
    m2[14] *= a * b5;
    return OK;
  }
#undef FNAME
//**************************************************************
// Transformation der Momente bei Rotation
//**************************************************************
#define FNAME "RotateMoments"
  int RotateMoments(const double m1[15], double c, double s, double m2[15])
  {
    double c2 = c * c;
    double s2 = s * s;

    double mx[15];

    for (int i = 0; i < 15; i++)
      {
        mx[i] = m1[i];
      }

    m2[i00] = mx[i00];
    m2[1]   = c * mx[1] - s * mx[2];
    m2[2]   = c * mx[2] + s * mx[1];
    m2[3]   = c2 * mx[3] + s * (-2 * c * mx[4] + s * mx[5]);
    m2[4]   = c * s * (mx[3] - mx[5]) + (c2 - s2) * mx[4];
    m2[5]   = s * (s * mx[3] + 2 * c * mx[4]) + c2 * mx[5];
    m2[6]   = c2 * (c * mx[6] - 3 * s * mx[7]) + s2 * (3 * c * mx[8] - s * mx[9]);
    m2[7]   = c2 * (c * mx[7] + s * (mx[6] - 2 * mx[8])) + s2 * (s * mx[8] + c * (mx[9] - 2 * mx[7]));
    m2[8]   = c2 * (c * mx[8] - s * (mx[9] - 2 * mx[7])) - s2 * (s * mx[7] - c * (mx[6] - 2 * mx[8]));
    m2[9]   = s2 * (s * mx[6] + 3 * c * mx[7]) + c2 * (3 * s * mx[8] + c * mx[9]);
    m2[10]  = c2 * (c * (c * mx[10] - 4 * s * mx[11]) + 6 * s2 * mx[12]) + s2 * s * (-4 * c * mx[13] + s * mx[14]);
    m2[11]  = c2 * (c * (c * mx[11] + s * (mx[10] - 3 * mx[12])) + s2 * 3 * (-mx[11] + mx[13])) + \
              s2 * (s * (c * (3 * mx[12] - mx[14]) - s * mx[13]));
    m2[12]  = c2 * (s2 * (mx[10] - 4 * mx[12] + mx[14]) + c * (2 * s * (mx[11] - mx[13]) + c * mx[12])) + \
              s2 * s * (s * mx[12] - 2 * c * (mx[11] - mx[13]));
    m2[13]  = s2 * (s * (c * (mx[10] - 3 * mx[12]) - s * mx[11]) + 3 * c2 * (mx[11] - mx[13])) + \
              c2 * (c * (s * (3 * mx[12] - mx[14]) + c * mx[13]));
    m2[14]  = s2 * (s * (s * mx[10] + 4 * c * mx[11]) + 6 * c2 * mx[12]) + c2 * c * (4 * s * mx[13] + c * mx[14]);
    return OK;
  }

  int RotateMoments(const double m1[15], double fi, double m2[15])
  {
    return RotateMoments(m1, cos(fi), sin(fi), m2);
  }

#undef FNAME
//********************************************************
// allgemeine affine Transformation von Momenten
//********************************************************
#define FNAME "AffinTransMoments"
  int AffinTransMoments(const double m1[15], double tr[3][3], double m2[5])
  {
    double m[15], det;

    CopyMoments(m1, m);

    det = tr[0][0] * tr[1][1] - tr[0][1] * tr[1][0];

    m2[i00] = m[i00] * det;
    m2[1] = (tr[0][0] * m[1] + tr[0][1] * m[2] + tr[0][2] * m[i00]) * det;
    m2[2] = (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00]) * det;
    m2[3] = (tr[0][0] * (tr[0][0] * m[3] + tr[0][1] * m[4] + tr[0][2] * m[1]) + \
             tr[0][1] * (tr[0][0] * m[4] + tr[0][1] * m[5] + tr[0][2] * m[2]) + \
             tr[0][2] * (tr[0][0] * m[1] + tr[0][1] * m[2] + tr[0][2] * m[i00])) * det;
    m2[4] = (tr[0][0] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1]) + \
             tr[0][1] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2]) + \
             tr[0][2] * (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00])) * det;
    m2[5] = (tr[1][0] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1]) + \
             tr[1][1] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2]) + \
             tr[1][2] * (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00])) * det;
    m2[6] = (tr[0][0] * (tr[0][0] * (tr[0][0] * m[6] + tr[0][1] * m[7] + tr[0][2] * m[3]) + \
                         tr[0][1] * (tr[0][0] * m[7] + tr[0][1] * m[8] + tr[0][2] * m[4]) + \
                         tr[0][2] * (tr[0][0] * m[3] + tr[0][1] * m[4] + tr[0][2] * m[1])) + \
             tr[0][1] * (tr[0][0] * (tr[0][0] * m[7] + tr[0][1] * m[8] + tr[0][2] * m[4]) + \
                         tr[0][1] * (tr[0][0] * m[8] + tr[0][1] * m[9] + tr[0][2] * m[5]) + \
                         tr[0][2] * (tr[0][0] * m[4] + tr[0][1] * m[5] + tr[0][2] * m[2])) + \
             tr[0][2] * (tr[0][0] * (tr[0][0] * m[3] + tr[0][1] * m[4] + tr[0][2] * m[1]) + \
                         tr[0][1] * (tr[0][0] * m[4] + tr[0][1] * m[5] + tr[0][2] * m[2]) + \
                         tr[0][2] * (tr[0][0] * m[1] + tr[0][1] * m[2] + tr[0][2] * m[i00]))) * det;
    m2[7] = (tr[0][0] * (tr[0][0] * (tr[1][0] * m[6] + tr[1][1] * m[7] + tr[1][2] * m[3]) + \
                         tr[0][1] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[0][2] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1])) + \
             tr[0][1] * (tr[0][0] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[0][1] * (tr[1][0] * m[8] + tr[1][1] * m[9] + tr[1][2] * m[5]) + \
                         tr[0][2] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2])) + \
             tr[0][2] * (tr[0][0] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1]) + \
                         tr[0][1] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2]) + \
                         tr[0][2] * (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00]))) * det;
    m2[8] = (tr[0][0] * (tr[1][0] * (tr[1][0] * m[6] + tr[1][1] * m[7] + tr[1][2] * m[3]) + \
                         tr[1][1] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[1][2] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1])) + \
             tr[0][1] * (tr[1][0] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[1][1] * (tr[1][0] * m[8] + tr[1][1] * m[9] + tr[1][2] * m[5]) + \
                         tr[1][2] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2])) + \
             tr[0][2] * (tr[1][0] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1]) + \
                         tr[1][1] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2]) + \
                         tr[1][2] * (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00]))) * det;
    m2[9] = (tr[1][0] * (tr[1][0] * (tr[1][0] * m[6] + tr[1][1] * m[7] + tr[1][2] * m[3]) + \
                         tr[1][1] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[1][2] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1])) + \
             tr[1][1] * (tr[1][0] * (tr[1][0] * m[7] + tr[1][1] * m[8] + tr[1][2] * m[4]) + \
                         tr[1][1] * (tr[1][0] * m[8] + tr[1][1] * m[9] + tr[1][2] * m[5]) + \
                         tr[1][2] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2])) + \
             tr[1][2] * (tr[1][0] * (tr[1][0] * m[3] + tr[1][1] * m[4] + tr[1][2] * m[1]) + \
                         tr[1][1] * (tr[1][0] * m[4] + tr[1][1] * m[5] + tr[1][2] * m[2]) + \
                         tr[1][2] * (tr[1][0] * m[1] + tr[1][1] * m[2] + tr[1][2] * m[i00]))) * det;
    m2[10] = (tr[0][0] * (tr[0][0] * (tr[0][0] * (tr[0][0] * m[10] + tr[0][1] * m[11] + tr[0][2] * m[6]) + \
                                      tr[0][1] * (tr[0][0] * m[11] + tr[0][1] * m[12] + tr[0][2] * m[7]) + \
                                      tr[0][2] * (tr[0][0] * m[ 6] + tr[0][1] * m[ 7] + tr[0][2] * m[3])) + \
                          tr[0][1] * (tr[0][0] * (tr[0][0] * m[11] + tr[0][1] * m[12] + tr[0][2] * m[7]) + \
                                      tr[0][1] * (tr[0][0] * m[12] + tr[0][1] * m[13] + tr[0][2] * m[8]) + \
                                      tr[0][2] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4])) + \
                          tr[0][2] * (tr[0][0] * (tr[0][0] * m[ 6] + tr[0][1] * m[ 7] + tr[0][2] * m[3]) + \
                                      tr[0][1] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4]) + \
                                      tr[0][2] * (tr[0][0] * m[ 3] + tr[0][1] * m[ 4] + tr[0][2] * m[1]))) + \
              tr[0][1] * (tr[0][0] * (tr[0][0] * (tr[0][0] * m[11] + tr[0][1] * m[12] + tr[0][2] * m[7]) + \
                                      tr[0][1] * (tr[0][0] * m[12] + tr[0][1] * m[13] + tr[0][2] * m[8]) + \
                                      tr[0][2] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4])) + \
                          tr[0][1] * (tr[0][0] * (tr[0][0] * m[12] + tr[0][1] * m[13] + tr[0][2] * m[8]) + \
                                      tr[0][1] * (tr[0][0] * m[13] + tr[0][1] * m[14] + tr[0][2] * m[9]) + \
                                      tr[0][2] * (tr[0][0] * m[ 8] + tr[0][1] * m[ 9] + tr[0][2] * m[5])) + \
                          tr[0][2] * (tr[0][0] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4]) + \
                                      tr[0][1] * (tr[0][0] * m[ 8] + tr[0][1] * m[ 9] + tr[0][2] * m[5]) + \
                                      tr[0][2] * (tr[0][0] * m[ 4] + tr[0][1] * m[ 5] + tr[0][2] * m[2]))) + \
              tr[0][2] * (tr[0][0] * (tr[0][0] * (tr[0][0] * m[ 6] + tr[0][1] * m[ 7] + tr[0][2] * m[3]) + \
                                      tr[0][1] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4]) + \
                                      tr[0][2] * (tr[0][0] * m[ 3] + tr[0][1] * m[ 4] + tr[0][2] * m[1])) + \
                          tr[0][1] * (tr[0][0] * (tr[0][0] * m[ 7] + tr[0][1] * m[ 8] + tr[0][2] * m[4]) + \
                                      tr[0][1] * (tr[0][0] * m[ 8] + tr[0][1] * m[ 9] + tr[0][2] * m[5]) + \
                                      tr[0][2] * (tr[0][0] * m[ 4] + tr[0][1] * m[ 5] + tr[0][2] * m[2])) + \
                          tr[0][2] * (tr[0][0] * (tr[0][0] * m[ 3] + tr[0][1] * m[ 4] + tr[0][2] * m[1]) + \
                                      tr[0][1] * (tr[0][0] * m[ 4] + tr[0][1] * m[ 5] + tr[0][2] * m[2]) + \
                                      tr[0][2] * (tr[0][0] * m[ 1] + tr[0][1] * m[ 2] + tr[0][2] * m[i00])))) * det;
    m2[11] = (tr[0][0] * (tr[0][0] * (tr[0][0] * (tr[1][0] * m[10] + tr[1][1] * m[11] + tr[1][2] * m[6]) + \
                                      tr[0][1] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[0][2] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3])) + \
                          tr[0][1] * (tr[0][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[0][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[0][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[0][2] * (tr[0][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[0][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[0][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]))) + \
              tr[0][1] * (tr[0][0] * (tr[0][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[0][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[0][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[0][1] * (tr[0][0] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[0][1] * (tr[1][0] * m[13] + tr[1][1] * m[14] + tr[1][2] * m[9]) + \
                                      tr[0][2] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5])) + \
                          tr[0][2] * (tr[0][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[0][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[0][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]))) + \
              tr[0][2] * (tr[0][0] * (tr[0][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[0][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[0][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1])) + \
                          tr[0][1] * (tr[0][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[0][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[0][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2])) + \
                          tr[0][2] * (tr[0][0] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]) + \
                                      tr[0][1] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]) + \
                                      tr[0][2] * (tr[1][0] * m[ 1] + tr[1][1] * m[ 2] + tr[1][2] * m[0])))) * det;
    m2[12] = (tr[0][0] * (tr[0][0] * (tr[1][0] * (tr[1][0] * m[10] + tr[1][1] * m[11] + tr[1][2] * m[6]) + \
                                      tr[1][1] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][2] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3])) + \
                          tr[0][1] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[0][2] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]))) + \
              tr[0][1] * (tr[0][0] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[0][1] * (tr[1][0] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][1] * (tr[1][0] * m[13] + tr[1][1] * m[14] + tr[1][2] * m[9]) + \
                                      tr[1][2] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5])) + \
                          tr[0][2] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]))) + \
              tr[0][2] * (tr[0][0] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1])) + \
                          tr[0][1] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2])) + \
                          tr[0][2] * (tr[1][0] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]) + \
                                      tr[1][1] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]) + \
                                      tr[1][2] * (tr[1][0] * m[ 1] + tr[1][1] * m[ 2] + tr[1][2] * m[0])))) * det;
    m2[13] = (tr[0][0] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[10] + tr[1][1] * m[11] + tr[1][2] * m[6]) + \
                                      tr[1][1] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][2] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]))) + \
              tr[0][1] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][1] * (tr[1][0] * m[13] + tr[1][1] * m[14] + tr[1][2] * m[9]) + \
                                      tr[1][2] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]))) + \
              tr[0][2] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]) + \
                                      tr[1][1] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]) + \
                                      tr[1][2] * (tr[1][0] * m[ 1] + tr[1][1] * m[ 2] + tr[1][2] * m[0])))) * det;
    m2[14] = (tr[1][0] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[10] + tr[1][1] * m[11] + tr[1][2] * m[6]) + \
                                      tr[1][1] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][2] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]))) + \
              tr[1][1] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[11] + tr[1][1] * m[12] + tr[1][2] * m[7]) + \
                                      tr[1][1] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][2] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[12] + tr[1][1] * m[13] + tr[1][2] * m[8]) + \
                                      tr[1][1] * (tr[1][0] * m[13] + tr[1][1] * m[14] + tr[1][2] * m[9]) + \
                                      tr[1][2] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]))) + \
              tr[1][2] * (tr[1][0] * (tr[1][0] * (tr[1][0] * m[ 6] + tr[1][1] * m[ 7] + tr[1][2] * m[3]) + \
                                      tr[1][1] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][2] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1])) + \
                          tr[1][1] * (tr[1][0] * (tr[1][0] * m[ 7] + tr[1][1] * m[ 8] + tr[1][2] * m[4]) + \
                                      tr[1][1] * (tr[1][0] * m[ 8] + tr[1][1] * m[ 9] + tr[1][2] * m[5]) + \
                                      tr[1][2] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2])) + \
                          tr[1][2] * (tr[1][0] * (tr[1][0] * m[ 3] + tr[1][1] * m[ 4] + tr[1][2] * m[1]) + \
                                      tr[1][1] * (tr[1][0] * m[ 4] + tr[1][1] * m[ 5] + tr[1][2] * m[2]) + \
                                      tr[1][2] * (tr[1][0] * m[ 1] + tr[1][1] * m[ 2] + tr[1][2] * m[0])))) * det;
    return OK;
  }
#undef FNAME

#define FNAME "CalcCentralMoments"
  int CalcCentralMoments(const double m[15], double mc[15])
  {
    if (m[0] == 0.0)
      throw IceException(FNAME, M_EMPTY_OBJECT);

    double xs = m[1] / m[0];
    double ys = m[2] / m[0];
    return TranslateMoments(m, -xs, -ys, mc);
  }
#undef FNAME

//*********************************************************
// Normierung der Momente auf Translation, Scherung, anisotrope
// Skalierung und Rotation (Wang)
//*********************************************************

  /* AffinNormMoments ist eine ICE-Funktion, die aus den Momenten eines
   * gegebenen Objektes die Momente der Standardlage und die affine
   * Transformation auf die Standardlage bereitstellt, dabei wird als
   * Normalisierung die "Standardmethode" realisiert, es wird dabei
   * garantiert, dass m21+m03>=0 für die Standardlage gilt */

#define FNAME "NormMomentsTranslation"
  int NormalizeMomentsTranslation(const double m[15], double msk[15],
                                  double& xs, double& ys)
  {
    xs = 0;
    ys = 0;

    if (fabs(m[0]) < 1e-15)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    PosSign(m, msk);

    if (msk[1] != 0 || msk[2] != 0)
      {
        xs = -msk[1] / msk[0];
        ys = -msk[2] / msk[0];
        TranslateMoments(msk, xs, ys, msk);
      }

    return OK;
  }
#undef FNAME
#define FNAME "NormalizeMomentsXShearing"
  int NormalizeMomentsXShearing(const double m[15], double msk[15],
                                double& beta)
  {
    PosSign(m, msk);
    // Scherungsnormierung in x-Richtung
    beta = -msk[4] / msk[5];
    XShearMoments(msk, beta, msk);
    return OK;
  }
#undef FNAME
#define FNAME "NormalizeMomentsYShearing"
  int NormalizeMomentsYShearing(const double m[15], double msk[15],
                                double& beta)
  {
    PosSign(m, msk);
    // Scherungsnormierung in y-Richtung
    beta = -msk[11] / msk[10];
    YShearMoments(msk, beta, msk);
    return OK;
  }
#undef FNAME
#define FNAME "NormalizeMomentsScaling"
  int NormalizeMomentsScaling(const double m[15], double msk[15],
                              double& alpha, double& gamma)
  {
    PosSign(m, msk);

//    std::cout << msk[3] << "," << msk[5] << std::endl;

    // anisotrope Skalierungsnormierung
    if ((fabs(msk[3]) < 1e-5) || (fabs(msk[5]) < 1e-5))
      throw IceException(FNAME, M_NUM_INSTABILITY);

    double h1 = msk[5] / (msk[3] * msk[3] * msk[3]);
    double h2 = msk[3] / (msk[5] * msk[5] * msk[5]);

    if (h1 < 0 || h2 < 0)
      throw IceException(FNAME, M_NUM_INSTABILITY);

    alpha = pow(h1, 0.125);
    gamma = pow(h2, 0.125);
    ScaleMoments(msk, alpha, gamma, msk);
    return OK;
  }
#undef FNAME
#define FNAME "NormalizeMomentsRotation"
  int NormalizeMomentsRotation(const double m[15], double msk[15],
                               double& cosinus, double& sinus)
  {
    PosSign(m, msk);

    // Rotationsparameter c und s
    double b = msk[9] + msk[7];
    double a = msk[6] + msk[8];

    if (fabs(a) < 1.0e-5 && fabs(b) < 1.0e-5)
      {
        // wegen Symmetrie keine Rotation
        cosinus = 1.0;
        sinus = 1.0;
        return OK;
      }

    sinus = a / sqrt(a * a + b * b);
    cosinus = b / sqrt(a * a + b * b);

    RotateMoments(msk, cosinus, sinus, msk);
    return OK;
  }
#undef FNAME
#define FNAME "AffinNormMoments"
  int AffinNormMoments(const double m[15], double ms[15], double atr[3][3])
  {
    double alpha, beta, gamma, c, s, xs, ys;

    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(m, ms, xs, ys));

    RETURN_ERROR_IF_FAILED(NormalizeMomentsXShearing(ms, ms, beta));

    RETURN_ERROR_IF_FAILED(NormalizeMomentsScaling(ms, ms, alpha, gamma));

    RETURN_ERROR_IF_FAILED(NormalizeMomentsRotation(ms, ms, c, s));

    InitTrans(atr);
    // Berechnung der Transformationsmatrix
    atr[0][0] = alpha * c;
    atr[0][1] = alpha * beta * c - gamma * s;
    atr[1][0] = alpha * s;
    atr[1][1] = alpha * beta * s + gamma * c;
    atr[0][2] = atr[0][0] * xs + atr[0][1] * ys;
    atr[1][2] = atr[1][0] * xs + atr[1][1] * ys;
    return OK;
  }
#undef FNAME
//*********************************************************
// Normierung der Momente mit der Polynommethode (Voss)
// X-Scherung, Y-Scherung, Skalierung
//*********************************************************
#define FNAME "PolyNormMoments"
  int PolyNormMoments(const double m1[15], double m2[15], double atr[3][3])
  {
    double mx[3][15];
    std::vector<Complex> sol(3);
    double a[3], b[3], c[3], d[3], s, smin, xs, ys;
    int i, imin, k, l;
    xs = 0;
    ys = 0;

    PosSign(m1, m2);

    //Translationsnormierung
    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(m2, m2, xs, ys));

    if (fabs(m2[9]) < 1e-15)
      {
        if (m2[8] != 0)
          {
            double p2 = m2[7] / (2 * m2[8]);
            double q = m2[6] / (3 * m2[8]);

            double h = p2 * p2 - q;

            if (h < 0)
              throw IceException(FNAME, M_WRONG_PARAMETER);

            h = sqrt(h);
            sol[0] = Complex(-p2 + h, 0.0);
            sol[1] = Complex(-p2 - h, 0.0);
            k = 0;
            l = 2;
          }
        else
          {
            if (m2[7] == 0)
              throw IceException(FNAME, M_WRONG_PARAMETER);
            else
              {
                sol[0] = Complex(-m2[6] / (3 * m2[7]), 0.0);
                k = 0;
                l = 1;
              }
          }
      }
    //m03!=0 --> Gleichung 3. Grades
    else
      {
        Root3(3 * m2[8] / m2[9], 3 * m2[7] / m2[9], m2[6] / m2[9], sol);
        //Lösungen mit gleichem Realteil aussortieren
        k = 0;
        l = 3;

        if (sol[0].real() == sol[1].real())
          {
            k = 1;
          }

        if (sol[1].real() == sol[2].real())
          {
            l = 2;
          }
      }

    //Normallagen für alle unterschiedlichen reellen Lösungen berechnen
    smin = 1e32;
    imin = -1;

    for (i = k; i < l; i++)
      {
        if (sol[i].imag() == 0)
          {
            a[i] = sol[i].real();
            XShearMoments(m2, a[i], mx[i]);
            b[i] = -mx[i][4] / mx[i][3];
            YShearMoments(mx[i], b[i], mx[i]);

            if (mx[i][5]*mx[i][3] > 1e-10)
              {
                c[i] = pow(mx[i][5] / (mx[i][3] * mx[i][3] * mx[i][3]), 0.125);
                d[i] = pow(mx[i][3] / (mx[i][5] * mx[i][5] * mx[i][5]), 0.125);
                ScaleMoments(mx[i], c[i], d[i], mx[i]);
                //Normallage mit m40+m04 --> min. auswählen
                s = mx[i][10] + mx[i][14];

                if (s < smin)
                  {
                    smin = s;
                    imin = i;
                  }
              }
          }
      }

    if (imin < 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    CopyMoments(mx[imin], m2);
    //    for (i=0;i<15;i++)
    //      m2[i]=mx[imin][i];

    // affine Transformation für ausgewählte Normallage
    InitTrans(atr);
    atr[0][0] = c[imin];
    atr[0][1] = a[imin] * c[imin];
    atr[1][0] = b[imin] * d[imin];
    atr[1][1] = (1 + a[imin] * b[imin]) * d[imin];

    if (m2[7] < 0)
      {
        atr[0][0] = -atr[0][0];
        atr[0][1] = -atr[0][1];
        atr[1][0] = -atr[1][0];
        atr[1][1] = -atr[1][1];
        m2[7] = -m2[7];
        m2[8] = -m2[8];
        m2[9] = -m2[9];
      }

    atr[0][2] = atr[0][0] * xs + atr[0][1] * ys;
    atr[1][2] = atr[1][0] * xs + atr[1][1] * ys;
    return OK;
  }
#undef FNAME
//*************************************************************
// Aus den auf Scherung in x-Richtung und anisotrope Skalierung
// normierten Momenten werden die Hu-Momente,
// in die nur Momente 3. und 4. Ordnung eingehen, berechnet
//*************************************************************

#define FNAME "HuInvariants"
  int HuInvariants(const double ms[15], double hu[7])
  {
    double h1  = ms[i30] + ms[8];
    double h2  = ms[7] + ms[9];
    double h3  = 3 * ms[7] - ms[9];
    double h4  = ms[6] - 3 * ms[8];
    double h12 = h1 * h1;
    double h22 = h2 * h2;
    double h32 = h3 * h3;
    double h42 = h4 * h4;
    double h5  = h1 * (h12 - 3 * h22);
    double h6  = h2 * (3 * h12 - h22);

    hu[0] = fabs(h42 + h32);
    hu[1] = fabs(h12 + h22);
    hu[2] = fabs(h4 * h5 + h3 * h6);
    hu[3] = fabs(h3 * h5 - h4 * h6);
    hu[4] = Sqr(ms[10] - ms[14]) + 4 * Sqr(ms[11] + ms[13]);
    hu[5] = Sqr(ms[10] - 6 * ms[12] + ms[14]) + 16 * Sqr(ms[11] - ms[13]);
    hu[6] = fabs(ms[10] + 2 * ms[12] + ms[14]);

    return OK;
  }
#undef FNAME

#define FNAME "AffinHuInvar"
  int AffinHuInvar(const double m[15], double hu[7], bool trans)
  {
    double ms[15], alpha, beta, gamma, xs, ys;

    if (trans)
      {
        RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(m, ms, xs, ys));
      }
    else
      {
        CopyMoments(m, ms);
      }

    RETURN_ERROR_IF_FAILED(NormalizeMomentsXShearing(ms, ms, beta));

    RETURN_ERROR_IF_FAILED(NormalizeMomentsScaling(ms, ms, alpha, gamma));

    HuInvariants(ms, hu);

    return OK;
  }
#undef FNAME
//************************************************************
// Affine Momente (Iterationsmethode Voss)
//************************************************************
#define FNAME "AffinIterateMoments"
  int AffinIterateMoments(const double m[15], double maf[15], double atr[3][3])
  {
    int i;
    double alpha = 0, beta = 0, gamma, delta, h1, h2, xs, ys;

    // Aenderung des Umlaufsinnes, wenn m00 < 0
    PosSign(m, maf);

    //Translationsnormierung
    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(maf, maf, xs, ys));

    InitTrans(atr);
    // evtl. instabile Standardlage bei achsenparallelen Figuren,
    // desh. vorher Drehung um bel. Winkel

    if (fabs(maf[13]) < 1e-10 || fabs(maf[11]) < 1e-10)
      {
        h1 = cos(0.5);
        h2 = sin(0.5);
        RotateMoments(maf, h1, h2, maf);
        atr[0][0] = h1;
        atr[0][1] = -h2;
        atr[1][0] = h2;
        atr[1][1] = h1;
      }

    // iterative Scherungsnormierung maf13=maf31=0
    if ((maf[14] < EPSILON) && (maf[10] < EPSILON))
      throw IceException(FNAME, M_NUM_INSTABILITY);

    bool modified = true;

    for (i = 0; i < 200 && modified; i++)
      {
        modified = false;

        if (fabs(maf[14]) > 1e-20)
          {
            NormalizeMomentsXShearing(maf, maf, alpha);
            atr[0][0] += alpha * atr[1][0];
            atr[0][1] += alpha * atr[1][1];
            modified = true;
          }

        if (fabs(maf[10]) > 1e-20)
          {
            NormalizeMomentsYShearing(maf, maf, beta);
            atr[1][0] += beta * atr[0][0];
            atr[1][1] += beta * atr[0][1];
            modified = true;
          }
      }

    // anisotrope Skalierungsnormierung
    RETURN_ERROR_IF_FAILED(NormalizeMomentsScaling(maf, maf, gamma, delta));

    atr[0][0] *= gamma;
    atr[0][1] *= gamma;
    atr[1][0] *= delta;
    atr[1][1] *= delta;

    atr[0][2] = atr[0][0] * xs + atr[0][1] * ys;
    atr[1][2] = atr[1][0] * xs + atr[1][1] * ys;

    return i;
  }
#undef FNAME
//************************************************************
// Affine Algebraische Invarianten (Flusser und Reiss)
//************************************************************
#define FNAME "AffinAlgebraicInvar"
  int AffinAlgebraicInvar(const double mp[15], double fl[4])
  {
    double m[15];

    // Aenderung des Umlaufsinnes, wenn m00 < 0
    PosSign(mp, m);

    double m0 = m[0];
    double m02 = m0 * m0;
    double m04 = m02 * m02;
    double m07 = m0 * m02 * m04;
    double m08 = m04 * m04;
    double m010 = m08 * m02;
    double m011 = m010 * m0;

    fl[0] = (m[3] * m[5] - m[4] * m[4]) / m04;

    fl[1] = (m[6] * (m[9] * (m[6] * m[9] - 6 * m[7] * m[8]) + 4 * m[8] * m[8] * m[8]) +
             m[7] * m[7] * (4 * m[7] * m[9] - 3 * m[8] * m[8])) / m010;

    fl[2] = (m[3] * (m[7] * m[9] - m[8] * m[8]) - m[4] * (m[6] * m[9] - m[7] * m[8]) +
             m[5] * (m[6] * m[8] - m[7] * m[7])) / m07;

    fl[3] = (m[3] * m[3] * (m[9] * (m[3] * m[9] -
                                    6 * m[4] * m[8] -
                                    6 * m[5] * m[7]) +
                            9 * m[5] * m[8] * m[8]) +
             m[5] * m[5] * (m[6] * (m[5] * m[6] -
                                    6 * m[4] * m[7] -
                                    6 * m[3] * m[8]) +
                            9 * m[3] * m[7] * m[7]) +
             m[4] * (m[4] * m[6] * (-8 * m[4] * m[9] +
                                    12 * m[5] * m[8]) +
                     m[3] * (m[7] * (12 * m[4] * m[9] -
                                     18 * m[5] * m[8]) +
                             6 * m[5] * m[6] * m[9]))) / m011;
    return OK;
  }
#undef FNAME

//********************************************************
// Normierung der Momente auf vergleichbare Größen
// Division der n-ten Momente durch sqrt(m00)**(1+n/2)
//********************************************************

#define FNAME "NormalizeMomentsArea"
  int NormalizeMomentsArea(const double m1[15], double m2[15], double& alpha)
  {
    // Normalisierung auf Fläche nach m00
    double m[15];

    // evtl. Umlaufrichtung umkehren, so daß m00 > 0
    PosSign(m1, m);

    if (m[0] < EPSILON)
      throw IceException(FNAME, M_NUM_INSTABILITY);

    // Normierung
    alpha = 1.0 / sqrt(m[0]);

    m2[0] = 1;

    double fak = 1.0 / m[0] * alpha;

    for (int i = 1; i < 3; i++)   // Momente 1. Ordnung
      {
        m2[i] = m[i] * fak;
      }

    fak *= alpha;

    for (int i = 3; i < 6; i++)   // Momente 2. Ordnung
      {
        m2[i] = m[i] * fak;
      }

    fak *= alpha;

    for (int i = 6; i < 10; i++)   // Momente 3. Ordnung
      {
        m2[i] = m[i] * fak;
      }

    fak *= alpha;

    for (int i = 10; i < 15; i++)   // Momente 4. Ordnung
      {
        m2[i] = m[i] * fak;
      }

    return OK;
  }

  int NormalizeMomentsArea(const double m1[15], double m2[15])
  {
    double alpha;
    return NormalizeMomentsArea(m1, m2, alpha);
  }

#undef FNAME
//********************************************************
// Es wird eine affine Transformation bestimmt, die
// approximativ das Objekt mit den Momenten m1 in das
// Objekt mit den Momenten m2 überführt
//********************************************************
#define FNAME "AffinFitMoments"
  double AffinFitMoments(const double m1p[15], const double m2p[15],
                         double tr[3][3])
  {
    try
      {
        double m1[15], m2[15];
        double mx[15], mn1[15], mn2[15], s3, s4, smin;
        double tr1[3][3], tr2[3][3], trx[8][3][3], tra[8][3][3];
        int i, imin, j, k, i1, i2;

        PosSign(m1p, m1);
        PosSign(m2p, m2);

        // Flaeche darf nicht verschwinden
        if (m1[0] < EPSILON || m2[0] < EPSILON)
          throw IceException(FNAME, M_NUM_INSTABILITY);

        double dx, dy;
        NormalizeMomentsTranslation(m1, mx, dx, dy);
        //    TranslateMoments(m1,-m1[1]/m1[0],-m1[2]/m1[0],mx);

        // Normierung der Momente durch isotrope Skalierung auf gleiche Fläche
        NormalizeMomentsArea(mx, mx);

        // optimale Methode auswaehlen
        s3 = 0.0;

        for (int i = 6; i < 10; i++)
          {
            s3 += mx[i] * mx[i];  // Summe 3. Mom.
          }

        s4 = 0.0;

        for (int i = 10; i < 15; i++)
          {
            s4 += mx[i] * mx[i];  // Summe 4. Mom.
          }

        s3 /= 4;
        s4 /= 5;

        if (s3 > s4)
          {
            // 3. Momente groesser als 4. Momente --> Polynommethode
            PolyNormMoments(m1, mn1, tr1);
            PolyNormMoments(m2, mn2, tr2);
            i1 = 6;
            i2 = 10;
          }
        else
          {
            // Iterationsmethode ist besser
            AffinIterateMoments(m1, mn1, tr1);
            AffinIterateMoments(m2, mn2, tr2);
            i1 = 10;
            i2 = 15;

            // Sonderbehandlung für Dreiecke
            if (fabs(mn1[6] + mn1[8]) < 1e-5 && fabs(mn1[7] + mn1[9]) < 1e-5)
              {
                //Dreieck --> Polynommethode
                PolyNormMoments(m1, mn1, tr1);
                PolyNormMoments(m2, mn2, tr2);
                i1 = 6;
                i2 = 10;
              }
          }

        InvertTrans(tr2);
        // alle verbleibenden Möglichkeiten testen

        for (i = 0; i < 3; i++)
          for (j = 0; j < 3; j++)
            {
              trx[0][i][j] = tr2[i][j];
            }

        // Drehungen um 90 Grad
        for (i = 0, j = 1; j < 4; i++, j++)
          {
            for (k = 0; k < 3; k++)
              {
                trx[j][k][0] = trx[i][k][1];
                trx[j][k][1] = -trx[i][k][0];
                trx[j][k][2] = trx[i][k][2];
              }
          }

        // Spiegelung an der y-Achse
        for (i = 0; i < 3; i++)
          {
            trx[4][i][0] = -tr2[i][0];
            trx[4][i][1] = tr2[i][1];
            trx[4][i][2] = tr2[i][2];
          }

        // Drehungen um 90 Grad
        for (i = 4, j = 5; j < 8; i++, j++)
          {
            for (k = 0; k < 3; k++)
              {
                trx[j][k][0] = trx[i][k][1];
                trx[j][k][1] = -trx[i][k][0];
                trx[j][k][2] = trx[i][k][2];
              }
          }

        // Transformation Objekt1 - Objekt2 bestimmen
        // und Abstandsmaß für Momente berechnen
        smin = 1e20;
        imin = 0;

        for (i = 0; i < 8; i++)
          {
            MulMatrix((double*)trx[i], (double*)tr1, 3, 3, 3, (double*)tra[i]);
            AffinTransMoments(m1, tra[i], mn1);
            double fehlersumme = 0.0;

            for (j = i1; j < i2; j++)
              {
                fehlersumme += Sqr(mn1[j] - m2[j]);
              }

            if (fehlersumme < smin)
              {
                smin = fehlersumme;
                imin = i;
              }
          }

        // Rückgabe der Transformation mit minimalem Abstand
        for (i = 0; i < 3; i++)
          for (j = 0; j < 3; j++)
            {
              tr[i][j] = tra[imin][i][j];
            }

        return smin;
      }
    RETHROW;
  }

  double AffinFitMoments(const double m1[15], const double m2[15], Trafo& tr)
  {
    try
      {
        double oldtr[3][3];

        double ret = AffinFitMoments(m1, m2, oldtr);

        Matrix m(3, 3);

        for (int i = 0; i < 3; i++)
          for (int j = 0; j < 3; j++)
            {
              m[i][j] = oldtr[i][j];
            }

        tr = Trafo(m);
        return ret;
      }
    RETHROW;
  }
#undef FNAME
}

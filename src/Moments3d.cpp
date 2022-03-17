#include <cmath>
#include <complex>
#include "Moments3d.h"

namespace ice
{
  Moments3d::Moments3d(const std::vector<Vector3d>& pl)
  {
    for (auto& m : moments)
      {
        m = 0.0;
      }

    for (int i = 0; i < (int)pl.size(); i++)
      {
        addPoint(pl[i]);
      }
  }

  void Moments3d::addPoint(const Vector3d& p)
  {
    double x = p.x;
    double y = p.y;
    double z = p.z;

    moments[ind000] += 1;

    moments[ind100] += x;
    moments[ind010] += y;
    moments[ind001] += z;

    moments[ind200] += x * x;
    moments[ind110] += x * y;
    moments[ind020] += y * y;
    moments[ind011] += y * z;
    moments[ind002] += z * z;
    moments[ind101] += x * z;

    moments[ind300] += x * x * x;
    moments[ind210] += x * x * y;
    moments[ind120] += x * y * y;
    moments[ind030] += y * y * y;
    moments[ind021] += y * y * z;
    moments[ind012] += y * z * z;
    moments[ind003] += z * z * z;
    moments[ind102] += x * z * z;
    moments[ind201] += x * x * z;

    moments[ind111] += x * y * z;

  }

  Moments3d::Moments3d(const std::vector<Vector3d>& pointList,
                       const std::vector<std::vector<int>>& triangleList)
  {
    for (auto& m : moments)
      {
        m = 0.0;
      }
    for (const std::vector<int>& vi : triangleList)
      {
        Vector3d p1 = pointList[vi[0]];
        Vector3d p2 = pointList[vi[1]];
        Vector3d p3 = pointList[vi[2]];
        addTriangle(p1, p2, p3);
      }
  }

  void Moments3d::addTriangle(const Vector3d& p1,
                              const Vector3d& p2,
                              const Vector3d& p3)
  {
    const double a1 = p1.x, a2 = p1.y, a3 = p1.z;
    const double b1 = p2.x, b2 = p2.y, b3 = p2.z;
    const double c1 = p3.x, c2 = p3.y, c3 = p3.z;

    const double d1 = a1 - c1, d2 = a2 - c2, d3 = a3 - c3;
    const double e1 = b1 - c1, e2 = b2 - c2, e3 = b3 - c3;

    const double m00 = 0.5;
    const double m10 = 1 / 6.0, m01 = 1 / 6.0;
    const double m20 = 1 / 12.0, m02 = 1 / 12.0, m11 = 1 / 24.0;
    const double m30 = 1 / 20.0, m03 = 1 / 20.0, m21 = 1 / 60.0, m12 = 1 / 60.0;

    const double E = d1 * d1 + d2 * d2 + d3 * d3;
    const double F = d1 * e1 + d2 * e2 + d3 * e3;
    const double G = e1 * e1 + e2 * e2 + e3 * e3;
    const double C = sqrt(E * G - F * F);

    moments[ind000] += C * m00;
    moments[ind100] += C * (d1 * m10 + e1 * m01 + c1 * m00);
    moments[ind010] += C * (d2 * m10 + e2 * m01 + c2 * m00);
    moments[ind001] += C * (d3 * m10 + e3 * m01 + c3 * m00);

    moments[ind200] += C * (d1 * d1 * m20 + 2 * d1 * e1 * m11 + 2 * d1 * c1 * m10 + e1 * e1 * m02 + 2 * e1 * c1 * m01 + c1 * c1 * m00);
    moments[ind020] += C * (d2 * d2 * m20 + 2 * d2 * e2 * m11 + 2 * d2 * c2 * m10 + e2 * e2 * m02 + 2 * e2 * c2 * m01 + c2 * c2 * m00);
    moments[ind002] += C * (d3 * d3 * m20 + 2 * d3 * e3 * m11 + 2 * d3 * c3 * m10 + e3 * e3 * m02 + 2 * e3 * c3 * m01 + c3 * c3 * m00);

    moments[ind110] += C * (d1 * d2 * m20 + e1 * e2 * m02 + (d1 * e2 + d2 * e1) * m11 + (d1 * c2 + d2 * c1) * m10 + (e1 * c2 + e2 * c1) * m01 + c1 * c2 * m00);
    moments[ind101] += C * (d1 * d3 * m20 + e1 * e3 * m02 + (d1 * e3 + d3 * e1) * m11 + (d1 * c3 + d3 * c1) * m10 + (e1 * c3 + e3 * c1) * m01 + c1 * c3 * m00);
    moments[ind011] += C * (d2 * d3 * m20 + e2 * e3 * m02 + (d2 * e3 + d3 * e2) * m11 + (d2 * c3 + d3 * c2) * m10 + (e2 * c3 + e3 * c2) * m01 + c2 * c3 * m00);

    moments[ind300] += C * (d1 * d1 * d1 * m30 + e1 * e1 * e1 * m03 + 3 * d1 * d1 * e1 * m21 + 3 * d1 * e1 * e1 * m12 + 3 * d1 * d1 * c1 * m20 + 3 * e1 * e1 * c1 * m02 + 6 * d1 * e1 * c1 * m11 + 3 * d1 * c1 * c1 * m10 + 3 * e1 * c1 * c1 * m01 + c1 * c1 * c1 * m00);
    moments[ind030] += C * (d2 * d2 * d2 * m30 + e2 * e2 * e2 * m03 + 3 * d2 * d2 * e2 * m21 + 3 * d2 * e2 * e2 * m12 + 3 * d2 * d2 * c2 * m20 + 3 * e2 * e2 * c2 * m02 + 6 * d2 * e2 * c2 * m11 + 3 * d2 * c2 * c2 * m10 + 3 * e2 * c2 * c2 * m01 + c2 * c2 * c2 * m00);
    moments[ind003] += C * (d3 * d3 * d3 * m30 + e3 * e3 * e3 * m03 + 3 * d3 * d3 * e3 * m21 + 3 * d3 * e3 * e3 * m12 + 3 * d3 * d3 * c3 * m20 + 3 * e3 * e3 * c3 * m02 + 6 * d3 * e3 * c3 * m11 + 3 * d3 * c3 * c3 * m10 + 3 * e3 * c3 * c3 * m01 + c3 * c3 * c3 * m00);

    moments[ind012] += C * (d2 * d3 * d3 * m30 + e2 * e3 * e3 * m03 + (2 * d2 * d3 * e3 + d3 * d3 * e2) * m21 + (2 * d3 * e2 * e3 + d2 * e3 * e3) * m12 + (2 * d2 * d3 * c3 + d3 * d3 * c2) * m20 + (2 * e2 * e3 * c3 + e3 * e3 * c2) * m02 + (2 * d2 * e3 * c3 + 2 * d3 * e2 * c3 + 2 * d3 * e3 * c2) * m11 + (d2 * c3 * c3 + 2 * d3 * c2 * c3) * m10 + (e2 * c3 * c3 + 2 * e3 * c2 * c3) * m01 + c2 * c3 * c3 * m00);
    moments[ind102] += C * (d1 * d3 * d3 * m30 + e1 * e3 * e3 * m03 + (2 * d1 * d3 * e3 + d3 * d3 * e1) * m21 + (2 * d3 * e1 * e3 + d1 * e3 * e3) * m12 + (2 * d1 * d3 * c3 + d3 * d3 * c1) * m20 + (2 * e1 * e3 * c3 + e3 * e3 * c1) * m02 + (2 * d1 * e3 * c3 + 2 * d3 * e1 * c3 + 2 * d3 * e3 * c1) * m11 + (d1 * c3 * c3 + 2 * d3 * c1 * c3) * m10 + (e1 * c3 * c3 + 2 * e3 * c1 * c3) * m01 + c1 * c3 * c3 * m00);
    moments[ind021] += C * (d3 * d2 * d2 * m30 + e2 * e3 * e3 * m03 + (2 * d3 * d2 * e2 + d2 * d2 * e3) * m21 + (2 * d2 * e3 * e2 + d3 * e2 * e2) * m12 + (2 * d3 * d2 * c2 + d2 * d2 * c3) * m20 + (2 * e3 * e2 * c2 + e2 * e2 * c3) * m02 + (2 * d3 * e2 * c2 + 2 * d2 * e3 * c2 + 2 * d2 * e2 * c3) * m11 + (d3 * c2 * c2 + 2 * d2 * c3 * c2) * m10 + (e3 * c2 * c2 + 2 * e2 * c3 * c2) * m01 + c3 * c2 * c2 * m00);
    moments[ind201] += C * (d3 * d1 * d1 * m30 + e3 * e1 * e1 * m03 + (2 * d3 * d1 * e1 + d1 * d1 * e3) * m21 + (2 * d1 * e3 * e1 + d3 * e1 * e1) * m12 + (2 * d3 * d1 * c1 + d1 * d1 * c3) * m20 + (2 * e3 * e1 * c1 + e1 * e1 * c3) * m02 + (2 * d3 * e1 * c1 + 2 * d1 * e3 * c1 + 2 * d1 * e1 * c3) * m11 + (d3 * c1 * c1 + 2 * d1 * c3 * c1) * m10 + (e3 * c1 * c1 + 2 * e1 * c3 * c1) * m01 + c3 * c1 * c1 * m00);
    moments[ind120] += C * (d1 * d2 * d2 * m30 + e1 * e2 * e2 * m03 + (2 * d1 * d2 * e2 + d2 * d2 * e1) * m21 + (2 * d2 * e1 * e2 + d1 * e2 * e2) * m12 + (2 * d1 * d2 * c2 + d2 * d2 * c1) * m20 + (2 * e1 * e2 * c2 + e2 * e2 * c1) * m02 + (2 * d1 * e2 * c2 + 2 * d2 * e1 * c2 + 2 * d2 * e2 * c1) * m11 + (d1 * c2 * c2 + 2 * d2 * c1 * c2) * m10 + (e1 * c2 * c2 + 2 * e2 * c1 * c2) * m01 + c1 * c2 * c2 * m00);
    moments[ind210] += C * (d2 * d1 * d1 * m30 + e2 * e1 * e1 * m03 + (2 * d2 * d1 * e1 + d1 * d1 * e2) * m21 + (2 * d1 * e2 * e1 + d2 * e1 * e1) * m12 + (2 * d2 * d1 * c1 + d1 * d1 * c2) * m20 + (2 * e2 * e1 * c1 + e1 * e1 * c2) * m02 + (2 * d2 * e1 * c1 + 2 * d1 * e2 * c1 + 2 * d1 * e1 * c2) * m11 + (d2 * c1 * c1 + 2 * d1 * c2 * c1) * m10 + (e2 * c1 * c1 + 2 * e1 * c2 * c1) * m01 + c2 * c1 * c1 * m00);

    moments[ind111] += C * (d1 * d2 * d3 * m30 + e1 * e2 * e3 * m03 + (d1 * d2 * e3 + d1 * e2 * d3 + e1 * d2 * d3) * m21 + (d1 * e2 * e3 + e1 * d2 * e3 + e1 * e2 * d3) * m12 + (d1 * d2 * c3 + d1 * c2 * d3 + c1 * d2 * d3) * m20 + (e1 * e2 * c3 + e1 * c2 * e3 + c1 * e2 * e3) * m02 + (d1 * e2 * c3 + d1 * c2 * e3 + d2 * e1 * c3 + d2 * c1 * e3 + d3 * e1 * c2 + d3 * e2 * c1) * m11 + (d1 * c2 * c3 + c1 * d2 * c3 + c1 * c2 * d3) * m10 + (e1 * c2 * c3 + c1 * e2 * c3 + c1 * c2 * e3) * m01 + c1 * c2 * c3 * m00);
  }

// calculating 3D moment invariants following
// C.-H. Lo, H.-S. Don: 3-D Moment Forms: Their Construction and Application to Object Identification and Positioning, PAMI 11(10)

  void Moments3d::calcInvariants(std::vector< double >& invariants, bool scale) const
  {
    invariants.clear();

    const double m000 = moments[ind000];
    const double m100 = moments[ind100];
    const double m010 = moments[ind010];
    const double m001 = moments[ind001];
    const double m200 = moments[ind200];
    const double m020 = moments[ind020];
    const double m002 = moments[ind002];
    const double m011 = moments[ind011];
    const double m101 = moments[ind101];
    const double m110 = moments[ind110];
    const double m300 = moments[ind300];
    const double m030 = moments[ind030];
    const double m003 = moments[ind003];
    const double m012 = moments[ind012];
    const double m021 = moments[ind021];
    const double m102 = moments[ind102];
    const double m201 = moments[ind201];
    const double m120 = moments[ind120];
    const double m210 = moments[ind210];
    const double m111 = moments[ind111];

    const double J1 = m200 + m020 + m002;
    const double J2 =
      m200 * m020 + m200 * m002 + m020 * m002 -
      m101 * m101 - m110 * m110 - m011 * m011;
    const double J3 = m200 * m020 * m002 - m002 * m110 * m110 + 2 * m110 * m101 * m011 - m020 * m101 * m101 - m200 * m011 * m011;

    const double nu00 = 2 * sqrt(M_PI) / 3.0 * J1;
    const double nu2200 = 2 * M_PI / (15.0 * sqrt(5)) * (8 * J1 * J1 / 3.0 - 8 * J2);
    const double etanu = 2 * M_PI * sqrt(2 * M_PI) / (15.0 * sqrt(35)) * (-32 * J1 * J1 * J1 / 9.0 + 16 * J1 * J2 - 48 * J3);

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

    const std::complex<double> nu11(sqrt(6 * M_PI) / 5.0 * (-m300 - m120 - m102), -sqrt(6 * M_PI) / 5.0 * (m030 + m210 + m012));
    const std::complex<double> nu01(sqrt(12 * M_PI) / 5.0 * (m003 + m201 + m021), 0);
    const std::complex<double> nu_11(sqrt(6 * M_PI) / 5.0 * (m300 + m120 + m102), -sqrt(6 * M_PI) / 5.0 * (m030 + m210 + m012));

    const std::complex<double> nu22(sqrt(2 * M_PI / 15.0) * (m200 - m020), sqrt(2 * M_PI / 15.0) * 2 * m110);
    const std::complex<double> nu12(sqrt(2 * M_PI / 15.0) * (-2)*m101, sqrt(2 * M_PI / 15.0) * (-2)*m011);
    const std::complex<double> nu02(sqrt(4 * M_PI / 45.0) * (2 * m002 - m200 - m020), 0);
    const std::complex<double> nu_12(sqrt(2 * M_PI / 15.0) * 2 * m101, sqrt(2 * M_PI / 15.0) * (-2)*m011);
    const std::complex<double> nu_22(sqrt(2 * M_PI / 15.0) * (m200 - m020), sqrt(2 * M_PI / 15.0) * (-2)*m110);

    const std::complex<double> nu33(sqrt(M_PI / 35.0) * (-m300 + 3 * m120), sqrt(M_PI / 35.0) * (m030 - 3 * m210));
    const std::complex<double> nu23(sqrt(6 * M_PI / 35.0) * (m201 - m021), sqrt(6 * M_PI / 35.0) * (2 * m111));
    const std::complex<double> nu13(sqrt(3 * M_PI / 7.0) / 5.0 * (m300 + m120 - 4 * m102), sqrt(3 * M_PI / 7.0) / 5.0 * (m030 + m210 - 4 * m012));
    const std::complex<double> nu03((2 / 5.0)*sqrt(M_PI / 7.0) * (2 * m003 - 3 * m201 - 3 * m021), 0);
    const std::complex<double> nu_13(sqrt(3 * M_PI / 7.0) / 5.0 * (-m300 - m120 + 4 * m102), sqrt(3 * M_PI / 7.0) / 5.0 * (m030 + m210 - 4 * m012));
    const std::complex<double> nu_23(sqrt(6 * M_PI / 35.0) * (m201 - m021), sqrt(6 * M_PI / 35.0) * (-2 * m111));
    const std::complex<double> nu_33(sqrt(M_PI / 35.0) * (m300 - 3 * m120), sqrt(M_PI / 35.0) * (m030 - 3 * m210));

    const std::complex<double> nu3322 = sqrt(10 / 21.0) * nu33 * nu_13 - sqrt(20 / 21.0) * nu23 * nu03 + sqrt(2 / 7.0) * nu13 * nu13;
    const std::complex<double> nu3312 = 5 / sqrt(21.0) * nu33 * nu_23 - sqrt(5 / 7.0) * nu23 * nu_13 + sqrt(2 / 21.0) * nu13 * nu03;
    const std::complex<double> nu3302 = 5 / sqrt(21.0) * nu33 * nu_33 - sqrt(3 / 7.0) * nu13 * nu_13 + 2 / sqrt(21.0) * nu00 * nu00;
    const std::complex<double> nu33_12 = 5 / sqrt(21.0) * nu_33 * nu23 - sqrt(5 / 7.0) * nu_23 * nu13 + sqrt(2 / 21.0) * nu_13 * nu03;
    const std::complex<double> nu33_22 = sqrt(10 / 21.0) * nu_33 * nu13 - sqrt(20 / 21.0) * nu_23 * nu03 + sqrt(2 / 7.0) * nu_13 * nu_13;

    const std::complex<double> nu3300 = 1 / sqrt(7) * (2.0 * nu33 * nu_33 - 2.0 * nu23 * nu_23 + 2.0 * nu13 * nu_13 - nu03 * nu03);
    const std::complex<double> nu1100 = 1 / sqrt(3) * (2.0 * nu11 * nu_11 - nu01 * nu01);

    const std::complex<double> nu3122 = -1 / sqrt(105.0) * nu23 * nu01 + 1 / sqrt(35.0) * nu33 * nu_11 + (1 / 5.0) / sqrt(21.0) * nu13 * nu11;
    const std::complex<double> nu3112 = sqrt(2 / 105.0) * nu23 * nu_11 + (1 / 5.0) / sqrt(7.0) * nu03 * nu11 - (2 / 5.0) * sqrt(2 / 21.0) * nu13 * nu01;
    const std::complex<double> nu3102 = (-1 / 5.0) * sqrt(3 / 7.0) * nu03 * nu01 + (1 / 5.0) / sqrt(2 / 7.0) * nu13 * nu_11 + (1 / 5.0) / sqrt(2 / 7.0) * nu_13 * nu11;
    const std::complex<double> nu31_12 = sqrt(2 / 105.0) * nu_23 * nu11 + 1 / (5 * sqrt(7.0)) * nu03 * nu_11 - (2 / 5.0) * sqrt(2 / 21.0) * nu_13 * nu01;
    const std::complex<double> nu31_22 = -1 / sqrt(105) * nu_23 * nu01 + 1 / sqrt(35) * nu_33 * nu11 + 1 / (5 * sqrt(21.0)) * nu_13 * nu_11;

    const std::complex<double> nu1122 = nu11 * nu11 / 5.0;
    const std::complex<double> nu1112 = sqrt(2.0) / 5.0 * nu01 * nu11;
    const std::complex<double> nu1102 = sqrt(2 / 3.0) / 5.0 * (nu01 * nu01 + nu11 * nu_11);
    const std::complex<double> nu11_12 = sqrt(2) / 5.0 * nu01 * nu_11;
    const std::complex<double> nu11_22 = nu_11 * nu_11 / 5.0;

    const std::complex<double> nu332nu2 = 1 / sqrt(5.0) * (nu33_22 * nu22 - nu33_12 * nu12 + nu3302 * nu02 - nu3312 * nu_12 + nu3322 * nu_22);

    const std::complex<double> nu312nu2 = 1 / sqrt(5.0) * (nu31_22 * nu22 - nu31_12 * nu12 + nu3102 * nu02 - nu3112 * nu_12 + nu3122 * nu_22);

    const std::complex<double> nu112nu2 = 1 / sqrt(5.0) * (nu11_22 * nu22 - nu11_12 * nu12 + nu1102 * nu02 - nu1112 * nu_12 + nu1122 * nu_22);

    const std::complex<double> nu2332 = 1 / sqrt(5.0) * (nu33_22 * nu3322 - nu33_12 * nu3312 + nu3302 * nu3302 - nu3312 * nu33_12 + nu3322 * nu33_22);

    const std::complex<double> nu2312 = 1 / sqrt(5.0) * (nu31_22 * nu3122 - nu31_12 * nu3112 + nu3102 * nu3102 - nu3112 * nu31_12 + nu3122 * nu31_22);

    const std::complex<double> nu332nu312 = 1 / sqrt(5.0) * (nu33_22 * nu3122 - nu33_12 * nu3112 + nu3302 * nu3102 - nu3312 * nu31_12 + nu3322 * nu31_22);

    const std::complex<double> nu312nu112 = 1 / sqrt(5.0) * (nu31_22 * nu1122 - nu31_12 * nu1112 + nu3102 * nu1102 - nu3112 * nu11_12 + nu3122 * nu11_22);

    if (scale)
      {
        invariants.push_back(nu2200 / (pow(nu00, 2.0)));
        invariants.push_back(etanu / (pow(nu00, 3.0)));
        invariants.push_back(nu3300.real() / (pow(nu00, 12.0 / 5.0)));
        invariants.push_back(nu1100.real() / (pow(nu00, 12.0 / 5.0)));
        invariants.push_back(nu332nu2.real() / (pow(nu00, 17.0 / 5.0)));
        invariants.push_back(nu312nu2.real() / (pow(nu00, 17.0 / 5.0)));
        invariants.push_back(nu112nu2.real() / (pow(nu00, 17.0 / 5.0)));
        invariants.push_back(nu2332.real() / (pow(nu00, 24.0 / 5.0)));
        invariants.push_back(nu2312.real() / (pow(nu00, 24.0 / 5.0)));
        invariants.push_back(nu332nu312.real() / (pow(nu00, 24.0 / 5.0)));
        invariants.push_back(nu312nu112.real() / (pow(nu00, 24.0 / 5.0)));
      }
    else
      {
        invariants.push_back(nu2200);
        invariants.push_back(etanu);
        invariants.push_back(nu3300.real());
        invariants.push_back(nu1100.real());
        invariants.push_back(nu332nu2.real());
        invariants.push_back(nu312nu2.real());
        invariants.push_back(nu112nu2.real());
        invariants.push_back(nu2332.real());
        invariants.push_back(nu2312.real());
        invariants.push_back(nu332nu312.real());
        invariants.push_back(nu312nu112.real());

        invariants.push_back(m000);
        invariants.push_back(m100 * m100 + m010 * m010 + m001 * m001);
      }
  }
}//namespace

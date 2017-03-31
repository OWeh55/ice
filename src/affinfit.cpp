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

#include "macro.h"
#include "defs.h"
#include "message.h"

#include "root.h"
#include "momente.h"
#include "gentrans.h"
#include "matdef.h"
#include "lineqn.h"
#include "analygeo.h"
#include "trans.h"
#include "lmdif.h"

#include "affinfit.h"

namespace ice
{
  static int S_Aff_Moments(double s, double* norm_moments,
                           double* p_b, double* p_c, double* p_d);

  static int S_Sim_Moments(double s, double* norm_moments, double* p_b, double* p_c);

  static int root_moment_3(double moment[15], double& min_3);
  static int root_moment_4(double moment[15], double& min_4);

  static double  fkt_3(double moment[15], double x);
  static double  fkt_4(double moment[15], double x);

  static int NewtonRaphson(double moment[15], double x0, double eps, double(*f)(double*, double), double(*f_strich)(double*, double), double& root);

  static double f3(double m[15], double x);
  static double f3_strich(double m[15], double x);
  static double f4(double m[15], double x);
  static double f4_strich(double m[15], double x);


#define FNAME "FitCircleMoments"
  int FitCircleMoments(const double mp[15],
                       double& x0, double& y0, double& radius,
                       double& guetemass)
  {
    /************************************************************
     * Fitting eines Kreises an ein Objekt
     * Input: m[15] 15 area moments of the object
     * Output: x0,y0 center, radius is radius
     ************************************************************/

    double mx[15];
    double mtrans[15];
    double mtrop[15];

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return -1;
      }

    double xs, ys;
    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(mx, mtrans, xs, ys));
    x0 = -xs;
    y0 = -ys;

    //    cout << "FitCircleM" << x0 << ","<<y0<< endl;
    double alpha;
    RETURN_ERROR_IF_FAILED(NormalizeMomentsArea(mtrans, mtrop, alpha));
    radius = 1.0 / sqrt(M_PI) / alpha;

    /************************************************************************
     * Auf mtrop stehen die Momente des Objektes in Standardlage            *
     * m00=1, m10=m01=0                                                     *
     * beim Kreis sind m20 = m02 = 1 / (4 * PI)                             *
     ************************************************************************/

    double soll = 1.0 / (4.0 * M_PI);

    guetemass = sqrt(
                  (mtrop[3] - soll) * (mtrop[3] - soll)
                  + mtrop[4] * mtrop[4]
                  + (mtrop[5] - soll) * (mtrop[5] - soll));

    return OK;
  }

  int FitCircleMoments(const double mp[15],
                       double& x0, double& y0, double& radius)
  {
    /***********************************************************
     * Fitting eines Kreises an ein Objekt
     * Input: m[15]   15 area moments of the object
     * Output: x0,y0   central point of circle
     *         radius  radius of circle
     ***********************************************************/
    double guete;
    return FitCircleMoments(mp, x0, y0, radius, guete);
  }
#undef FNAME

  //----------------------------------------------------

#define FNAME "FitEllipseMoments"
  int FitEllipseMoments(const double mp[], double* ell_par)
  {
    /*************************************************************/
    /*  Fitting einer Ellipse an ein Objekt                      */
    /* Input: m[] 15 area moments of the object                  */
    /* Output: ell_par[] 5 parameter of an ellipse               */
    /*************************************************************/

    double ell_koef[6];
    double mx[15];
    double mtrans[15], det;
    int type;

    PosSign(mp, mx);

    if (mx[0] == 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return -1;
      }

    double sx, sy;

    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(mx, mtrans, sx, sy));

    det = mtrans[5] * mtrans[3] - mtrans[4] * mtrans[4];

    ell_koef[0] = mtrans[5];
    ell_koef[1] = mtrans[3];
    ell_koef[2] = 0;
    ell_koef[3] = 0;
    ell_koef[4] = -2 * mtrans[4];
    ell_koef[5] = -4 * det / mtrans[0];

    FeatureQuadrFunc(ell_koef, ell_par, &type);

    if (type != ELLIPSE)
      {
        Message(FNAME, M_NO_ELLIPSE, WRONG_PARAM);
        return WRONG_PARAM;
      }

    ell_par[0] = -sx;
    ell_par[1] = -sy;

    return OK;
  }
#undef FNAME

  //----------------------------------

#define FNAME "FitEllipticalSegmentsMoments"
  int FitEllipticalSegmentMoments(const double moments_object[15],
                                  double* ell_par,
                                  double* seg_par1, double* seg_par2)
  {
    double mom[15];
    PosSign(moments_object, mom);

    if (mom[0] == 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return (-1);
      }

    /****************************************************************/
    /*  Fitting eines Ellipsensegmentes an ein Objekt                */
    /****************************************************************/
    double s, sum, hilfe;
    double E_a, E_b, E_c, E_d, E_e, E_f;
    double b, c, d;
    double b_opt = 0.0, c_opt = 0.0, d_opt = 0.0;
    double s_min = 1.0E30;
    double s_opt = -0.99;
    double atr[3][3], atr_inv[3][3];
    double norm_moments[15];
    double moments_aff[15];
    double moments_h[15];
    double ell_koef[6];
    double p1[2], p2[2];
    double phi;
    int i, j, type;

    MatrixStruct ElS, E, ZW;
    MatrixStruct AF, AF_T, M1, M2;
    ElS = NewMatrix(MAT_DOUBLE, 3, 3);
    E = NewMatrix(MAT_DOUBLE, 3, 3);
    AF = NewMatrix(MAT_DOUBLE, 3, 3);
    AF_T = NewMatrix(MAT_DOUBLE, 3, 3);
    ZW = NewMatrix(MAT_DOUBLE, 3, 3);

    AffinNormMoments(mom, moments_aff, atr);

    CopyMoments(moments_aff, moments_h);

    phi = 0.0;

    if ((moments_aff[7] + moments_aff[9]) < 0.0)
      {
        phi = M_PI;
        RotateMoments(moments_h, cos(phi), sin(phi), moments_aff);
        RotTrans(0.0, 0.0, phi, atr);
      }

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        {
          AF->data[i][j] = atr[i][j];
        }

    TranspMat(AF, AF_T);

    for (s = -0.999; s <= 1.0; s += 0.001)
      {
        S_Aff_Moments(s, norm_moments, &b, &c, &d);
        sum = 0.0;

        for (i = 0; i < 15; ++i)
          {
            hilfe = norm_moments[i] - moments_aff[i];

            if (i == 0) hilfe = hilfe / 10.0;

            sum += hilfe * hilfe;
          }

        if (sum < s_min)
          {
            s_min = sum;
            s_opt = s;
            b_opt = b;
            c_opt = c;
            d_opt = d;
          }
      }

    ElS->data[0][0] = 1.0 / (c_opt * c_opt);
    ElS->data[0][1] = ElS->data[1][0] = 0.0;
    ElS->data[0][2] = ElS->data[2][0] = 0.0;
    ElS->data[1][1] = 1.0 / (d_opt * d_opt);
    ElS->data[1][2] = ElS->data[2][1] = -b_opt / d_opt;
    ElS->data[2][2] = b_opt * b_opt - 1.0;
    /* ElS ist die 3x3 Ellipsenmatrix zur Beschreibung der Ellipse des Standardellipsensegmentes , diese muß nun durch die affine Transformation dem Objekt überlagert werden  */
    MulMat(ElS, AF, ZW);
    MulMat(AF_T, ZW, E);
    /* Auf der 3x3 Matrix stehen die Koeffizienten der dem Objekt überlagerten Ellipse */
    E_a = E->data[0][0];
    E_b = 2 * E->data[0][1];
    E_c = E->data[1][1];
    E_d = 2 * E->data[0][2];
    E_e = 2 * E->data[1][2];
    E_f = E->data[2][2];
    ell_koef[0] = E_a;
    ell_koef[1] = E_c;
    ell_koef[2] = E_d;
    ell_koef[3] = E_e;
    ell_koef[4] = E_b;
    ell_koef[5] = E_f;
    FeatureQuadrFunc(ell_koef, ell_par, &type);
    p1[0] = c_opt * sqrt(1.0 - (s_opt * s_opt));
    p1[1] = (b_opt + s_opt) * d_opt;
    p2[0] = -p1[0];
    p2[1] = p1[1];
    M1 = NewMatrix(MAT_DOUBLE, 3, 3);
    M2 = NewMatrix(MAT_DOUBLE, 3, 3);

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        M1->data[i][j] = atr[i][j];

    InvertMat(M1, M2);

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        atr_inv[i][j] = M2->data[i][j];

    TransPoint(p1, atr_inv, seg_par1);
    TransPoint(p2, atr_inv, seg_par2);
    FreeMatrix(M1);
    FreeMatrix(M2);
    FreeMatrix(ElS);
    FreeMatrix(E);
    FreeMatrix(ZW);
    FreeMatrix(AF);
    FreeMatrix(AF_T);

    return OK;
  }

  // ******

  static int S_Aff_Moments(double s, double* norm_moments, double* p_b, double* p_c, double* p_d)
  {
    /* Berechnung der normierten Momente bez. aff. Transf. nach Standardmethode
       fuer vorgegebenes Kreissegment mit dem Parameter s */

    double xs, phi;
    double yc, b, c, d;
    double M00, /*M10,*/ M01, M20, /*M11,*/ M02, /*M30,*/ M21, /*M12,*/ M03, M40, /*M31,*/ M22, /*M13,*/ M04;
    double MS00, /*MS10, MS01,*/ MS20,/* MS11,*/ MS02, /*MS30,*/ MS21, /*MS12,*/ MS03, MS40, /*MS31,*/ MS22, /*MS13,*/ MS04;
    double MSC00, MSC10, MSC01, MSC20, MSC11, MSC02, MSC30, MSC21, MSC12, MSC03;
    double MSC40, MSC31, MSC22, MSC13, MSC04;

    double s2 = s * s;
    double s4 = s2 * s2;
    if (s2 >= 1.0)
      {
        xs = 0;
      }
    else
      {
        xs = sqrt(1.0 - s2);
      }

    phi = atan(s / xs);
    M00 = phi + M_PI / 2.0 + xs * s;
    //  M10 = 0.0;
    M01 = 2.0 / 3.0 * (-cos(phi) + s2 * xs);
    M20 = (1.0 / 8.0) * sin(2.0 * phi) + M_PI / 8.0 + phi / 4.0 + (xs * xs / 6.0) * xs * s;
    // M11 = 0.0;
    M02 = -(1.0 / 8.0) * sin(2.0 * phi) + M_PI / 8.0 + phi / 4.0 + (s * s / 2.0) * xs * s;
    // M30 = 0.0;
    M21 = -(2.0 / 15.0) * cos(phi) * cos(phi) * cos(phi) + (2.0 / 15.0) * xs * xs * xs * s2;
    // M12 = 0.0;
    M03 = -(2.0 / 5.0) * cos(phi) + (2.0 / 15.0) * cos(phi) * cos(phi) * cos(phi) + 2.0 / 5.0 * s4 * xs;
    M40 = sin(2.0 * phi) / 12.0 + sin(4.0 * phi) / 96.0 + 3.0 / 24.0 * phi + 3.0 / 48.0 * M_PI;
    M40 += xs * xs * xs * xs / 15.0 * xs * s;
    // M31 = 0.0;
    M22 = -sin(4.0 * phi) / 96.0 + phi / 24.0 + M_PI / 48.0;
    M22 += s2 * xs * xs * xs * s / 9.0;
    //    M13 = 0.0;
    M04 = -sin(2.0 * phi) / 12.0 + sin(4.0 * phi) / 96.0 + 3.0 / 24.0 * phi + 3.0 / 48.0 * M_PI;
    M04 += s4 * xs * s / 3.0;

    //    xc = 0.0;
    yc = M01 / M00;      /* Schwerpunkt xc,yc des Kreissegmentes  */

    if (yc >= 0)
      {
        b = yc;
      }
    else
      {
        b = -yc;  /* b ist Abstand von 0 zu yc */
      }

    *p_b = b;
    MS00 = M00;
    // MS10 = 0.0;
    // MS01 = 0.0;
    MS20 = M20;
    // MS11 = 0.0;
    MS02 = M02 + 2 * b * M01 + b * b * M00;
    //MS30 = 0.0;
    MS21 = M21 + b * M20;
    //MS12 = 0.0;
    MS03 = M03 + 3 * b * M02 + 3 * b * b * M01 + b * b * b * M00;
    MS40 = M40;
    //MS31 = 0.0;
    MS22 = M22 + 2 * b * M21 + b * b * M20;
    //MS13 = 0.0;
    MS04 = M04 + 4 * b * M03 + 6 * b * b * M02 + 4 * b * b * b * M01 + b * b * b * b * M00;
    /* Die Momente MS.. sind nun die auf den Schwerpunkt xc,yc des Kreissegmentes bezogenen Momente  */

    c = pow(MS02 / (MS20 * MS20 * MS20), 1.0 / 8.0);
    d = pow(MS20 / (MS02 * MS02 * MS02), 1.0 / 8.0);
    *p_c = c;
    *p_d = d;
    MSC00 = c * d * MS00;
    MSC10 = 0.0;
    MSC01 = 0.0;
    MSC20 = 1.0;
    MSC11 = 0.0;
    MSC02 = 1.0;
    MSC30 = 0.0;
    MSC21 = c * c * c * d * d * MS21;
    MSC12 = 0.0;
    MSC03 = c * d * d * d * d * MS03;
    MSC40 = c * c * c * c * c * d * MS40;
    MSC31 = 0.0;
    MSC22 = c * c * c * d * d * d * MS22;
    MSC13 = 0.0;
    MSC04 = c * d * d * d * d * d * MS04;
    norm_moments[0] = MSC00;
    norm_moments[1] = MSC10;
    norm_moments[2] = MSC01;
    norm_moments[3] = MSC20;
    norm_moments[4] = MSC11;
    norm_moments[5] = MSC02;
    norm_moments[6] = MSC30;
    norm_moments[7] = MSC21;
    norm_moments[8] = MSC12;
    norm_moments[9] = MSC03;
    norm_moments[10] = MSC40;
    norm_moments[11] = MSC31;
    norm_moments[12] = MSC22;
    norm_moments[13] = MSC13;
    norm_moments[14] = MSC04;
    /* Die Momente MSC.. sind nun die auf MSC20=1,MSC02=1 normierten Momente bezüglich einer anisotropen Skalierung mit den Faktoren c und d des Kreissegmentes, wobei der Schwerpunkt der Koordinatenursprung war */
    return OK;
  }
#undef FNAME

#define FNAME "FitCircularSegmentMoments"
  int FitCircularSegmentMoments(const double moments[15], double* circle_par, double* seg_par1, double* seg_par2)
  {
    double mom[15];
    PosSign(moments, mom);

    if (mom[0] == 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return ERROR;
      }

    /****************************************************************/
    /*  Fitting eines Kreissegmentes an ein Objekt                */
    /****************************************************************/
    /*                                                            */

    double s, sum, hilfe;
    double E_a, E_b, E_c, E_d, E_e, E_f;
    double phi1, phi;
    double tanphi1;
    double b, c, gamma;
    double p, q;

    double b_opt = 0.0, c_opt = 0.0;
    double s_min = 1.0E30;
    double s_opt = -0.99;
    double ctr[3][3], ctr_inv[3][3];
    double norm_moments[15];
    double moments_circle[15];
    double moments_sh[15];
    double moments_rot[15];
    double ell_koef[6], ell_par[5];
    double p1[2], p2[2];
    int i, j, type;
    MatrixStruct ElS, E, ZW;
    MatrixStruct AF, AF_T, M1, M2;
    ElS = NewMatrix(MAT_DOUBLE, 3, 3);
    E = NewMatrix(MAT_DOUBLE, 3, 3);
    AF = NewMatrix(MAT_DOUBLE, 3, 3);
    AF_T = NewMatrix(MAT_DOUBLE, 3, 3);
    ZW = NewMatrix(MAT_DOUBLE, 3, 3);

    double xs_o, ys_o;
    RETURN_ERROR_IF_FAILED(NormalizeMomentsTranslation(mom, moments_sh, xs_o, ys_o));

    /* Schwerpunkt (-xs_o,-ys_o) des Objektes bestimmt */
    InitTrans(ctr);
    ShiftTrans(xs_o, ys_o, ctr);

    if (fabs(moments_sh[4]) > 1.0e-12)
      {
        p = (moments_sh[3] - moments_sh[5]) / moments_sh[4];
        q = -1.0;
        tanphi1 = -p / 2.0 + sqrt(p * p / 4.0 - q);
        phi1 = atan(tanphi1);
      }
    else
      {
        phi1 = 0.0;
      }

    phi = phi1;
    RotateMoments(moments_sh, cos(phi), sin(phi), moments_rot);

    if (fabs(moments_rot[4]) > 1.0e-12)
      {
        phi = -phi1;
        RotateMoments(moments_sh, cos(phi), sin(phi), moments_rot);
      }

    for (i = 0; i < 3; ++i)
      {
        if (moments_rot[3] < moments_rot[5] || moments_rot[7] < 0.0)
          {
            phi = phi + M_PI / 2.0;
            RotateMoments(moments_rot, cos(M_PI / 2.0), sin(M_PI / 2.0), moments_rot);
          }
      }

    RotTrans(0.0, 0.0, phi, ctr);
    gamma = 1.0 / sqrt(moments_rot[0]);
    ScaleTrans(0.0, 0.0, gamma, gamma, ctr);
    ScaleMoments(moments_rot, gamma, gamma, moments_circle);

    /*                                                                    */
    /* Ende der Berechnung des "canonical frame" des Objektes             */
    /*******************************************************************/
    /* Es wurde vom Objekt die Standardlage bez. Ähnlichkeitstransf. ber. */
    /* Auf ctr steht die Transformation des Objektes auf die Standardlage */
    /* Auf moments_circle stehen die Momente der Standardlage             */
    /**********************************************************************/
    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        {
          AF->data[i][j] = ctr[i][j];
        }

    TranspMat(AF, AF_T);

    for (s = -0.999; s <= 1.0; s += 0.001)
      {
        S_Sim_Moments(s, norm_moments, &b, &c);
        sum = 0.0;

        for (i = 0; i < 15; ++i)
          {
            hilfe = norm_moments[i] - moments_circle[i];
            sum += hilfe * hilfe;
          }

        if (sum < s_min)
          {
            s_min = sum;
            s_opt = s;
            b_opt = b;
            c_opt = c;
          }
      }

    ElS->data[0][0] = 1.0 / (c_opt * c_opt);
    ElS->data[0][1] = ElS->data[1][0] = 0.0;
    ElS->data[0][2] = ElS->data[2][0] = 0.0;
    ElS->data[1][1] = 1.0 / (c_opt * c_opt);
    ElS->data[1][2] = ElS->data[2][1] = -b_opt / c_opt;
    ElS->data[2][2] = b_opt * b_opt - 1.0;
    /* ElS ist die 3x3 Ellipsenmatrix zur Beschreibung der Ellipse des Standardellipsensegmentes , diese muß nun durch die affine Transformation dem Objekt überlagert werden  */
    MulMat(ElS, AF, ZW);
    MulMat(AF_T, ZW, E);
    /* Auf der 3x3 Matrix stehen die Koeffizienten der dem Objekt überlagerten Ellipse */
    E_a = E->data[0][0];
    E_b = 2 * E->data[0][1];
    E_c = E->data[1][1];
    E_d = 2 * E->data[0][2];
    E_e = 2 * E->data[1][2];
    E_f = E->data[2][2];
    ell_koef[0] = E_a;
    ell_koef[1] = E_c;
    ell_koef[2] = E_d;
    ell_koef[3] = E_e;
    ell_koef[4] = E_b;
    ell_koef[5] = E_f;
    FeatureQuadrFunc(ell_koef, ell_par, &type);
    circle_par[0] = ell_par[0];
    circle_par[1] = ell_par[1];
    circle_par[2] = (ell_par[2] + ell_par[3]) / 2.0;
    p1[0] = c_opt * sqrt(1.0 - (s_opt * s_opt));
    p1[1] = (b_opt + s_opt) * c_opt;
    p2[0] = -p1[0];
    p2[1] = p1[1];
    M1 = NewMatrix(MAT_DOUBLE, 3, 3);
    M2 = NewMatrix(MAT_DOUBLE, 3, 3);

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        M1->data[i][j] = ctr[i][j];

    InvertMat(M1, M2);

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        ctr_inv[i][j] = M2->data[i][j];

    TransPoint(p1, ctr_inv, seg_par1);
    TransPoint(p2, ctr_inv, seg_par2);
    FreeMatrix(M1);
    FreeMatrix(M2);
    FreeMatrix(ElS);
    FreeMatrix(E);
    FreeMatrix(ZW);
    FreeMatrix(AF);
    FreeMatrix(AF_T);
    return OK;
  }

  static int S_Sim_Moments(double s, double* norm_moments, double* p_b, double* p_c)
  {
    /* Berechnung der normierten Momente bez. Ähnlichkeits. Transf.
       mit der Normierung
       m10=m01=0 für Translation
       m11=0     für Rotation
       m00=1     für isotrope Skalierung
       fuer vorgegebenes Kreissegment mit dem Parameter s */
    double xs, phi, xc, yc, b, c;
    double M00, M10, M01, M20, M11, M02, M30, M21, M12, M03, M40, M31, M22, M13, M04;
    double MS00, MS10, MS01, MS20, MS11, MS02, MS30, MS21, MS12, MS03, MS40, MS31, MS22, MS13, MS04;
    double MSC00, MSC10, MSC01, MSC20, MSC11, MSC02, MSC30, MSC21, MSC12, MSC03;
    double MSC40, MSC31, MSC22, MSC13, MSC04;

    if ((s == 1) || (s == -1))
      {
        xs = 0;
      }
    else
      {
        xs = sqrt(1.0 - s * s);
      }

    phi = atan(s / xs);
    M00 = phi + M_PI / 2.0 + xs * s;
    M10 = 0.0;
    M01 = 2.0 / 3.0 * (-cos(phi) + s * xs * s);
    M20 = (1.0 / 8.0) * sin(2.0 * phi) + M_PI / 8.0 + phi / 4.0 + (xs * xs / 6.0) * xs * s;
    M11 = 0.0;
    M02 = -(1.0 / 8.0) * sin(2.0 * phi) + M_PI / 8.0 + phi / 4.0 + (s * s / 2.0) * xs * s;
    M30 = 0.0;
    M21 = -(2.0 / 15.0) * cos(phi) * cos(phi) * cos(phi) + (2.0 / 15.0) * xs * xs * s * xs * s;
    M12 = 0.0;
    M03 = -(2.0 / 5.0) * cos(phi) + (2.0 / 15.0) * cos(phi) * cos(phi) * cos(phi) + 2.0 / 5.0 * s * s * s * xs * s;
    M40 = sin(2.0 * phi) / 12.0 + sin(4.0 * phi) / 96.0 + 3.0 / 24.0 * phi + 3.0 / 48.0 * M_PI;
    M40 += xs * xs * xs * xs / 15.0 * xs * s;
    M31 = 0.0;
    M22 = -sin(4.0 * phi) / 96.0 + phi / 24.0 + M_PI / 48.0;
    M22 += s * s * xs * xs * xs * s / 9.0;
    M13 = 0.0;
    M04 = -sin(2.0 * phi) / 12.0 + sin(4.0 * phi) / 96.0 + 3.0 / 24.0 * phi + 3.0 / 48.0 * M_PI;
    M04 += s * s * s * s * xs * s / 3.0;
    xc = 0.0;
    yc = M01 / M00;      /* Schwerpunkt xc,yc des Kreissegmentes  */

    if (yc >= 0)
      {
        b = yc;
      }
    else
      {
        b = -yc;  /* b ist Abstand von 0 zu yc */
      }

    *p_b = b;
    MS00 = M00;
    MS10 = 0.0;
    MS01 = 0.0;
    MS20 = M20;
    MS11 = 0.0;
    MS02 = M02 + 2 * b * M01 + b * b * M00;
    MS30 = 0.0;
    MS21 = M21 + b * M20;
    MS12 = 0.0;
    MS03 = M03 + 3 * b * M02 + 3 * b * b * M01 + b * b * b * M00;
    MS40 = M40;
    MS31 = 0.0;
    MS22 = M22 + 2 * b * M21 + b * b * M20;
    MS13 = 0.0;
    MS04 = M04 + 4 * b * M03 + 6 * b * b * M02 + 4 * b * b * b * M01 + b * b * b * b * M00;
    /* Die Momente MS.. sind nun die auf den Schwerpunkt xc,yc des Kreissegmentes bezogenen Momente  */
    /* MS11=0 ist automatisch für die Roatation erfüllt und braucht nicht mehr berücksichtigt zu werden */
    c = 1.0 / (sqrt(MS00));
    *p_c = c;
    MSC00 = 1.0;
    MSC10 = 0.0;
    MSC01 = 0.0;
    MSC20 = c * c * c * c * MS20;
    MSC11 = 0.0;
    MSC02 = c * c * c * c * MS02;
    MSC30 = 0.0;
    MSC21 = c * c * c * c * c * MS21;
    MSC12 = 0.0;
    MSC03 = c * c * c * c * c * MS03;
    MSC40 = c * c * c * c * c * c * MS40;
    MSC31 = 0.0;
    MSC22 = c * c * c * c * c * c * MS22;
    MSC13 = 0.0;
    MSC04 = c * c * c * c * c * c * MS04;
    norm_moments[0] = MSC00;
    norm_moments[1] = MSC10;
    norm_moments[2] = MSC01;
    norm_moments[3] = MSC20;
    norm_moments[4] = MSC11;
    norm_moments[5] = MSC02;
    norm_moments[6] = MSC30;
    norm_moments[7] = MSC21;
    norm_moments[8] = MSC12;
    norm_moments[9] = MSC03;
    norm_moments[10] = MSC40;
    norm_moments[11] = MSC31;
    norm_moments[12] = MSC22;
    norm_moments[13] = MSC13;
    norm_moments[14] = MSC04;
    /* Die Momente MSC.. sind nun die auf MSC20=1,MSC02=1 normierten Momente bezüglich einer anisotropen Skalierung mit den Faktoren c und d des Kreissegmentes, wobei der Schwerpunkt der Koordinatenursprung war */
    return OK;
  }
#undef FNAME

#define FNAME "FitSuperEllipseMoments"
  int FitSuperEllipseMoments(const double moments[], double& sup_c1, double& sup_f1, double sup_tr1[][3], double& sup_c2, double& sup_f2, double sup_tr2[][3])

  {
    /****************************************************************/
    /* Fitting einer Super Quadrik (in der Ebene ist dies eine      */
    /* Superellipse) an ein Objekt                                  */
    /****************************************************************/

    double mom[15];
    PosSign(moments, mom);

    if (mom[0] == 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return (-1);
      }

    int i, j;
    double c, s, h, s1_opt, s2_opt;
    double m00, m40, m22, m04;
    double qm00, qm20, qm40, qm22, qm04;
    double m1s00, m1s40, m1s22, m1s04;
    double m2s00, m2s40, m2s22, m2s04;
    double f1c, f2c, f1c_min, f2c_min, c1_min, c2_min;
    double maf[15];
    double atr[3][3];
    f1c_min = f2c_min = 1.0e+60;
    c1_min = c2_min = 0.05;
    AffinIterateMoments(mom, maf, atr);
    m00 = maf[0];
    m40 = maf[10];
    m22 = maf[12];
    m04 = maf[14];
    //printf(" m00 m40 m22 m04 %f %f %f %f\n",m00,m40,m22,m04);
    s1_opt = s2_opt = 0.01;

    /* Auf maf[] stehen die nach der Iterationsmethode normalisierten Momente des gegebenen Objektes */
    for (c = 0.01; c < 100; c += 0.01)
      {
        qm00 = 4.0 / (c * 2.0) * GammaFunction(1.0 / c) * GammaFunction(1.0 / c) / GammaFunction(2.0 / c);
        qm20 = 4.0 / (c * 4.0) * GammaFunction(3.0 / c) * GammaFunction(1.0 / c) / GammaFunction(4.0 / c);
        qm40 = 4.0 / (c * 6.0) * GammaFunction(5.0 / c) * GammaFunction(1.0 / c) / GammaFunction(6.0 / c);
        qm22 = 4.0 / (c * 6.0) * GammaFunction(3.0 / c) * GammaFunction(3.0 / c) / GammaFunction(6.0 / c);
        qm04 = 4.0 / (c * 6.0) * GammaFunction(1.0 / c) * GammaFunction(5.0 / c) / GammaFunction(6.0 / c);
        s = 1.0 / pow(qm20, 0.25);
        m1s00 = s * s * qm00;
        m1s40 = s * s * s * s * s * s * qm40;
        m1s22 = s * s * s * s * s * s * qm22;
        m1s04 = s * s * s * s * s * s * qm04;
        h = (m00 - m1s00);
        f1c = h * h;
        h = (m40 - m1s40);
        f1c += h * h;
        h = (m22 - m1s22);
        f1c += h * h;
        h = (m40 - m1s40);
        f1c += h * h;

        if (f1c < f1c_min)
          {
            f1c_min = f1c;
            c1_min = c;
            s1_opt = s;
          }

        /* m1spq sind normalisierte Momente, jetzt noch rotation um 45 Grad m2spq */
        m2s00 = m1s00;
        m2s40 = (m1s40 + 6 * m1s22 + m1s04) / 4.0;
        m2s22 = (m1s40 - 2 * m1s22 + m1s04) / 4.0;
        m2s04 = m2s40;
        h = (m00 - m2s00);
        f2c = h * h;
        h = (m40 - m2s40);
        f2c += h * h;
        h = (m22 - m2s22);
        f2c += h * h;
        h = (m40 - m2s40);
        f2c += h * h;

        if (f2c < f2c_min)
          {
            f2c_min = f2c;
            c2_min = c;
            s2_opt = s;
          }
      }

    sup_c1 = c1_min;
    sup_f1 = f1c_min;
    sup_c2 = c2_min;
    sup_f2 = f2c_min;

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        {
          sup_tr1[i][j] = atr[i][j];
        }

    ScaleTrans(0.0, 0.0, 1.0 / s1_opt, 1.0 / s1_opt, sup_tr1);
    RotTrans(0.0, 0.0, M_PI / 4.0, atr);

    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        {
          sup_tr2[i][j] = atr[i][j];
        }

    ScaleTrans(0.0, 0.0, 1.0 / s2_opt, 1.0 / s2_opt, sup_tr2);
    return OK;
  }
  // ************************************************************************

  int OrientationMoments(const double momente[15], double& angle)
  {
    double mom[15];
    double moment_t[15];
    double xs, ys;
    double m2norm = 0.0, m3norm = 0.0, m4norm = 0.0;
    double tan2phi;
    double wert1, wert2;
    double phi1, phi2;
    double scale;

    PosSign(momente, mom);

    if (mom[0] == 0.0 || mom[3] <= 0.0 ||
        mom[5] <= 0.0 || mom[10] <= 0.0 ||
        mom[12] <= 0.0 || mom[14] <= 0.0)
      {
        Message(FNAME, M_WRONG_OBJECT, WRONG_PARAM);
        return (-1);
      }

    xs = mom[1] / mom[0];
    ys = mom[2] / mom[0];

    TranslateMoments(mom, -xs, -ys, moment_t);

    // Momente auf m00=1 transformieren
    scale = 1.0 / sqrt(moment_t[0]);
    ScaleMoments(moment_t, scale, scale, moment_t);

    m2norm = (moment_t[3] - moment_t[5]) * (moment_t[3] - moment_t[5]) + 4.0 * moment_t[4] * moment_t[4];
    m2norm = m2norm / ((moment_t[3] + moment_t[5]) * (moment_t[3] + moment_t[5]));

    // Printf("m2norm-ice : %f \n",m2norm);
    //Momente zweiter Ordnung werden zur Winkelbestimmung benutzt
    if (m2norm > 0.01)
      {
        tan2phi = 2 * moment_t[4] / (moment_t[3] - moment_t[5]);
        phi1 = atan(tan2phi) / 2.0;

        if (phi1 < 0.0) phi1 += M_PI / 2.0;

        wert1 = moment_t[3] * cos(phi1) * cos(phi1);
        wert1 += 2.0 * moment_t[4] * cos(phi1) * sin(phi1);
        wert1 += moment_t[5] * sin(phi1) * sin(phi1);
        phi2 = phi1 + M_PI / 2.0;
        wert2 = moment_t[3] * cos(phi2) * cos(phi2);
        wert2 += 2.0 * moment_t[4] * cos(phi2) * sin(phi2);
        wert2 += moment_t[5] * sin(phi2) * sin(phi2);

        if (wert1 < wert2)
          {
            angle = phi1;
          }
        else
          {
            angle = phi2;
          }

        return 1;
      }

    if (m2norm <= 0.01)
      {
        //Momente dritter Ordnung werden benutzt
        m3norm = (moment_t[6] - 3.0 * moment_t[8]) * (moment_t[6] - 3.0 * moment_t[8]);
        m3norm += (3.0 * moment_t[7] - moment_t[9]) * (3.0 * moment_t[7] - moment_t[9]);
        m3norm = sqrt(m3norm);

        //Printf("m3norm-ice : %f \n",m3norm);
        if (m3norm > 0.01)
          {
            root_moment_3(moment_t, angle);
            return 2;
          }
      }

    if (m3norm <= 0.01)
      {
        // Momente vierter Ordnung werden benutzt
        m4norm = (moment_t[10] - moment_t[14]) * (moment_t[10] - moment_t[14]);
        m4norm += 4.0 * (3 * moment_t[12] - moment_t[10]) * (3.0 * moment_t[12] - moment_t[14]);
        m4norm += 16.0 * (moment_t[11] - moment_t[13]) * (moment_t[11] - moment_t[13]);
        m4norm = m4norm / ((moment_t[10] + 2.0 * moment_t[12] + moment_t[14]) * (moment_t[10] + 2.0 * moment_t[12] + moment_t[14]));

        //Printf("m4norm-ice : %f \n",m4norm);
        if (m4norm > 0.001)
          {
            root_moment_4(moment_t, angle);
            return 3;
          }
      }

    if (m4norm <= 0.001)
      {
        // Angle cannot be determined
        return -1;
      }

    return -1;
  }

  // ********************************************************
  int root_moment_3(double moment[15], double& phi_3)
  {
    double min = 1.0e30;
    double phi, fkt, x;
    double tan_phi;
    double h;
    int i;
    phi_3 = 0.0;
    double root_tan;

    for (i = 0; i < 720; ++i)
      {
        x = double(i) / 2.0;
        phi = x / 180.0 * M_PI;
        fkt = fkt_3(moment, phi);

        if (fkt < min)
          {
            min = fkt;
            phi_3 = phi;
          }
      }

    tan_phi = tan(phi_3);
    NewtonRaphson(moment, tan_phi, 1.0e-10, f3, f3_strich, root_tan);

    if (root_tan < 0) root_tan = -root_tan;

    h = atan(root_tan);

    if (fabs((h - phi_3) / M_PI * 180.0) < 1.0) phi_3 = h;

    return 0;
  }

  int root_moment_4(double moment[15], double& phi_4)
  {
    double min = 1.0e30;
    double phi, fkt, x;
    double tan_phi;
    double root_tan;
    double h;
    int i;
    phi_4 = 0.0;

    for (i = 0; i < 360; ++i)
      {
        x = double(i) / 2.0;
        phi = x / 180.0 * M_PI;
        fkt = fkt_4(moment, phi);

        if (fkt < min)
          {
            min = fkt;
            phi_4 = phi;
          }

      }

    tan_phi = tan(phi_4);
    NewtonRaphson(moment, tan_phi, 1.0e-10, f4, f4_strich, root_tan);

    if (root_tan < 0) root_tan = -root_tan;

    h = atan(root_tan);

    if (fabs((h - phi_4) / M_PI * 180.0) < 0.5) phi_4 = h;


    return 0;
  }

  // ******************************************

  double  fkt_3(double moment[15], double x)
  {
    double fkt;
    fkt = +moment[6] * cos(x) * cos(x) * cos(x);
    fkt += +3.0 * moment[7] * cos(x) * cos(x) * sin(x);
    fkt += +3.0 * moment[8] * cos(x) * sin(x) * sin(x);
    fkt += +moment[9] * sin(x) * sin(x) * sin(x);
    return fkt;
  }

  double  fkt_4(double moment[15], double x)
  {
    double fkt;
    fkt = +moment[10] * cos(x) * cos(x) * cos(x) * cos(x);
    fkt += +4.0 * moment[11] * cos(x) * cos(x) * cos(x) * sin(x);
    fkt += +6.0 * moment[12] * cos(x) * cos(x) * sin(x) * sin(x);
    fkt += +4.0 * moment[13] * cos(x) * sin(x) * sin(x) * sin(x);
    fkt += +moment[14] * sin(x) * sin(x) * sin(x) * sin(x);
    return fkt;
  }

  // ********************************************

  int NewtonRaphson(double moment[15],
                    double x0, double eps,
                    double(*f)(double*, double),
                    double(*f_strich)(double*, double),
                    double& root)
  {
    double delta;
    int i = 0;

    root = x0;

    do
      {
        i++;

        if (i > 1000)
          {
            root = x0;
            return 0;
          }

        double derive = f_strich(moment, root);

        if (fabs(derive) < 1.0e-12)
          {
            root = x0;
            return 0;
          }

        delta = -f(moment, root) / derive;
        root += delta;
      }
    while (fabs(delta) > eps);

    return 0;
  }

  // *****************************
  double f3(double m[15], double x)
  {
    double y;
    y = -m[8] * x + m[9] - 2.0 * m[7];
    y = y * x + 2 * m[8] - m[6];
    y = y * x + m[7];
    return y;
  }
  double f3_strich(double m[15], double x)
  {
    double y;
    y = -3.0 * m[8] * x + 2.0 * (m[9] - 2.0 * m[7]);
    y = y * x + 2.0 * m[8] - m[6];
    return y;
  }

  double f4(double m[15], double x)
  {
    double y;
    y = -m[13] * x + m[14] - 3.0 * m[12];
    y = y * x + 3 * m[13] - 3.0 * m[11];
    y = y * x + 3.0 * m[12] - m[10];
    y = y * x + m[11];
    return y;
  }
  double f4_strich(double m[15], double x)
  {
    double y;
    y = -4.0 * m[13] * x + 3.0 * (m[14] - 3.0 * m[12]);
    y = y * x + 2.0 * (3.0 * m[13] - 3.0 * m[11]);
    y = y * x + 3.0 * m[12] - m[10];
    return y;
  }
#undef FNAME
}

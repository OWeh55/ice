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

/*
 * selfcalib.cpp  planar self calibration (zhang)
 *
 * Michael Schwarz 04/2007
 * Ortmann 08/2012, 04/2017
 */

#include <float.h>
#include <algorithm>

#include "affinfit.h"
#include "analygeo.h"
#include "assignment.h"
#include "Camera.h"
#include "contfeat.h"
#include "glinefit.h"
#include "hist.h"
#include "drawline.h"
#include "lseg.h"
#include "mateigen.h"
#include "paint.h"
#include "segment1.h"
#include "simplex.h"
#include "threshld.h"
#include "filter.h"
#include "polygonim.h"
#include "distance.h"
#include "fitmoments.h"
#include "MatrixAlgebra.h"
#include "Contur.h"
#include "conturfunctions.h"
#include "Distortion1.h"
#include "Distortion2.h"
#include "Distortion3.h"

#define selfcalib_debug

#include "selfcalib.h"

namespace ice
{

// handling of border - copy from s to p
  void copyborder(const Image& s, const Image& p, int size)
  {
    int i, j, val;

    // Rand fuellen
    for (i = 0; i < (p->xsize - 1); i++)
      {
        for (j = 0; j < size; j++)
          {
            val = GetValUnchecked(s, i, 0 + j);
            PutValUnchecked(p, i, 0 + j, val);
            val = GetValUnchecked(s, i, (p->ysize - 1) - j);
            PutValUnchecked(p, i, (p->ysize - 1) - j, val);
          }
      }

    for (i = 0; i < (p->ysize - 1); i++)
      {
        for (j = 0; j < size; j++)
          {
            val = GetValUnchecked(s, 0 + j, i);
            PutValUnchecked(p, 0 + j, i, val);
            val = GetValUnchecked(s, (p->xsize - 1) - j, i);
            PutValUnchecked(p, (p->xsize - 1) - j, i, val);
          }
      }
  }

#define FNAME "project_point"
// Projektion eines Punktes aus der Welt in das Bild
  void project_point(const double world[], const CameraParameters& cp, double image[])
  {
    double cc[3];
    cc[0] = world[0] * cp.R[0][0] + world[1] * cp.R[0][1] + world[2] * cp.R[0][2] + cp.t[0];
    cc[1] = world[0] * cp.R[1][0] + world[1] * cp.R[1][1] + world[2] * cp.R[1][2] + cp.t[1];
    cc[2] = world[0] * cp.R[2][0] + world[1] * cp.R[2][1] + world[2] * cp.R[2][2] + cp.t[2];

    double sc[3];
    sc[0] = cp.K[0][0] * cc[0] + cp.K[0][1] * cc[1] + cp.K[0][2] * cc[2];
    sc[1] = cp.K[1][0] * cc[0] + cp.K[1][1] * cc[1] + cp.K[1][2] * cc[2];
    sc[2] = cp.K[2][0] * cc[0] + cp.K[2][1] * cc[1] + cp.K[2][2] * cc[2];

    if (sc[2] != 0.0)
      {
        image[0] = sc[0] / sc[2];
        image[1] = sc[1] / sc[2];
      }
  }
#undef FNAME

#define FNAME "Orthonormalize"
  Matrix Orthonormalize(const Matrix& A)
  {
    Matrix U, S, V;
    SingularValueDcmp(A, U, S, V);

    for (int i = 0; i < S.cols(); i++)
      {
        S[i][i] = 1.0;
      }

    return U * S * (!V);
  }
#undef FNAME

#define FNAME "calib_homography_zhang"
// Berechnung der Homographie aus Punktkorrespondenzen
// image -> punktliste bild
// world -> punktliste vorgabe
  Matrix calib_homography_zhang(int nPoints, Matrix& image, Matrix& world)
  {
    int nRows = 2 * nPoints;

    if (nRows < 9)
      {
        nRows = 9;
      }

    Matrix A(nRows, 9);
    int equationNr = 0;

    for (int i = 0; i < nPoints; i++)
      {
        // Zhang has the things the other way round... anyway...
        A[equationNr][0] = 0;
        A[equationNr][1] = 0;
        A[equationNr][2] = 0;
        A[equationNr][3] = -world[i][0];
        A[equationNr][4] = -world[i][1];
        A[equationNr][5] = -1;
        A[equationNr][6] = image[i][1] * world[i][0];
        A[equationNr][7] = image[i][1] * world[i][1];
        A[equationNr][8] = image[i][1];

        equationNr++;
        A[equationNr][0] = world[i][0];
        A[equationNr][1] = world[i][1];
        A[equationNr][2] = 1;
        A[equationNr][3] = 0;
        A[equationNr][4] = 0;
        A[equationNr][5] = 0;
        A[equationNr][6] = -image[i][0] * world[i][0];
        A[equationNr][7] = -image[i][0] * world[i][1];
        A[equationNr][8] = -image[i][0];
        equationNr++;
      }

    for (; equationNr < nRows; equationNr++)
      {
        for (int i = 0; i < 9; i++)
          {
            A[equationNr][i] = 0.0;
          }
      }

    // compute SVD
    Matrix U, V;
    Vector s;
    SingularValueDcmp(A, U, s, V);

    double min = s[0];
    int min_col = 0;
    for (int i = 1; i < 9; i++)
      {
        if (s[i] < min)
          {
            min = s[i];
            min_col = i;
          }
      }

    // create H
    Matrix H(3, 3);
    H[0][0] = V[0][min_col] / V[8][min_col];
    H[0][1] = V[1][min_col] / V[8][min_col];
    H[0][2] = V[2][min_col] / V[8][min_col];
    H[1][0] = V[3][min_col] / V[8][min_col];
    H[1][1] = V[4][min_col] / V[8][min_col];
    H[1][2] = V[5][min_col] / V[8][min_col];
    H[2][0] = V[6][min_col] / V[8][min_col];
    H[2][1] = V[7][min_col] / V[8][min_col];
    H[2][2] = V[8][min_col] / V[8][min_col];

//    cout << H << endl;
    return H;
  }
#undef FNAME

#define FNAME "calib_intrinsic_zhang"
// Berechnung der internen Kameraparametern nach Zhang

  void makeEquation(Vector& es, const Trafo& homographie, int z_i, int z_j)
  {
    Matrix T(homographie.getMatrix());
    es[0] = T[0][z_i] * T[0][z_j];
    es[1] = T[0][z_i] * T[1][z_j] + T[1][z_i] * T[0][z_j];
    es[2] = T[0][z_i] * T[2][z_j] + T[2][z_i] * T[0][z_j];
    es[3] = T[1][z_i] * T[1][z_j];
    es[4] = T[1][z_i] * T[2][z_j] + T[2][z_i] * T[1][z_j];
    es[5] = T[2][z_i] * T[2][z_j];
  }

  Matrix calib_intrinsic_zhang(const vector<Trafo>& H)
  {
    int nHomographies = H.size();
    Matrix C(2 * nHomographies, 6, 1);

    int equationNr = 0;
    for (int k = 0; k < nHomographies; k++)
      {
        // second row is v11 - v22
        makeEquation(C[equationNr + 1], H[k], 0, 0);
        makeEquation(C[equationNr], H[k], 1, 1);
        C[equationNr + 1] = C[equationNr + 1] - C[equationNr];
        // first row is v12
        makeEquation(C[equationNr], H[k], 0, 1);
        equationNr += 2;
      }

    Matrix U, V;
    Vector s;
    SingularValueDcmp(C, U, s, V);

    double min = s[0];
    int min_col = 0;

    for (int i = 1; i < 6; i++)
      {
        if (s[i] < min)
          {
            min = s[i];
            min_col = i;
          }
      }

    // create B
    Matrix B(3, 3);
    B[0][0] = V[0][min_col] / V[5][min_col];
    B[0][1] = B[1][0] = V[1][min_col] / V[5][min_col];
    B[0][2] = B[2][0] = V[2][min_col] / V[5][min_col];
    B[1][1] = V[3][min_col] / V[5][min_col];
    B[1][2] = B[2][1] = V[4][min_col] / V[5][min_col];
    B[2][2] = V[5][min_col] / V[5][min_col];

    // compute A from B
    if (!isPositiveDefinite(B))
      throw IceException(FNAME, M_INTERN);

    Matrix A = CholeskyDecomposition(B);

    A = Inverse(!A);
    A = A * (1.0 / A[2][2]);

    return A;
  }
#undef FNAME

#define FNAME "calib_extrinsic_zhang"
// Berechnung der externen Kameraparameter nach Zhang
  CameraParameters calib_extrinsic_zhang(const Matrix& H, const Matrix& K)
  {
    CameraParameters ret;
    Matrix A = Inverse(K);
    ret.R = Matrix(3, 3, 1);
    ret.K = Matrix(K);
    ret.t = Vector(3);

    Vector h(3);

    for (int i = 0; i < 3; i++)
      {
        h[i] = H[i][0];
      }

    Vector r = A * h;
    double lambda = 1.0 / r.Length();

    for (int i = 0; i < 3; i++)
      {
        ret.R[i][0] = r[i];
      }

    for (int i = 0; i < 3; i++)
      {
        h[i] = H[i][1];
      }

    r = A * h;

    for (int i = 0; i < 3; i++)
      {
        ret.R[i][1] = r[i];
      }

    // in theory r1 and r2 have the same length... we take the mean anyway
    lambda = (lambda + (1.0 / r.Length())) / 2.0;
    ret.R = ret.R * lambda;

    ret.R[0][2] = ret.R[1][0] * ret.R[2][1] - ret.R[2][0] * ret.R[1][1];
    ret.R[1][2] = ret.R[2][0] * ret.R[0][1] - ret.R[0][0] * ret.R[2][1];
    ret.R[2][2] = ret.R[0][0] * ret.R[1][1] - ret.R[1][0] * ret.R[0][1];
    ret.R = Orthonormalize(ret.R);

    for (int i = 0; i < 3; i++)
      {
        h[i] = H[i][2];
      }

    ret.t = A * h;

    ret.t = ret.t * lambda;

    return ret;
  }
#undef FNAME

  int dimx   = 0;
  int dimy   = 0;
  int dimmax = 0;

#define FNAME "point_set_distance"
// Berechnung der Punktabstände
  double point_set_distance(const Matrix& m1, const Matrix& m2)
  {
    int N1, N2;
    int i, j;
    double sum1, sum2;
    double dist;
    double h;
    double x, y, u, v;
    N1 = m1.rows();
    N2 = m2.rows();
    sum1 = 0.0;

    for (i = 0; i < N1; i++)
      {
        x = m1[i][0];
        y = m1[i][1];
        dist = DBL_MAX;

        for (j = 0; j < N2; j++)
          {
            u = m2[j][0];
            v = m2[j][1];
            h = sqrt((x - u) * (x - u) + (y - v) * (y - v));

            if (h < dist)
              {
                dist = h;
              }
          }

        sum1 += dist;
      }

    sum1 /= double(N1);

    sum2 = 0.0;

    for (i = 0; i < N2; i++)
      {
        x = m2[i][0];
        y = m2[i][1];
        dist = DBL_MAX;

        for (j = 0; j < N1; j++)
          {
            u = m1[j][0];
            v = m1[j][1];
            h = sqrt((x - u) * (x - u) + (y - v) * (y - v));

            if (h < dist)
              {
                dist = h;
              }
          }

        sum2 += dist;
      }

    sum2 /= double(N2);

    if (sum1 < sum2)
      {
        return sum2;
      }
    else
      {
        return sum1;
      }
  }
#undef FNAME

#define FNAME "scan_image"
// Ermittlung der Bildkoordinaten des Musters aus einem Bild
  int scan_image(Image& image, int& number_of_points,
                 PointList& pl1, PointList& pl2,
                 IMatrix& ref_pairs,
                 Image& debug_image, Image& debug_mark)
  {
    dimx   = image->xsize;
    dimy   = image->ysize;
    dimmax = dimx < dimy ? dimy : dimx;

    int max_gw = image.maxval;

    int number_of_found_objects;
    int number_of_searched_objects = CPOINTS;
    double x1, y1, x2, y2, x3, y3, x4, y4;
    Contur c;
    int ps[2];

    //    double t_p[4][2];
    double length, area, form, conv;
    double pc[3];
    double punkte_liste[1000][2];

    double sum1, sum2;
    double hx1, hx2, hy1, hy2;
    double min_cost;
    double phi = 0.0;
    double set_distance;
    int i, j, k, l, M, M_MIN;
    int TRANSFORMATION = TRM_PROJECTIVE;

    Trafo t1;

    Matrix h_matrix(0, 2);

    Image pic1, pic2, mark1;

    pic1 = NewImg(image);
    pic2 = NewImg(image);
    CopyImg(image, pic1);
    CopyImg(image, pic2);
    smearImg(image, pic2, 5, 5);
    copyborder(image, pic2, 2);

    mark1 = NewImg(dimx, dimy, max_gw);
    clearImg(mark1);

    double abstand = 0.0;        // Abstand der Muster in Bezug abhaengig von Pixelzahl und Bildrichtung
    double verschiebung_x = 0.0; // Verschiebung, damit Muster in der Mitte liegt
    double verschiebung_y = 0.0; // Verschiebung, damit Muster in der Mitte liegt

    if (dimx > dimy)
      {
        if (((double)dimx / 10.0) < ((double) dimy / 7.0))
          {
            abstand = (double)dimx / 10.0;
            verschiebung_y = (dimy / 2.0) - (3.5 * abstand);
          }
        else
          {
            abstand = (double)dimy / 7.0;
            verschiebung_x = (dimx / 2.0) - (5.0 * abstand);
          }
      }
    else
      {
        if (((double)dimx / 7.0) < ((double) dimy / 10.0))
          {
            abstand = (double)dimx / 7.0;
            verschiebung_y = (dimy / 2.0) - (5.0 * abstand);
          }
        else
          {
            abstand = (double)dimy / 10.0;
            verschiebung_x = (dimx / 2.0) - (3.5 * abstand);
          }
      }

    for (i = 0; i < number_of_searched_objects; ++i)
      {
        // Muster in die richtige Richtung drehen
        if (dimx > dimy)
          {
            y1 = pattern[i][0];
            x1 = pattern[i][1];
            y2 = pattern[i][2];
            x2 = pattern[i][3];
            y3 = pattern[i][4];
            x3 = pattern[i][5];
            y4 = pattern[i][6];
            x4 = pattern[i][7];
          }
        else
          {
            x1 = pattern[i][0];
            y1 = pattern[i][1];
            x2 = pattern[i][2];
            y2 = pattern[i][3];
            x3 = pattern[i][4];
            y3 = pattern[i][5];
            x4 = pattern[i][6];
            y4 = pattern[i][7];
          }

        // Musterbild hat 7 Spalten (X-Richtung)
        // Musterbild hat 10 Zeilen (Y-Richtung)
        // Musterbild hat 50 Objekte

        // Muster in die richtige Richtung drehen
        if (dimx > dimy)
          {
            pc[0] = ((x1 + x2 + x3 + x4) / 4.0) * abstand + verschiebung_x;
            pc[1] = (double)dimy - ((y1 + y2 + y3 + y4) / 4.0) * abstand - verschiebung_y; // in Y-Richtung spiegeln
          }
        else
          {
            pc[0] = ((x1 + x2 + x3 + x4) / 4.0) * abstand + verschiebung_x;
            pc[1] = ((y1 + y2 + y3 + y4) / 4.0) * abstand + verschiebung_y;
          }

        punkte_liste[i][0] = pc[0];
        punkte_liste[i][1] = pc[1];
      }

    pl1 = NewPointList(number_of_searched_objects);

    for (i = 0; i < number_of_searched_objects; ++i)
      {
        pl1->xptr[i] = punkte_liste[i][0];
        pl1->yptr[i] = punkte_liste[i][1];
        pl1->wptr[i] = 1.0;
      }

    // Punktlist 1 = pl1 ist fertig erstellt!
    // Jetz mit Konturfolge Objekte bestimmen

    ps[0] = 0;
    ps[1] = 0;
    //threshold = CalcThreshold(pic1);
    number_of_found_objects = 0;

    // Lokale Kontursuche
    int neighb = (int)(dimmax / 100.0);

    if (neighb % 2 == 0)
      {
        neighb++;
      }

    LocalSeg(pic2, mark1, neighb, 30);

    //while (SearchStart(pic1, mark1, NULL, threshold, 1, ps, HORZ) == OK)
    while (SearchStart(mark1, Image(), LocalSegObj, 0, 1, ps, HORZ) == OK)
      {
        //c = CalcContur(pic1, mark1, NULL, threshold, ps, 0);
        c = CalcContur(mark1, Image(), LocalSegObj, 0, ps, 0);

        if (c.isValid())
          {
            MarkContur(c, 3, mark1);

            if (c.isClosed())
              {
                FeatureContur(c, length, area, form, conv);

                if (area > 0.0)
                  {
                    FillRegion(c, 3, mark1);
                  }

                // wenn Objektgroesse oder Form nicht stimmt wird das Objekt nicht benutzt
                if (area > ((dimx * dimy) /  100.0))
                  {
                    continue;
                  }

                if (area < ((dimx * dimy) / 8000.0))
                  {
                    continue;
                  }

                /*
                  CopyImg(mark1,debug_mark);
                  printf("area: %f (%f..%f)   form: %f   conv: %f\n",
                  area, ((dimx * dimy) / 8000.0),(dimx * dimy) /  100.0,
                  form, conv);
                  getchar();
                */
                if (form > 2.9)
                  {
                    continue;
                  }

                Moments mom(c);
                Matrix m = ReducePolygon(c, 4);
                m = FitPolygon(mom, m);

                Point p1(m[0]);
                Point p2(m[1]);
                Point p3(m[2]);
                Point p4(m[3]);

                // Längen der Seiten
                double l1 = (p1 - p2).Length();
                double l2 = (p2 - p3).Length();
                double l3 = (p3 - p4).Length();
                double l4 = (p4 - p1).Length();

                // Längen der Diagonalen
                double l5 = (p1 - p3).Length();
                double l6 = (p2 - p4).Length();

                // wenn Objektgroesse oder Form nicht stimmt wird das Objekt nicht benutzt
                if (fabs(l1 - l3) > ((l1 + l3) * 0.2))
                  {
                    continue;
                  }

                if (fabs(l2 - l4) > ((l2 + l4) * 0.2))
                  {
                    continue;
                  }

                if (fabs(l5 - l6) > ((l5 + l6) * 0.7))
                  {
                    continue;
                  }

                Vector intersection;

//    cout << m << endl;

                if (Intersect(m[0], m[2],
                              m[1], m[3],
                              intersection))
                  {
//      cout << "Schnittpunkt: " << intersection << endl;
                    h_matrix.append(intersection);
                    ++number_of_found_objects;
                  }
              }
          }
      }

    // wenn mehr als 50 Objekte, dann nach Abstandskriterium raus werfen
    if (number_of_found_objects > number_of_searched_objects)
      {

#ifdef selfcalib_debug
        {
          cout << image->getTitle() << endl;
          printf("Anzahl gefundener Objekte: %d\n", number_of_found_objects);
          printf("Korrektur ueber Abstand zum naechsten Nachbarn\n");
        }
#endif

        double distance = 0.0;
        double* matrix_of_min_distance =
          (double*)malloc(number_of_found_objects * sizeof(double));
        double* sort_matrix_of_min_distance =
          (double*)malloc(number_of_found_objects * sizeof(double));

        for (i = 0; i < number_of_found_objects; i++)
          {
            matrix_of_min_distance[i] = dimmax;

            for (j = 0; j < number_of_found_objects; j++)
              {
                if (i != j)
                  {
                    distance = Distance(h_matrix[i][0], h_matrix[i][1], h_matrix[j][0], h_matrix[j][1]);

                    if (distance < matrix_of_min_distance[i])
                      {
                        matrix_of_min_distance[i] = distance;
                      }
                  }
              }
          }

        distance = 0.0;

        for (i = 0; i < number_of_found_objects; i++)
          {
            sort_matrix_of_min_distance[i] = matrix_of_min_distance[i];
          }

        sort(sort_matrix_of_min_distance,
             sort_matrix_of_min_distance + number_of_found_objects);

        distance = sort_matrix_of_min_distance[number_of_found_objects / 2];

        //printf("distance: %f\n", distance);

        for (i = 0; i < number_of_found_objects; i++)
          {
            if (matrix_of_min_distance[i] > distance * 1.50 ||
                matrix_of_min_distance[i] < distance * 0.75)
              {
                number_of_found_objects--;

                for (j = i; j < number_of_found_objects; j++)
                  {
                    h_matrix[j][0] = h_matrix[j + 1][0];
                    h_matrix[j][1] = h_matrix[j + 1][1];
                  }

                //          h_matrix[number_of_found_objects + 1][0] = 0.0;
                //          h_matrix[number_of_found_objects + 1][1] = 0.0;
              }
          }
      }

#ifdef selfcalib_debug
    {
      int markersize_x = (int)ceil((float)dimx / 1000.0);
      int markersize_y = (int)ceil((float)dimy / 1000.0);
      int markersize = markersize_x > markersize_y ? markersize_x : markersize_y;

      // markiere die Mittelpunkte der Gefundenen Objekte
      pc[2] = markersize * 3;

      for (i = 0; i < number_of_found_objects; i++)
        {
          pc[0] = h_matrix[i][0];
          pc[1] = h_matrix[i][1];
          drawCircle(pc, 4, 4, DEFAULT, mark1);
        }

      if (IsImg(debug_image))
        {
          CopyImg(pic1, debug_image);
        }

      if (IsImg(debug_mark))
        {
          CopyImg(mark1, debug_mark);
        }

      printf("Anzahl gefundener Objekte: %d\n", number_of_found_objects);
      printf("die gelben Punkte markieren den Mittelpunkt der gefundenen Objekte\n");
      printf("weiter mit <Enter> ...\n");
      getchar();

      if (IsImg(debug_image))
        {
          clearImg(debug_image);
        }

      if (IsImg(debug_mark))
        {
          clearImg(debug_mark);
        }
    }
#endif

    if (number_of_found_objects != number_of_searched_objects)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Anzahl gefundener Objekte: %d\n", number_of_found_objects);
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, "wrong number of objects");
      }

    pl2 = NewPointList(number_of_found_objects);

    for (i = 0; i < number_of_found_objects; i++)
      {
        pl2->xptr[i] = h_matrix[i][0];
        pl2->yptr[i] = h_matrix[i][1];
      }

    // Punktlisten pl1, pl2 erzeugt
    // number_of_searched_objects ist die Anzahl der Gitter-Punkte
    // number_of_found_objects ist die Anzahl der gefundenen Objekte

    Matrix m1(number_of_searched_objects, 2), m2(number_of_found_objects, 2), m1_rot(number_of_searched_objects, 2);

    Trafo rot1, rot2;

    for (i = 0; i < number_of_searched_objects; i++)
      {
        m1[i][0] = pl1->xptr[i];
        m1[i][1] = pl1->yptr[i];
      }

    sum1 = sum2 = 0.0;

    for (i = 0; i < number_of_searched_objects; ++i)
      {
        sum1 += m1[i][0];
        sum2 += m1[i][1];
      }

    sum1 /= double(number_of_searched_objects);
    sum2 /= double(number_of_searched_objects);

    for (i = 0; i < number_of_found_objects; i++)
      {
        m2[i][0] = pl2->xptr[i];
        m2[i][1] = pl2->yptr[i];
      }

    // sum1,sum2 ist Schwerpunkt der Punktmenge pl1 bzw. m1

    if (number_of_found_objects > number_of_searched_objects)
      {
        M = number_of_found_objects;
        M_MIN = number_of_searched_objects;
      }
    else
      {
        M = number_of_searched_objects;
        M_MIN = number_of_found_objects;
      }

    //Matrix cost(M, M);
    Matrix Abstand_2(M, M);
    IMatrix reference_pairs(M, 2);
    Matrix m3(M_MIN, 2);
    Matrix m4(M_MIN, 2);
    Matrix m5(M_MIN, 2);
    Vector gewichte(M_MIN);

    for (i = 0; i < M_MIN; i++)
      {
        gewichte[i] = 1.0;
      }

    double min_distance, phi_min;

    min_distance = DBL_MAX;
    phi_min = 0.0;

    for (phi = 0.0; phi < 360.0; phi += 10.0)
      {
        rot2 = rot1;
        rot2.shift(-sum1, -sum2);
        rot2.rotate(0.0, 0.0, (phi / 180.0)*M_PI);
        rot2.shift(+sum1, +sum2);
        transformList(rot2, m1, m1_rot);

        // Abstandsmatrix bestimmen
        for (i = 0; i < M; i++)
          for (j = 0; j < M; j++)
            {
              Abstand_2[i][j] = 0.0;
            }

        for (i = 0; i < number_of_searched_objects; i++)
          for (j = 0; j < number_of_found_objects; j++)
            {
              hx1 = m1_rot[i][0];
              hy1 = m1_rot[i][1];
              hx2 = m2[j][0];
              hy2 = m2[j][1];
              Abstand_2[i][j] = ((hx1 - hx2) * (hx1 - hx2) + (hy1 - hy2) * (hy1 - hy2));
            }

        Hungarian(Abstand_2, reference_pairs, min_cost);

        l = 0;

        for (k = 0; k < M; k++)
          {
            i = reference_pairs[k][0];
            j = reference_pairs[k][1];

            if (number_of_searched_objects < number_of_found_objects)
              if (i >= M_MIN)
                {
                  continue;
                }

            if (number_of_searched_objects > number_of_found_objects)
              if (j >= M_MIN)
                {
                  continue;
                }

            x1 = int(m3[l][0] = m1_rot[i][0]);
            y1 = int(m3[l][1] = m1_rot[i][1]);
            x2 = int(m4[l][0] = m2[j][0]);
            y2 = int(m4[l][1] = m2[j][1]);
            l++;
          }

        t1 = MatchPointlistsLinOpt(m3, m4, TRANSFORMATION);

        transformList(t1, m3, m5);

        set_distance = point_set_distance(m4, m5);

        if (set_distance < min_distance)
          {
            min_distance = set_distance;
            phi_min = phi;
          }
      }

    rot2 = rot1;
    rot2.shift(-sum1, -sum2);
    rot2.rotate(0.0, 0.0, (phi_min / 180.0)*M_PI);
    rot2.shift(+sum1, +sum2);

    transformList(rot2, m1, m1_rot);

    // Abstandsmatrix bestimmen
    for (i = 0; i < M; i++)
      for (j = 0; j < M; j++)
        {
          Abstand_2[i][j] = 0.0;
        }

    for (i = 0; i < number_of_searched_objects; i++)
      for (j = 0; j < number_of_found_objects; j++)
        {
          hx1 = m1_rot[i][0];
          hy1 = m1_rot[i][1];
          hx2 = m2[j][0];
          hy2 = m2[j][1];
          Abstand_2[i][j] = ((hx1 - hx2) * (hx1 - hx2) + (hy1 - hy2) * (hy1 - hy2));
        }

    Hungarian(Abstand_2, reference_pairs, min_cost);
    l = 0;

    number_of_points = number_of_found_objects;

    ref_pairs = reference_pairs;

    if (number_of_found_objects == number_of_searched_objects)
      {
        return 0;
      }
    else
      {
#ifdef selfcalib_debug
        {
          printf("Fehler: Anzahl gefundener Objekte: %d\n", number_of_found_objects);
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }
  }
#undef FNAME

#define FNAME "sort_reference_pairs"
// Sortierung der zugehörigen Referenzpaaren
  int sort_reference_pairs(IMatrix& reference_pairs, int number_of_points, PointList& pl1, PointList& pl2, Matrix& image_coord, Matrix& world_coord)
  {
    int i, j, k;

    for (k = 0; k < number_of_points; k++)
      {
        i = reference_pairs[k][0];
        j = reference_pairs[k][1];

        world_coord[k][0] = pl1->xptr[i];
        world_coord[k][1] = pl1->yptr[i];
        world_coord[k][2] = 0.0;

        image_coord[k][0] = pl2->xptr[j];
        image_coord[k][1] = pl2->yptr[j];
      }

    return 0;
  }
#undef FNAME

#define FNAME "norm_reference_pairs"
// Normierung der Referenzpaare
  int norm_reference_pairs(int number_of_points, Matrix& image_coord, Matrix& world_coord)
  {
    int dim = (int)(dimmax / 2.0);

    for (int i = 0; i < number_of_points; i++)
      {
        image_coord[i][0] /= (double)dim;
        image_coord[i][1] /= (double)dim;

        image_coord[i][0] -= ((double)dimx / 2.0) / (double)dim;
        image_coord[i][1] -= ((double)dimy / 2.0) / (double)dim;

        world_coord[i][0] /= (double)dim;
        world_coord[i][1] /= (double)dim;

        world_coord[i][0] -= ((double)dimx / 2.0) / (double)dim;
        world_coord[i][1] -= ((double)dimy / 2.0) / (double)dim;
      }

    return 0;
  }
#undef FNAME

#define FNAME "AssignCalibPattern_dist"
// Zuordnung des Musters aus dem Bild, mit bzw. ohne Entzeichnung
  int AssignCalibPattern_dist(Image& image,
                              Matrix& imagepoints, Matrix& worldpoints,
                              Camera& c, int dist,
                              Image& debug_image, Image& debug_mark)
  {
    int error = 0;

    int number_of_points;
    PointList pl1, pl2;
    IMatrix reference_pairs;

    error = scan_image(image, number_of_points, pl1, pl2, reference_pairs, debug_image, debug_mark);

    if (error != 0)
      throw IceException(FNAME, M_INTERN);

    error = sort_reference_pairs(reference_pairs, number_of_points, pl1, pl2, imagepoints, worldpoints);

    Matrix world_coords(number_of_points, 2);

    for (int i = 0; i < number_of_points; i++)
      {
        world_coords[i][0] = worldpoints[i][0];
        world_coords[i][1] = worldpoints[i][1];
      }

    // der zu uebergebende Mittelpunkt der Verzeichnung ist der Bildmittelpunkt
    Vector vector_image_center(dimx / 2.0, dimy / 2.0);

    // Verzeichnung aus Referenzpaaren berechnen
    if (dist)
      {
        dimx = image->xsize;
        dimy = image->ysize;

        if (c.DistType() == 1)
          {
            Distortion1 verzeichnung(imagepoints, world_coords, vector_image_center);

            // Eingangsbilder entzeichnen
            Image temp_image = NewImg(image);
            verzeichnung.rectifyImage(image, temp_image, INTERPOL);
            CopyImg(temp_image, image);

            c.setDist(verzeichnung);

            // Matrix der Bildkoordinaten entzeichnen und
            // normierte Bildkoordinaten aus entzeichneter Matrix berechnen

            for (int i = 0; i < number_of_points; i++)
              {
                // OffMessage();
                error = verzeichnung.rectify(imagepoints[i][0], imagepoints[i][1]);
                // SetOk();
                // OnMessage();
              }
          }
        else if (c.DistType() == 2)
          {
            Distortion2 verzeichnung(imagepoints, world_coords, vector_image_center);

            // Eingangsbilder entzeichnen
            Image temp_image = NewImg(image);
            verzeichnung.rectifyImage(image, temp_image, INTERPOL);
            CopyImg(temp_image, image);

            c.setDist(verzeichnung);

            // Matrix der Bildkoordinaten entzeichnen und
            // normierte Bildkoordinaten aus entzeichneter Matrix berechnen
            for (int i = 0; i < number_of_points; i++)
              {
                // OffMessage();
                error = verzeichnung.rectify(imagepoints[i][0], imagepoints[i][1]);
                // SetOk();
                // OnMessage();
              }
          }
        else if (c.DistType() == 3)
          {
            Distortion3 verzeichnung(imagepoints, world_coords, vector_image_center);

            // Eingangsbilder entzeichnen
            Image temp_image = NewImg(image);
            verzeichnung.rectifyImage(image, temp_image, INTERPOL);
            CopyImg(temp_image, image);

            c.setDist(verzeichnung);

            // Matrix der Bildkoordinaten entzeichnen und
            // normierte Bildkoordinaten aus entzeichneter Matrix berechnen
            for (int i = 0; i < number_of_points; i++)
              {
                // OffMessage();
                error = verzeichnung.rectify(imagepoints[i][0], imagepoints[i][1]);
                // SetOk();
                // OnMessage();
              }
          }

#ifdef selfcalib_debug
        {
          if (IsImg(debug_image))
            {
              CopyImg(image, debug_image);
            }

          printf("Entzeichnetes Bild\n");
          printf("weiter mit <Enter> ...\n");
          getchar();

          if (IsImg(debug_image))
            {
              clearImg(debug_image);
            }
        }
#endif

      }

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Verzeichnung\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "calibrate_int_ext"
// Kalibrierung mit allen möglichen Varianten
// - aus mindestens 3 verzeichneten Bildern
// - aus mindestens 3 normalen Bildern
// - aus mindestens 3 Homographien
// - aus mindestens 3 Punktkorrespondenz-Matrizen
  int calibrate_int_ext(vector<Camera>& cv,
                        int calib_mode,
                        bool calib_dist,
                        bool calib_ext,
                        vector<Image>& images_in,
                        vector<Matrix>& all_image_coords,
                        vector<Matrix>& all_world_coords,
                        vector<Trafo>& H,
                        Image& debug_image, Image& debug_mark)
  {
    int error = 0;
    int actual_image_number = 0;
    int number_of_images = images_in.size();
    int number_of_points = CPOINTS;

    vector<CameraParameters> cps(number_of_images);

    Matrix K;

    Matrix* all_repro_coords = new Matrix[number_of_images];

    for (actual_image_number = 0;
         actual_image_number < number_of_images;
         actual_image_number++)
      {
        all_repro_coords[actual_image_number] = Matrix(number_of_points, 2);
      }

    for (actual_image_number = 0;
         actual_image_number < number_of_images;
         actual_image_number++)
      {
        if (calib_mode == 0)
          {
            if (calib_dist)
              {
                error = AssignCalibPattern(images_in[actual_image_number],
                                           all_image_coords[actual_image_number],
                                           all_world_coords[actual_image_number],
                                           cv[actual_image_number],
                                           debug_image, debug_mark);
              }
            else
              {
                error = AssignCalibPattern(images_in[actual_image_number],
                                           all_image_coords[actual_image_number],
                                           all_world_coords[actual_image_number],
                                           debug_image, debug_mark);
              }
          }

        if (calib_mode < 2)
          {
            error = ComputeHomography(all_image_coords[actual_image_number],
                                      all_world_coords[actual_image_number],
                                      H[actual_image_number]);
          }
      }

    int anzahl_gleicher_werte = 0;
    Matrix image1_coords(number_of_points, 2);
    Matrix image2_coords(number_of_points, 2);

    for (int i = 0; i < number_of_images; i++)
      for (int j = i; j < number_of_images; j++)
        {
          if (i != j)
            for (int k = 0; k < number_of_points; k++)
              {
                image1_coords = all_image_coords[i];
                image2_coords = all_image_coords[j];

                if ((fabs(image1_coords[k][0]) > fabs(image2_coords[k][0]) * 0.99 &&
                     fabs(image1_coords[k][0]) < fabs(image2_coords[k][0]) * 1.01) &&
                    (fabs(image1_coords[k][1]) > fabs(image2_coords[k][1]) * 0.99 &&
                     fabs(image1_coords[k][1]) < fabs(image2_coords[k][1]) * 1.01))
                  {
                    anzahl_gleicher_werte++;
                  }

                //printf("anz: %d   i: %d   j: %d   k: %d    ", anzahl_gleicher_werte, i, j, k);
                //printf("x: %f   x: %f   y: %f   y: %f\n", image1_coords[k][0], image2_coords[k][0], image1_coords[k][1], image2_coords[k][1]);
              }
        }

    if (anzahl_gleicher_werte > 10)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Mehrere gleiche Bilder verwendet\n");
          printf("Anzahl der gleichen Punkte: %d\n", anzahl_gleicher_werte);
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif
        delete[] all_repro_coords;
        throw IceException(FNAME, M_INTERN);
      }

    // Bestimmung der internen Kameraparameter nach Zhang
    K = calib_intrinsic_zhang(H);

    if (calib_ext)
      {
        for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
          {
            double summe_der_abst = 0.0;

            // Bestimmung der externen Kameraparameter nach Zhang
            cps[actual_image_number] = calib_extrinsic_zhang(Matrix(H[actual_image_number].getMatrix()), K);

            for (int i = 0; i < number_of_points; i++)
              {
                // Ruecktransformation des Musters in das Bildkoordinatensystem
                double world_coord[3];
                double image_coord[2];

                world_coord[0] = all_world_coords[actual_image_number][i][0];
                world_coord[1] = all_world_coords[actual_image_number][i][1];
                world_coord[2] = all_world_coords[actual_image_number][i][2];

                project_point(world_coord, cps[actual_image_number], image_coord);

                all_repro_coords[actual_image_number][i][0] = image_coord[0];
                all_repro_coords[actual_image_number][i][1] = image_coord[1];

                // VERGLEICH MIT ORIGINALDATEN ALS FEHLEREINSCHAETZUNG
                summe_der_abst += sqrt((all_repro_coords[actual_image_number][i][0] -
                                        all_world_coords[actual_image_number][i][0]) *
                                       (all_repro_coords[actual_image_number][i][0] -
                                        all_world_coords[actual_image_number][i][0]) +
                                       (all_repro_coords[actual_image_number][i][1] -
                                        all_world_coords[actual_image_number][i][1]) *
                                       (all_repro_coords[actual_image_number][i][1] -
                                        all_world_coords[actual_image_number][i][1]));

              }

            if (summe_der_abst > CSUMABST)
              {

#ifdef selfcalib_debug
                {
                  printf("Fehler: zu grosse Abweichungen bei der Fehlereinschaetzung\n");
                  printf("weiter mit <Enter> ...\n");
                  getchar();
                }
#endif

                delete[] all_repro_coords;
                throw IceException(FNAME, M_INTERN);
              }
          }

        // Kameraparameter in Vector schreiben
        for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
          {
            int ain = actual_image_number;

            //Set(double fp,double ap,double sp,double u0p,double v0p);
            cv[ain].set(cps[ain].K[0][0] * (dimmax / 2.0),
                        cps[ain].K[1][1] / (-cps[ain].K[0][0]),
                        cps[ain].K[0][1] / cps[ain].K[0][0],
                        cps[ain].K[0][2] * (dimmax / 2.0) + (dimx / 2.0),
                        cps[ain].K[1][2] * (dimmax / 2.0) + (dimy / 2.0));

            //SetExt(double dxp,double dyp,double dzp,double ap,double bp,double cp);
            if ((cps[ain].R[0][0] == 0) && (cps[ain].R[1][0] == 0))
              {
                cv[ain].setExt(cps[ain].t[0] * (dimmax / 2.0),
                               cps[ain].t[1] * (dimmax / 2.0),
                               cps[ain].t[2] * (dimmax / 2.0),
                               0.0,
                               90.0,
                               atan(cps[ain].R[0][1] / cps[ain].R[1][1]));
              }
            else
              {
                cv[ain].setExt(cps[ain].t[0] * (dimmax / 2.0),
                               cps[ain].t[1] * (dimmax / 2.0),
                               cps[ain].t[2] * (dimmax / 2.0),
                               atan(cps[ain].R[1][0] / cps[ain].R[0][0]),
                               atan(-cps[ain].R[2][0]) / sqrt((cps[ain].R[0][0] * cps[ain].R[0][0]) + (cps[ain].R[1][0] * cps[ain].R[1][0])),
                               atan(cps[ain].R[2][1] / cps[ain].R[2][2]));
              }
          }
      }
    else
      {
        // Kameraparameter in Vector schreiben
        //Set(double fp,double ap,double sp,double u0p,double v0p);
        cv[0].set(K[0][0] * (dimmax / 2.0),
                  K[1][1] / (-K[0][0]),
                  K[0][1] / K[0][0],
                  K[0][2] * (dimmax / 2.0) + (dimx / 2.0),
                  K[1][2] * (dimmax / 2.0) + (dimy / 2.0));
      }

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: calibrate_int_ext(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    delete[] all_repro_coords;
    return error;
  }
#undef FNAME

#define FNAME "AssignCalibPattern"
  int AssignCalibPattern(Image& image, Matrix& imagepoints, Matrix& worldpoints, Image debug_image, Image debug_mark)
  {
    if (imagepoints.rows() != CPOINTS || imagepoints.cols() != 2 ||
        worldpoints.rows() != CPOINTS || worldpoints.cols() != 3)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Dimensionen (imagepoints[%d, 2], worldpoints[%d, 3])\n", CPOINTS, CPOINTS);
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_MATRIXFORMAT);

        return WRONG_PARAM;
      }

    int error = 0;

    Camera c;

    error = AssignCalibPattern_dist(image, imagepoints, worldpoints, c, false, debug_image, debug_mark);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: AssignCalibPatern_dist(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "AssignCalibPattern"
  int AssignCalibPattern(Image& image, Matrix& imagepoints, Matrix& worldpoints, Camera& c, Image debug_image, Image debug_mark)
  {
    if (imagepoints.rows() != CPOINTS || imagepoints.cols() != 2 ||
        worldpoints.rows() != CPOINTS || worldpoints.cols() != 3)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Dimensionen (imagepoints[%d, 2], worldpoints[%d, 3])\n", CPOINTS, CPOINTS);
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_MATRIXFORMAT);

        return WRONG_PARAM;
      }

    int error = 0;

    error = AssignCalibPattern_dist(image, imagepoints, worldpoints, c, true, debug_image, debug_mark);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: AssignCalibPatern_dist(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "ComputeHomography"
  int ComputeHomography(Matrix& imagepoints, Matrix& worldpoints, Trafo& H)
  {
    if (imagepoints.rows() < 4 || imagepoints.cols() != 2 ||
        worldpoints.rows() < 4 || worldpoints.cols() != 3 ||
        worldpoints.rows() != imagepoints.rows())
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Dimensionen (imagepoints[>=4, 2], worldpoints[>=4, 3])\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_MATRIXFORMAT);
      }

    int error = 0;

    int number_of_points = worldpoints.rows();

    error = norm_reference_pairs(number_of_points, imagepoints, worldpoints);

    // Homographien berechnen
    H = calib_homography_zhang(number_of_points, imagepoints, worldpoints);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: ComputeHomography(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "CalibrateWithPattern"
  int CalibrateWithPattern(vector<Matrix>& imagepoints, vector<Matrix>& worldpoints, Camera& c)
  {
    if (imagepoints.size() < 3 || imagepoints.size() != worldpoints.size())
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: zu geringe Datenanzahl oder Datenanzahl nicht gleich\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);

        return WRONG_PARAM;
      }

    for (unsigned int i = 0; i < imagepoints.size(); i++)
      {
        if (imagepoints[i].rows() != CPOINTS || imagepoints[i].cols() != 2 ||
            worldpoints[i].rows() != CPOINTS || worldpoints[i].cols() != 3)
          {

#ifdef selfcalib_debug
            {
              printf("Fehler: Falsche Dimensionen (imagepoints[>=4, 2], worldpoints[>=4, 3])\n");
              printf("weiter mit <Enter> ...\n");
              getchar();
            }
#endif

            throw IceException(FNAME, M_MATRIXFORMAT);

            return WRONG_PARAM;
          }
      }

    int error = 0;

    vector<Camera> cv(imagepoints.size());

    for (unsigned int i = 0; i < imagepoints.size(); i++)
      {
        cv[i] = c;
      }

    int number_of_images = imagepoints.size();
    vector<Image> images(number_of_images);
    vector<Trafo> H(number_of_images);

    Image debug_image, debur_mark;

    error = calibrate_int_ext(cv, 1, false, false, images, imagepoints, worldpoints, H, debug_image, debur_mark);

    c = cv[0];

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: CalibrateWithPattern(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "CalibrateWithPattern"
  int CalibrateWithPattern(vector<Matrix>& imagepoints, vector<Matrix>& worldpoints, vector<Camera>& cv)
  {
    if (imagepoints.size() < 3 || imagepoints.size() != worldpoints.size() || imagepoints.size() != cv.size())
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: zu geringe Datenanzahl oder Datenanzahl nicht gleich\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);

        return WRONG_PARAM;
      }

    for (unsigned int i = 0; i < imagepoints.size(); i++)
      {
        if (imagepoints[i].rows() != CPOINTS || imagepoints[i].cols() != 2 ||
            worldpoints[i].rows() != CPOINTS || worldpoints[i].cols() != 3)
          {

#ifdef selfcalib_debug
            {
              printf("Fehler: Falsche Dimensionen (imagepoints[>=4, 2], worldpoints[>=4, 3])\n");
              printf("weiter mit <Enter> ...\n");
              getchar();
            }
#endif

            throw IceException(FNAME, M_MATRIXFORMAT);

            return WRONG_PARAM;
          }
      }

    int error = 0;

    int number_of_images = imagepoints.size();

    vector<Image> images(number_of_images);

    vector<Trafo> H(number_of_images);

    Image debug_image, debur_mark;

    error = calibrate_int_ext(cv, 1, false, true, images, imagepoints, worldpoints, H, debug_image, debur_mark);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: CalibrateWithPattern(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "CalibrateWithHomographies"
  int CalibrateWithHomographies(vector<Trafo>& H, Camera& c)
  {
    if (H.size() < 3)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Homographieanzahl (H[>=3])\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);
      }

    int error = 0;

    vector<Camera> cv(H.size());

    for (unsigned int i = 0; i < H.size(); i++)
      {
        cv[i] = c;
      }

    int number_of_images = H.size();
    int actual_image_number = 0;
    int number_of_points = CPOINTS;

    vector<Image> images(number_of_images);

    vector<Matrix> all_image_coords(number_of_images);
    vector<Matrix> all_world_coords(number_of_images);

    for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
      {
        all_image_coords[actual_image_number] = Matrix(number_of_points, 2);
        all_world_coords[actual_image_number] = Matrix(number_of_points, 3);
      }

    Image debug_image, debur_mark;

    error = calibrate_int_ext(cv, 2, false, false, images, all_image_coords, all_world_coords, H, debug_image, debur_mark);

    c = cv[0];

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: CalibrateWithHomographies\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "CalibrateWithHomographies"
  int CalibrateWithHomographies(vector<Trafo>& H, vector<Camera>& cv)
  {
    if (H.size() < 3 || H.size() != cv.size())
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Homographieanzahl (H[>=3]) oder Anzahl von H und cv verschieden\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);

        return WRONG_PARAM;
      }

    int error = 0;

    int number_of_images = H.size();
    int actual_image_number = 0;
    int number_of_points = CPOINTS;

    vector<Image> images(number_of_images);

    vector<Matrix> all_image_coords(number_of_images);
    vector<Matrix> all_world_coords(number_of_images);

    for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
      {
        all_image_coords[actual_image_number] = Matrix(number_of_points, 2);
        all_world_coords[actual_image_number] = Matrix(number_of_points, 3);
      }

    Image debug_image, debur_mark;

    error = calibrate_int_ext(cv, 2, false, true, images, all_image_coords, all_world_coords, H, debug_image, debur_mark);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: CalibrateWithHomographies(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "calibrate"
  int Calibrate(vector<Image>& images, vector<Camera>& cv,
                bool dist, Image debug_image, Image debug_mark)
  {
    if (images.size() < 3 || images.size() != cv.size())
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Bildanzahl (images[>=3]) oder Anzahl von images und cv verschieden\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);

        return WRONG_PARAM;
      }

    int error = 0;

    int number_of_images = images.size();
    int actual_image_number = 0;
    int number_of_points = CPOINTS;

    vector<Matrix> all_image_coords(number_of_images);
    vector<Matrix> all_world_coords(number_of_images);

    for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
      {
        all_image_coords[actual_image_number] = Matrix(number_of_points, 2);
        all_world_coords[actual_image_number] = Matrix(number_of_points, 3);
      }

    vector<Trafo> H(number_of_images);

    error = calibrate_int_ext(cv, 0, dist, true, images, all_image_coords, all_world_coords, H, debug_image, debug_mark);

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Calibrate(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "Calibrate"
  int Calibrate(vector<Image>& images, Camera& c, bool dist, Image debug_image, Image debug_mark)
  {
    if (images.size() < 3)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Bildanzahl (images[>=3])\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_VECTORDIM);
      }

    int error = 0;

    vector<Camera> cv(images.size());

    for (unsigned int i = 0; i < images.size(); i++)
      {
        cv[i] = c;
      }

    int number_of_images = images.size();
    int actual_image_number = 0;
    int number_of_points = CPOINTS;

    vector<Matrix> all_image_coords(number_of_images);
    vector<Matrix> all_world_coords(number_of_images);

    for (actual_image_number = 0; actual_image_number < number_of_images; actual_image_number++)
      {
        all_image_coords[actual_image_number] = Matrix(number_of_points, 2);
        all_world_coords[actual_image_number] = Matrix(number_of_points, 3);
      }

    vector<Trafo> H(number_of_images);

    error = calibrate_int_ext(cv, 0, dist, false, images,
                              all_image_coords, all_world_coords,
                              H, debug_image, debug_mark);

    c = cv[0];

    if (error != 0)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Calibrate(...)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_INTERN);
      }

    return error;
  }
#undef FNAME

#define FNAME "DrawPattern"
  Image DrawPattern(int dimx, int dimy)
  {
    if (dimx < 1 || dimy < 1)
      {

#ifdef selfcalib_debug
        {
          printf("Fehler: Falsche Dimension (z.B. 640x480)\n");
          printf("weiter mit <Enter> ...\n");
          getchar();
        }
#endif

        throw IceException(FNAME, M_WRONG_PARAM);

        Image img = NewImg(dimx, dimy, 255);

        return img;
      }

    Image img = NewImg(dimx, dimy, 255);

    Matrix pl(4, 2);

    Contur c;

    double abstand = 0.0;        // Abstand der Muster in Bezug abhaengig von Pixelzahl und Bildrichtung
    double verschiebung_x = 0.0; // Verschiebung, damit Muster in der Mitte liegt
    double verschiebung_y = 0.0; // Verschiebung, damit Muster in der Mitte liegt

    if (dimx > dimy)
      {
        if (((double)dimx / 10.0) < ((double) dimy / 7.0))
          {
            abstand = (double)dimx / 10.0;
            verschiebung_y = (dimy / 2.0) - (3.5 * abstand);
          }
        else
          {
            abstand = (double)dimy / 7.0;
            verschiebung_x = (dimx / 2.0) - (5.0 * abstand);
          }
      }
    else
      {
        if (((double)dimx / 7.0) < ((double) dimy / 10.0))
          {
            abstand = (double)dimx / 7.0;
            verschiebung_y = (dimy / 2.0) - (5.0 * abstand);
          }
        else
          {
            abstand = (double)dimy / 10.0;
            verschiebung_x = (dimx / 2.0) - (3.5 * abstand);
          }
      }

    for (int i = 0; i < CPOINTS; i++)
      {
        if (dimx > dimy)
          {
            pl[0][1] = (double)dimy - pattern[i][0] * abstand - verschiebung_y;
            pl[0][0] = pattern[i][1] * abstand + verschiebung_x;
            pl[1][1] = (double)dimy - pattern[i][2] * abstand - verschiebung_y;
            pl[1][0] = pattern[i][3] * abstand + verschiebung_x;
            pl[2][1] = (double)dimy - pattern[i][4] * abstand - verschiebung_y;
            pl[2][0] = pattern[i][5] * abstand + verschiebung_x;
            pl[3][1] = (double)dimy - pattern[i][6] * abstand - verschiebung_y;
            pl[3][0] = pattern[i][7] * abstand + verschiebung_x;;
          }
        else
          {
            pl[0][0] = pattern[i][0] * abstand + verschiebung_x;
            pl[0][1] = pattern[i][1] * abstand + verschiebung_y;
            pl[1][0] = pattern[i][2] * abstand + verschiebung_x;
            pl[1][1] = pattern[i][3] * abstand + verschiebung_y;
            pl[2][0] = pattern[i][4] * abstand + verschiebung_x;
            pl[2][1] = pattern[i][5] * abstand + verschiebung_y;
            pl[3][0] = pattern[i][6] * abstand + verschiebung_x;
            pl[3][1] = pattern[i][7] * abstand + verschiebung_y;
          }

        c.reset();
        c.setStart((int)pl[3][0], (int)pl[3][1]);

        for (int j = 0; j < 4; j++)
          {
            c.add((int)pl[j][0], (int)pl[j][1]);
          }

        FillRegion(c, 255, img);
      }

    return img;
  }
#undef FNAME
} //namespace ice

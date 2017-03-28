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
 * affinfit.c  Fitting einer affinen Transformation zwischen zwei Polygonen
 *
 *     m. schubert 08/1996
 *     H. Suesse 01/2009
 *     W. Ortmann 03/2009
 */

#ifndef _AFFINFIT_H
#define _AFFINFIT_H

#include "pointlist.h"

namespace ice
{
  /****************************************************************************/
  /* Berechnung des Abstandes zweier Polygone, indem zu Punkten des ersten    */
  /* Polygon der Abstand zum naechstliegenden Punkt des anderen Polygons      */
  /* bestimmt und der Mittelwert gebildet wird.                               */
  /****************************************************************************/
  double DistPolygons(PointList pl1, PointList pl2, double tr[3][3]);
  /****************************************************************************/
  /* Es wird eine affine Transformation zwischen zwei Polygonen mit Hilfe     */
  /* von Flaechenmomenten und darauf basierenden Standardlagen angepaßt       */
  /****************************************************************************/
  double AffinFitPolygons(PointList pl1, PointList pl2, double tr[3][3]);
  /****************************************************************************/
  /* Es wird an das Objekt mit den Momenten m ein Dreieck mit Hilfe           */
  /* von Flaechenmomenten und darauf basierenden Standardlagen angepaßt       */
  /****************************************************************************/

  int FitSquareMoments(const double mp[], double p[][2]);
  int FitSquareMoments(const double mp[], double p[][2], double& guetemass);

  int FitTriangleMoments(const double m[15], double p[3][2]);

  int FitEquilateraltriangleMoments(const double mp[], double p[][2]);
  int FitEquilateraltriangleMoments(const double mp[], double p[][2], double& guetemass);

  int FitIsoscelestriangleMoments(const double mp[], double p[][2]);
  int FitIsoscelestriangleMoments(const double mp[], double p[][2], double& guetemass);

  int FitParallelogramMoments(const double m[15], double p[4][2]);
  int FitParallelogramMoments(const double m[15], double p[4][2], double& guetemass);

  int FitPolygonMoments(int n, const double mp[15], double pstart[][2], double p[][2]);
  int FitPolygonMoments(int n, const double mp[15], double pstart[][2], double p[][2], double& guetemass);
  int FitParallelogramMoments(const double m[15], Matrix& mat);
  int FitParallelogramMoments(const double m[15], Matrix& mat, double& guetemass);

  int FitRectangleMoments(const double m[15], double p[4][2]);
  int FitRectangleMoments(const double m[15], double p[4][2], double& guetemass);

  int FitCircleMoments(const double m[15], double& x0, double& y0, double& radius);
  int FitCircleMoments(const double mp[15], double& x0, double& y0, double& radius, double& guetemass);

  int FitEllipseMoments(const double m[15], double ell_par[]);

  int FitEllipticalSegmentMoments(const double m[15], double ell_par[5],
                                  double seg_par1[2], double seg_par2[]);

  int FitCircularSegmentMoments(const double m[15], double circle_par[3],
                                double seg_par1[2], double seg_par2[2]);

  int FitSuperEllipseMoments(const double moments_object[15],
                             double& sup_c1, double& sup_f1, double sup_tr1[3][3],
                             double& sup_c2, double& sup_f2, double sup_tr2[3][3]);
// ***********************************************************************

  int OrientationMoments(const double moment[15], double& angle);
}
#endif

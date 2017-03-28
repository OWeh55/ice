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
/*************************************************
 *                                               *
 *  Euklidische Bewegungen (3D) mit Frames       *
 *                                               *
 *  (C) m. schubert 2.95                         *
 *************************************************/

#ifndef _FRAMES_H
#define _FRAMES_H

#include "geo.h"

namespace ice
{
  typedef struct
  {
    double frame[4][4];
    double err;
  } Frame;

  double* FTransPoint(double p1[3], Frame* f, double p2[3]);
  double* FTransVec(double v1[3], Frame* f, double v2[3]);
  Frame* CatFrames(Frame* f1, Frame* f2, Frame* f3);
  Frame* InvertFrame(Frame* f1, Frame* f2);
  int CopyFrame(Frame* f1, Frame* f2);
  int CalcPlaneFrame(double* p, int n, Frame* f);
  //  int CopyPointCorr(PointCorr *pcs, PointCorr *pcd);
  //  Frame *CameraFrame(camera cam, Frame *frm);
  //  int RecalibCamera(camera c, Frame *frm);
  int ReadFrame(char* file, Frame* f);
  int WriteFrame(Frame* f, char* file);
  Frame* MakeFrame(int mode, double par[6], Frame* frm);
  int MakeEuler(int mode, Frame* f, double par1[6], double par2[6]);
  int MakeAxleFrame(double axle[3], double phi, Frame* f);
  Frame* OrthonormalizeFrame(Frame* frm);

  int FrameInit(Frame* f);
  int TrafoToFrame(const Trafo& t, Frame* f);
  int FrameToTrafo(Frame* f, Trafo& t);
}
#endif

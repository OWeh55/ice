/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2013 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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
#ifndef ICE_COMMON_H
#define ICE_COMMON_H

/*
 * all "normal" include files
 */

#include "Accumulator.h"
#include "Accumulator2.h"
#include "affinfit.h"
#include "analygeo.h"
#include "arith.h"
#include "assignment.h"
#include "based.h"
#include "base.h"
#include "Camera.h"
#include "circle.h"
#include "circleseg.h"
#include "Classifier.h"
#include "ClassifierBayes.h"
#include "ClassifierBayes2.h"
#include "ClassifierMinimumDistance.h"
#include "ClassifierNearestNeighbor.h"
#include "ClassifierKNearestNeighbor.h"
#include "ClassifierTree.h"
#include "ClassifierRandomForest.h"
#include "ClassifierWithNames.h"
#include "ColorImageFunctions.h"
#include "ColorImage.h"
#include "ColorSpace.h"
#include "ColorValue.h"
#include "componenttree.h"
#include "contfeat.h"
#include "contools.h"
#include "conturfunctions.h"
#include "Contur.h"
#include "conturPolygon.h"
#include "convex.h"
#include "convexhull.h"
#include "convolution_fft.h"
#include "darith.h"
#include "defs.h"
#include "delaunay.h"
#include "derive.h"
#include "dijkstra.h"
#include "dirfunc.h"
#include "DisjointSets.h"
#include "distance.h"
#include "Distortion0.h"
#include "Distortion1.h"
#include "Distortion2.h"
#include "Distortion3.h"
#include "Distortion.h"
#include "disttrafo.h"
#include "DPList.h"
#include "draw.h"
#include "drawline.h"
#include "dtime.h"
#include "dualquaternion.h"
#include "ellipse.h"
#include "equsys.h"
#include "feature.h"
#include "filter.h"
#include "fitfn.h"
#include "fitgauss.h"
#include "fit.h"
#include "fitmoments.h"
#include "forest.h"
#include "fourier.h"
#include "fouriertrafo.h"
#include "fouriertrafo2d.h"
#include "freeman.h"
#include "function.h"
#include "gentrans.h"
#include "geo.h"
#include "geo_ob.h"
#include "glinefit.h"
#include "haar.h"
#include "histogramequalization.h"
#include "hist.h"
#include "icefunc.h"
#include "IceException.h"
#include "image3dfunc.h"
#include "image3d.h"
#include "imatch.h"
#include "IMatrix.h"
#include "KDTree.h"
#include "KDIndexTree.h"
#include "klt.h"
#include "lineaccumulation.h"
#include "lineqn.h"
#include "lineseg.h"
#include "lintrans.h"
#include "lists.h"
#include "lmdif.h"
#include "LMSolver.h"
#include "lseg.h"
#include "LsiFilter.h"
#include "ludecomp.h"
#include "matchimg.h"
#include "matdef.h"
#include "mateigen.h"
#include "mathfunc.h"
#include "MatrixAlgebra.h"
#include "matrix_function.h"
#include "Matrix.h"
#include "matrixtemplate.h"
#include "matrixtools.h"
#include "MaxSrch.h"
#include "IceException.h"
#include "MinTree.h"
#include "momente.h"
#include "moments.h"
#include "morph.h"
#include "MtchTool.h"
#include "numbase.h"
#include "objectfunction.h"
#include "omatch.h"
#include "paint.h"
#include "peak1d.h"
#include "peakEvaluation.h"
#include "PhaseCor.h"
#include "picio.h"
#include "Point.h"
#include "PointListWalker.h"
#include "polyfit.h"
#include "polygonalcurve.h"
#include "polygon.h"
#include "polygonim.h"
#include "PreProc.h"
#include "qft.h"
#include "quaternion.h"
#include "quatmatch.h"
#include "quatmatrix.h"
#include "quatvector.h"
#include "radon.h"
#include "rank.h"
#include "readwrit.h"
#include "region.h"
#include "RegionWalker.h"
#include "ridge.h"
#include "rooteig.h"
#include "root.h"
#include "rotquaternion.h"
#include "Sdr.h"
#include "segment1.h"
#include "segment.h"
#include "selfcalib.h"
#include "shading.h"
#include "simplex.h"
#include "sort.h"
#include "statistics.h"
#include "strtool.h"
#include "swaptilt.h"
#include "threshld.h"
#include "tophat.h"
#include "trafo_color.h"
#include "trafodualquaternion.h"
#include "Trafo.h"
#include "trafo_imgd.h"
#include "trafo_img.h"
#include "trans.h"
#include "trfunc.h"
#include "triangle.h"
#include "util.h"
#include "Vector3d.h"
#include "vectorarith.h"
#include "vectordistance.h"
#include "vectorfunc.h"
#include "vectortools.h"
#include "Vector.h"
#include "VideoFile.h"
#include "WindowWalker.h"
#include "Walker.h"
#include "WaterShed.h"

#endif

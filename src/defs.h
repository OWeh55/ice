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
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif
//----------------------------------------------------
// defs.h
// Definitions for error messages, return values, constants
//

#ifndef _DEFS_H
#define _DEFS_H

namespace ice
{
//
// Error messages (Strings)
//

#define M_0       ""
#define M_INTERN  "internal error"
#define M_NO_MEM  "not enough memory"

#define M_NOT_IMPLEMENTED "not implemented"

// general parameter errors
#define M_NOT_FOUND   "not found"

#define M_WRONG_PARAMETER   "wrong parameter"

#define M_WRONG_MODE    "invalid mode"
#define M_CONFLICTING_MODES "conflictiong modes"

#define M_WRONG_LENGTH   "invalid length parameter"
#define M_WRONG_VALUE   "invalid pixel value parameter"
#define M_WRONG_SIZE  "invalid size parameter"
#define M_WRONG_COORDINATE "invalid coordinates"
#define M_WRONG_INDEX "invalid index"
#define M_WRONG_DIMENSION   "invalid dimension"
#define M_WRONG_PTR   "invalid pointer"
#define M_WRONG_POINTER M_WRONG_PTR
#define M_WRONG_POINTLIST   "invalid pointlist"
#define M_EMPTY_POINTLIST       "empty pointlist"
#define M_EMPTY_LIST            "empty list"
#define M_DIFFERENT_LISTSIZE    "list sizes differ"
#define M_TOO_LESS_POINTS       "no enough points"
#define M_TOO_MUCH_POINTS       "too much points"
#define M_TOO_LESS_VALUES       "too less values"

#define M_XTOOSMALL             "x coordinate too small"
#define M_XTOOLARGE              "x coordinate too large"
#define M_YTOOSMALL              "y coordinate too small"
#define M_YTOOLARGE              "x coordinate too large"
#define M_ZTOOSMALL              "z coordinate too small"
#define M_ZTOOLARGE              "x coordinate too large"
#define M_VALTOOSMALL           "pixel value too small"
#define M_VALTOOLARGE           "Pixel value too large"

#define M_X_OUT_OF_RANGE         "x coordinate out of range"
#define M_Y_OUT_OF_RANGE        "y coordinate out of range"
#define M_Z_OUT_OF_RANGE        "z coordinate out of range"
#define M_VAL_OUT_OF_RANGE      "value out of range"

#define M_INVALID_STRUCT  "invalid data structure"

#define M_VECTORDIM             "wrong vector dimension"

// general errors
#define M_NOT_INITIALIZED       "not initialized"
#define M_ALREADY_INITIALIZED   "already initialized"
#define M_NO_DATA               "no data"
#define M_WRONG_CODING          "wrong string coding"

// classifiers
#define M_NOT_TRAINED           "classifier not trained"
#define M_NOT_FINISHED          "training not finished"

#define M_INVALID_CLASSNUMBER   "invalid class index"
#define M_INVALID_NCLASSES      "invalid number of classes"

// Files
// #define M_WRONG_FILE    "Dateifehler"
#define M_FILE_OPEN     "Cannot open file"
#define M_WRONG_WRITE   "error writing to file"
#define M_WRONG_READ    "error reading from file"
#define M_WRONG_FILETYPE        "wrong type of file"
#define M_WRONG_FILEFORMAT      "wrong file format"
#define M_UNSUPPORTED_FILE      "unsupported file type"
#define M_ALREADY_OPEN          "already open"
#define M_NOT_OPEN              "not opened"

// Visualisation
#define M_NOT_VIS   "image not displayed"
#define M_NOT_AVAILABLE         "option not available"
#define M_HIGHRANGE_VIS         "mode not usable with value images. use Show(GRAY,..)"

// Fehler bei Bildeingabegeräten
#define M_NO_SCANDEVICE         "no scan device available"
#define M_WRONG_CONFIG          "wrong scan device configuration"
#define M_NO_PREVIEW            "preview not possible"
#define M_SCAN_ERROR            "scan error"

  /* Arbeit mit Bildern */

#define M_INVALID_IMAGE   "invalid image"
#define M_WRONG_IMAGE     M_INVALID_IMAGE

#define M_INVALID_IMAGED  "invalid double image"
#define M_WRONG_IMAGED    M_INVALID_IMAGED

#define M_WRONG_IMGSIZE   "wrong image size"
#define M_IMAGE_TOO_LARGE "image too large"
#define M_IMAGE_TOO_SMALL "image too small"
#define M_SIZES_DIFFER    "image sizes differ"
#define M_SAME_IMAGE      "same image"
#define M_LOWRANGE        "pixel range too low"
#define M_HIGHRANGE       "pixel range too high"

#define M_OUTSIDE         "point outside image"

#define M_MODE_NOT_APPLYABLE    "mode not applicable"

#define M_NOT_CONVERTIBLE "not convertible"
#define M_WRONG_RANGE     "ranges differ"

#define M_WRONG_CS        "wrong color space"
#define M_CS_DIFFER       "different color spaces"

  /* Arbeit mit Fenstern */
#define M_INVALID_WINDOW   "invalid window"

  /* Konturen*/
#define M_INVALID_CONTUR      "invalid contur"
#define M_WRONG_STARTPOINT    "invalid starting point"
#define M_WRONG_STARTPOINT2   "insulated starting point"
#define M_WRONG_STARTPOINT3   "starting point inside object"
#define M_CONTUR_NOT_CLOSED   "contur not closed"

  /* Numerik */
#define M_DIVISION_ZERO     "division by zero"
#define M_NO_SOLUTION       "no solution"
#define M_SOL_MANIFOLD      "manifold of solutions"
#define M_NUM_INSTABILITY   "numerical instability"
#define M_ZERO_VECTOR       "zero vector"
#define M_NO_ORTHO          "no orthogonal matrix"
#define M_WRONG_VECTOR      "invalid vector"
#define M_ZERO_DET          "not solvable (determinant is zero)"

#define M_MATRIXFORMAT      "wrong matrix format"
#define M_WRONG_MATRIX      "invalid matrix"
#define M_MAT_NO_COMPAT     "matrices not compatible"
#define M_MATRIX_SINGULAR   "matrix is singular"
#define M_NO_REGULAR        "Matrix is not regular"
#define M_NO_SYMM           "Matrix is not symmetrical"
#define M_NO_SQUARE         "Matrix is not quadratic"
#define M_VEC_DEPEND        "Vectors not independent"
#define M_NO_INVERSE        "inverse not exists"
#define M_WRONG_START       "invalid initial state"
#define M_POINT_IDENTIC     "indentical points"
#define M_WRONG_POINTS      "invalid point set"

// geometrische Transformationen, Matching
#define M_EMPTY_OBJECT      "empty object"
#define M_WRONG_TRAFO       "degenerate transformation matrix"
#define M_TRAFO_NOTAFFINE   "transformation not affine"
#define M_OBJECT_FEATURE    "error determining object features"

// Histogramme
#define M_HIST_EMPTY        "no entry in histogram"

// Statistik
#define M_STAT_NOENTRY      "no entry in statistics"
#define M_ZERO_VARIANZ      "variance is zero for one dimension"

#define M_NO_ELLIPSE        "no ellipse"

#define M_NOT_NESTED        "function may not be called nested!"

  /************************************************
   *                                              *
   *    Konstanten, Fehler- und Rueckkehr-Kodes   *
   *                                              *
   ************************************************/

// Return-Codes/Error-Codes of functions
#ifndef ERROR
#define ERROR           (-255)
#endif

#define WRONG_PARAM   (-1)
#define WRONG_PARAMETER (WRONG_PARAM)
#define NO_GRAPHIC    (-2)
#define TOO_LARGE   (-3)
#define NOT_FOUND   (-4)
#define NO_MEM      (-5)
#define FILE_NOT_FOUND          (-6)
#define WRONG_FILE    (-7)
#define WRONG_WINDOW      (-8)
#define NO_SOLUTION       (-9)
#define VARIOUS_SOLUTION    (-10)
#define NUM_INSTABILITY     (-11)
#define INVALID_STRUCTURE   (-12)
#define INTERN_ERROR    (-13)
#define NO_UNIQUE_SOLUTION  (-14)
#define WRONG_POINTER   (-15)
#define WRONG_STARTPOINT  (-16)
#define CHAIN_DESTROYED         (-17)
#define DIVISION_ZERO           (-18)
#define ZERO_VECTOR             (-19)
#define MISSING_DATA            (-20)
#define WRONG_MODE    (-21)
#define CONOCLOSED    (-22)
#define WRONG_PRECISION         (-23)
#define WRONG_VECTOR    (-24)
#define POINT_IDENTIC   (-25)
#define WRONG_TRANS             (-26)
#define INFINIT_POINT   (-27)
#define NO_ELLIPSE    (-28)
#define WRONG_MATRIX            (-29)
#define MAT_NO_COMPAT           (-30)
#define INVALID_CALL            (-31)
#define WRONG_STATE             (-32)

#define OK    0
#define INVALID   1

// Warnings for intersection, ..
#define OUTSIDE         2

#define OFF   0       /* Schalter fuer Visualisierung .. */
#define ON    1
#define GRAY 20
#define STEREO_IH 2
#define RESET   5
#define UPDATE    6
#define OVERLAY   7
#define SET             8

// RGB Macro is is also used in wx-2.6.2\include\wx/msw/private.h
#define _RGB    9

#define Rgb   10
#define rGb   11
#define rgB   12

#define OVERLAY1        13
#define OVERLAY2        14
#define OVERLAY3        15

#define RAISE           16
#define HIDE            17
#define UPDATE_WINDOW   18
#define RESET_WINDOW    19

  /* Informations-Konstante */

#define DEFAULT   0
#define MAXX    1
#define MAXY    2
#define TABLE_LEN 3
#define REAL_TIME_COLOR 4
#define REAL_TIME_ZOOM  5
#define CHANNEL   6
#define VIRTUAL_X 7
#define VIRTUAL_Y 8
#define IMAGES    9
#define TRUECOLOR 10
#define ALL   11
#define WNDW    12

#define EQUAL   13
#define NOT_EQUAL 14
#define NORMAL          15 /* Hintransformation */
#define INVERS          16 /* Ruecktransformation */
#define REAL    17 /* reelle Loesung */
#define COMPLEX   18 /* komplexe Loesung */
#define MAXVAL          19

#define NO_REGULAR  30 /* keine regulaere Matrix */
#define ZERO            31 /* Nullmatrix */
#define UNIT    32 /* Einheitsmatrix */
#define ORTHONORMAL 33 /* Orthonormalmatrix */
#define DIAGONAL  34 /* Diagonalmatrix */
#define SYMMETRIC 35 /* Symmetr. Matrix */

#define EXIST   40
#define NO_EXIST  41
#define DEGENERATE      42 /* entartete Kurve */
#define LINE    43 /* eine Gerade */
#define CUTTING_LINES   44 /* sich schneidende Geraden */
#define PARALLEL_LINES  45 /* parallele Geraden */
#define PARABEL   46
#define HYPERBEL        47
#define ELLIPSE         48
#define INTERPOL  49 /* Interpolation */
#define IDENTICAL 50
#define PARALLEL  51
#define ROW   52
#define NOFILL    53
#define RAW             54 /* "Rohe" Daten übertragen */
#define NORMALIZED      55 /* Daten normalisieren */
#define ADAPTIVE        56
#define SIGNED          57
#define UNSIGNED        58
#define NO_INTERSECTION 59
#define TANGENT         60

  /* Fileformate */
#define UNSUPPORTED_TYP 70
#define UNKNOWN_TYP     71
#define TIF             72
#define PCX             73
#define PNG             74
#define TGA             75
#define JPEG            76
#define BMP             77
#define PBM             78
#define PSD             79

  // colors
#define C_BLACK         0 // alpha display only
#define C_BROWN         0 // image display only
#define C_RED           1
#define C_GREEN         2
#define C_BLUE          3
#define C_YELLOW        4
#define C_CYAN          5
#define C_MAGENTA       6
#define C_PINK          7 // image display only
#define C_WHITE         7 // alpha display only

  // default values
  // defaults maxval for implicitly created images
#define DEFAULT_IMG_MAXVAL 255

#define MODE_NEW 1
#define MODE_COPY 2
#define MODE_REPLACE 3

}
#endif /*_DEFS_H*/

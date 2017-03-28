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
#define M_INTERN  "interner Fehler"
#define M_OS      "Betriebssystemfehler "
#define M_NO_MEM  "Nicht genug Speicher vorhanden"

#define M_NOT_IMPLEMENTED "Funktion nicht implementiert"

// general parameter errors
#define M_NOT_FOUND   "Nicht gefunden"
#define M_WRONG_PARAM   "Falsche Parameterbelegung"
#define M_WRONG_PARAMETER M_WRONG_PARAM
#define M_WRONG_MODE    "Unzulaessiger Modus"
#define M_CONFLICTING_MODES "Widerspruechliche Modi"

#define M_WRONG_LEN   "Unzulaessige Laengenangabe"
#define M_WRONG_LENGTH M_WRONG_LEN
#define M_WRONG_VAL   "Unzulaessiger Wert-Parameter"
#define M_WRONG_VALUE M_WRONG_VAL
#define M_WRONG_MAGNITUDE "Unzulaessige Groessenangabe"
#define M_WRONG_COORDINATE "Unzulaessige Koordinatenangaben"
#define M_WRONG_INDEX           "Unzulässiger Index"
#define M_WRONG_DIM             "Falsche Dimension"
#define M_WRONG_DIMENSION M_WRONG_DIM
#define M_WRONG_PTR   "Unzulaessiger Pointer"
#define M_WRONG_POINTER M_WRONG_PTR
#define M_WRONG_POINTLIST   "Ungueltige Punktliste"
#define M_EMPTY_POINTLIST       "Leere Punktliste"
#define M_EMPTY_LIST            "Leere Liste"
#define M_DIFFERENT_LISTSIZE    "Unterschiedliche Listenlaenge"
#define M_TOO_LESS_POINTS       "Zu wenig Punkte"
#define M_TOO_MUCH_POINTS       "Zu viele Punkte"
#define M_TOO_LESS_VALUES       "Zu wenig Werte"

#define M_XTOOSMALL             "x-Koordinate zu klein"
#define M_XTOOLARGE             "x-Koordinate zu gross"
#define M_YTOOSMALL             "y-Koordinate zu klein"
#define M_YTOOLARGE             "y-Koordinate zu gross"
#define M_ZTOOSMALL             "z-Koordinate zu klein"
#define M_ZTOOLARGE             "z-Koordinate zu gross"
#define M_VALTOOSMALL           "Pixelwert zu klein"
#define M_VALTOOLARGE           "Pixelwert zu gross"

#define M_X_OUT_OF_RANGE        "x-Koordinate ungültig"
#define M_Y_OUT_OF_RANGE        "y-Koordinate ungültig"
#define M_Z_OUT_OF_RANGE        "z-Koordinate ungültig"
#define M_VAL_OUT_OF_RANGE      "Wert ungültig"

#define M_INVALID_STRUCT  "Ungültige Datenstruktur"

#define M_VECTORDIM             "Falsche Vektor-Dimension"
#define M_MATRIXFORMAT          "Falsches Matrixformat"

// general errors
#define M_NOT_INITIALISED       "Nicht initialisiert"
#define M_ALREADY_INITIALIZED   "Initialisierung bereits abgeschlossen"
#define M_NO_DATA               "Keine Daten"
#define M_WRONG_CODING          "Falsche String-Kodierung (utf8 benutzen)"

// classifiers
#define M_NOT_TRAINED           "Klassifikator ist nicht ausreichend angelernt"
#define M_NOT_FINISHED          "Anlernen nicht abgeschlossen"

#define M_INVALID_CLASSNUMBER   "Klassennummer unzulässig"
#define M_INVALID_NCLASSES      "Ungültige Klassenanzahl"

// String Operations
#define M_WRONG_STRPTR    "Unzulaessiger Stringpointer"
#define M_WRONG_FORMAT          "Unzulaessiges Stringformat"
// #define M_STR_TO_LONG  "String zu lang"

// Files
#define M_WRONG_FILE    "Dateifehler"
#define M_FILE_OPEN   "Fehler beim Oeffnen der Datei "
#define M_WRONG_WRITE   "Fehler beim Schreiben in die Datei "
#define M_WRONG_FILETYPE        "Falscher Dateityp"
#define M_UNSUPPORTED_FILE      "Nicht unterstuetzter Dateityp"
#define M_ALREADY_OPEN          "Schon geoeffnet"
#define M_NOT_OPEN              "Nicht geoeffnet"

// Visualisation
#define M_NOT_VIS   "Bild nicht zur Visualisierung angemeldet"
#define M_NOT_AVAILABLE         "Option nicht verfuegbar"
#define M_ALREADY_VISIBLE       "Bild bereits visualisiert"
#define M_HIGHRANGE_VIS         "Wertumfang zu gross - Show(GRAY,..) benutzen"

// Fehler bei Bildeingabegeräten
#define M_NO_SCANDEVICE         "Kein Bildaufnahme-Geraet vorhanden"
#define M_WRONG_CONFIG          "Konfigurations-Fehler der Bildaufnahme-Geraete"
#define M_NO_PREVIEW            "Keine Voransicht moeglich"
#define M_SCAN_ERROR            "Fehler bei Bildaufnahme"

  /* Arbeit mit Bildern */
#define M_WRONG_IMAGE     "Unzulaessiges Bild"
#define M_WRONG_IMAGED    "Unzulaessiges Gleitkomma-Bild"
#define M_WRONG_IMGSIZE   "Falsche Bildgroesse"
#define M_IMAGE_TOO_LARGE       "Bild ist zu gross"
#define M_SIZES_DIFFER          "Bilder sind unterschiedlich gross"
#define M_SAME_IMAGE            "Bilder sind identisch"
#define M_LOWRANGE              "Pixelwertumfang zu gering"
#define M_HIGHRANGE             "Pixelwertumfang zu gross"
#define M_INVALID               "Bild ungueltig"
#define M_OUTSIDE           "Punkt ausserhalb des Bildes"

#define M_MODE_NOT_APPLYABLE    "Modus nicht anwendbar"

#define M_WRONG_CS              "Falscher Farbraum"
#define M_NOT_CONVERTIBLE       "Angeforderter Kanal nicht konvertierbar"
#define M_WRONG_RANGE           "Wertebereiche unterschiedlich"
#define M_CS_DIFFER             "Unterschiedliche Farbraeume"

  /* Arbeit mit Fenstern */
#define M_WRONG_WINDOW    "Passt nicht in aktuellen Fensterbereich"
#define M_WRONG_WINDOW2   "Gemeinsamer Fensterbereich zu klein"
#define M_WRONG_WINDOW3   "Unzulaessiger Fensterbereich"

  /* Konturen*/
#define M_INVALID_CONTUR        "Kontur ist ungueltig"
#define M_WRONG_STARTPOINT  "Unzulaessiger Startpunkt"
#define M_WRONG_STARTPOINT2   "Isolierter Startpunkt"
#define M_WRONG_STARTPOINT3   "Startpunkt liegt im Objekt"
#define M_NO_SEGMENT    "Kein Segment in Kontur gefunden"
#define M_CONTUR_NOT_CLOSED "Kontur nicht geschlossen"

  /* Numerik */
#define M_DIVISION_ZERO   "Division durch Null"
#define M_NO_SOLUTION   "Keine Loesung vorhanden"
#define M_SOL_MANIFOLD          "Loesungsvielfalt"
#define M_NUM_INSTABILITY "Numerische Instabilitaet"
#define M_ZERO_VECTOR           "Nullvektor"
#define M_NO_ORTHO    "Keine Orthonormalmatrix"
#define M_WRONG_VECTOR          "Unzulaessiger Vektorpointer"
#define M_ZERO_DET      "Nicht loesbar (Determinante 0)"
#define M_WRONG_MATRIX          "Unzulaessige Matrix"
#define M_MAT_NO_COMPAT         "Matrizen nicht kompatibel"
#define M_MATRIX_SINGULAR       "Matrix ist singulär"
#define M_WRONG_MATRIXTYPE      "Falscher Matrix-Typ"
#define M_NO_REGULAR            "Matrix ist nicht regulaer"
#define M_NO_SYMM               "Matrix ist nicht symmetrisch"
#define M_NO_SQUARE             "Matrix ist nicht quadratisch"
#define M_VEC_DEPEND    "Vektoren nicht unabhaengig"
#define M_NO_INVERSE    "Inverse existiert nicht"
#define M_WRONG_START   "Ungeeigneter Startwert"
#define M_POINT_IDENTIC   "Punkte identisch"
#define M_WRONG_POINTS    "Unzulaessige Punktauswahl"
#define M_WRONG_TRANS   "Unzulaessige Transformationsmatrix"
#define M_NO_PROJ               "Keine projektive Matrix (dritte Zeile 0)"
#define M_NO_NULL               "Keine Nullstelle"

// geometrische Transformationen, Matching
#define M_WRONG_OBJECT          "Leeres Objekt oder negatives Gebiet"
#define M_WRONG_TRAFO           "Entartete Transformations-Matrix"
#define M_TRAFO_NOTAFFINE       "Transformation ist nicht affine"
#define M_OBJECT_FEATURE        "Objekt-Merkmale können nicht berechnet werden"

// Histogramme
#define M_HIST_EMPTY            "Histogramm enthaelt keine Daten"

// Klassifikatoren
#define M_CLASSPTR        "Nicht initialisierte Klassifikatorstruktur"
#define M_TRAIN           "Unvollstaendig angelernter Klassifikator"
#define M_CLASS         "Falsche Klassenangabe"
#define M_TWOCLASSES            "Keine zwei Klassen vorhanden"

// Statistik
#define M_STATIST_PTR       "Nicht initialisierter Statistikbereich"
#define M_STAT_NOENTRY    "Keine Eintraege im Statistikbereich"
#define M_ZERO_VARIANZ    "Eine Komponente besitzt Varianz 0"
#define M_EMPTY_STAT    "Leerer Statistikbereich"

// Kalibrierung - Rekonstruktion - multiokulare BV (Triangulation)
#define M_PARALLEL              "Parallele Geraden"
#define M_CALIBERR        "Kalibrierung aus Punktkorrespondenzen nicht moeglich"
#define M_PRJZ            "Projektionszentrum nicht berechenbar"
#define M_MISS_P          "Anzahl Punktkorrespondenzen zu klein"
#define M_UNCALIBRATED_CAM      "Kamera ist nicht kalibriert"
#define M_TOO_FEW_CAM           "unzureichende Anzahl an Kamerastrukturen"
#define M_NO_DISTANCE           "Distanz nicht berechenbar"
#define M_WRONG_CAMERA_POS  "Kameraposition ueberpruefen!"
#define M_NO_ELLIPSE    "keine Ellipse"
#define M_INFINIT_POINT   "Abbildung in unendlich fernen Punkt"

#define M_NOT_NESTED            "Funktion kann nicht verschachtelt gerufen werden!"

// fuer wxwindows
#define M_OVERFLOW_INT          "Ueberschreitung des Integerbereiches"
#define M_OVERFLOW_DOUBLE       "Ueberschreitung des double-Bereiches"

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
//#define DISABLE   3
//#define REORG   4
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

// #define GRAY 20


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

  /* Farben */
#define C_BLACK         0 /* nur fuer Alpha */
#define C_BROWN         0 /* nur fuer Graphic */
#define C_RED           1
#define C_GREEN         2
#define C_BLUE          3
#define C_YELLOW        4
#define C_CYAN          5
#define C_MAGENTA       6
#define C_PINK          7 /* nur fuer Graphic */
#define C_WHITE         7 /* nur fuer Alpha */

  /* Defaultwerte */
  /* defaults für implicit angelegte Bilder, wenn Maxval nicht anders
     ermittelbar ist */

#define DEFAULT_IMG_MAXVAL 255

#define MODE_NEW 1
#define MODE_COPY 2
#define MODE_REPLACE 3

}
#endif /*_DEFS_H*/

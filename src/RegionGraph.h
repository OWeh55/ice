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

// Author: Stephan Wagner, Feb. 2005

#ifndef REGIONGRAPH_H
#define REGIONGRAPH_H

#include <vector>
#include <deque>
#include <set>
#include "base.h"

using namespace std;
namespace ice
{

// Makrodefinition fuer 8-er Nachbarschaft
// jetzt 12 Zuweisungen, vorher 18, 8 Vergleiche
// normaler 8-er Nachbarschaftsansatz: 8*2=16 Zuweisungen und 8*4=32 Vergleiche
#define ForAll8Nbrs(p,x,y,x_nb,y_nb,code) \
{\
  if (x>0){\
    y_nb=y;\
    x_nb=x-1;\
    code;\
    if (y>0) { y_nb=y-1; code;}\
    if (y<p->ysize-1) { y_nb=y+1; code;}\
  }\
  if (x<p->xsize-1) {\
    y_nb=y;\
    x_nb=x+1;\
    code;\
    if (y>0) { y_nb=y-1; code;}\
    if (y<p->ysize-1) { y_nb=y+1; code;}\
  }\
  if (y>0) { y_nb=y-1; x_nb=x; code;}\
  if (y<(p->ysize-1)) { y_nb=y+1; x_nb=x; code;}\
}\


// Makrodefinition fuer 4-er Nachbarschaft
// jetzt 4 Zuweisungen, 4 Vergleiche
#define ForAll4Nbrs(p,x,y,x_nb,y_nb,code) \
{\
  if (x>0) { y_nb=y; x_nb=x-1; code;}\
  if (x<(p->xsize-1)) { y_nb=y; x_nb=x+1; code;}\
  if (y>0) { y_nb=y-1; x_nb=x; code;}\
  if (y<(p->ysize-1)) { y_nb=y+1; x_nb=x; code;}\
}\

  struct STPoint
  {
    // Punktstruktur
    int x;
    int y;
    int grw;
  };

  typedef vector<struct STPoint> PointVECTOR;
  typedef deque<struct STPoint> FIFOList;

  typedef deque<int> INT_FIFOList;
  typedef vector<int, allocator<int> > INTVECTOR;

// Graphenknoten
  class Node
  {

  public:
    int Label;
    int Degree;     // Anzahl der Nachbarn

    INTVECTOR Nb;   // Nachbarschaftsliste des Knotens

    int GetDegree();
    void SetLabel(int Label);

    bool IsConnected(int NodeNumber);  // Knoten mit Knoten NodeNumber verbunden?
    void AddEdge(int NodeNumber);    // Verbindung zu Knoten NodeNumber hinzufuegen
    bool DeleteEdge(int Number);     // Nachbarschaft loeschen

    Node();
    virtual ~Node();
  };

// Knoten des Regionengraphs
  class RegionNode: public Node
  {
  public:

    int PixelAnz;     // Anzahl der Pixel

    long GrwSum;
    float GrwQuadSum;

    int GrwMw;        // durchschnittlicher Grauwert der Region
    float GrwVar;     // Grauwertvarianz

    int GrwMin;       // minimaler Grauwert
    int GrwMax;       // maximaler Grauwert

    PointVECTOR WSHDPixel;  // Wasserscheidenpixel
    PointVECTOR RegionPixel;// Regionenpixel

    void CompGRWMW();   // durchschnittlichen Grauwert der Region berechnen
    void CompGRWVAR();    // Grauwertvarianz berechnen

    RegionNode();
    virtual ~RegionNode();

    void drawRegion(Image working, int color);   // Region einzeichnen
    void drawWSHDPixel(Image working, int color);// Wasserscheidenpixel einzeichnen

    void CompNeighborRegions8(Image WorkingImg);// Nachbarschaft der Regionen berechnen
    void CompNeighborRegions4(Image WorkingImg);
  };

// Knoten des Kantengraphs
  class EdgeNode: public Node
  {
  public:

    int Reg1;   // erste angrenzende Region
    int Reg2;   // zweite angrenzende Region
    bool WSHED;
    int Status;
    int Distance;
    int Index;

    int ref;    // wenn Region schon verschmolzen, also nicht mehr aktiv, Verweis auf die aktuelle Region
    bool deleted;

    int GRWWert;  // Differenz der Grauwerte der angrenzenden Regionen
    long WSPGRWSum;

    PointVECTOR WSHDPixel;  // Wasserscheidenpixel, die zwischen den beiden Regionen liegen

    void drawWSHDPixel(Image working, int color);

    EdgeNode();
    virtual ~EdgeNode();

  };

// ------------ Ende Knotendefinitionen ---------------

// Definition Kantengraph
  class EdgeGraph
  {
  public:

    struct EdgeInfo
    {
      int to;
      int IndexNbr;
    } einfo;

    int EdgeNodeCount;

    Image CopiedSourceImg;

    typedef vector<EdgeNode*> EdgeVector;

    typedef vector<int, allocator<int> > INTVECTOR;
    INTVECTOR::iterator IntIter;

    typedef vector<struct EdgeInfo> EdgeAddressVECTOR;
    EdgeAddressVECTOR::iterator EdgeAddressIter;

    EdgeAddressVECTOR EdgeNodeAccessLUT;

    INTVECTOR RegionAccessLUT;
    INTVECTOR SortedEdgeNodeAccessLUT;

    INTVECTOR Sorted[256];  // Listenarray zum Sortieren initialisieren

    INT_FIFOList Q;

    EdgeVector edges;

    int current_label;
    int current_dist;

    bool compNb1(int node); // Berechnung 1.1. der Wasserscheidentransformation auf Graphen
    bool compNb2(int node); // Berechnung 1.2. der Wasserscheidentransformation auf Graphen
    bool compNb3(int node); // Berechnung 1.3. der Wasserscheidentransformation auf Graphen
    void ComputeWatershedTrans();// Wasserscheidentransformation auf Kantengraph ausfuehren
    // Wasserscheidenkanten nach WST-Transformation einzeichnen
    void drawAllWSHEDs(Image i, Image& wsheds, int tresh);
    void drawWSHEDs(Image i1);

    int  getIndex(int r1, int r2);
    bool setIndex(int r1, int r2, int index);
    // Wasserscheidenpixel untersuchen und angrenzende Regionen feststellen
    void distributeToEdges8(Image WorkingImg, int x, int y);

    EdgeGraph();
    virtual ~EdgeGraph();
  };

  class RegionGraph
  {
  public:
    Image CopiedSourceImg;
    Image CopiedLabImg;
    Image WorkingImg;

    typedef vector<RegionNode*> RegionVector;
    RegionVector regions;   // Vektor von Regionenknoten

    PointVECTOR  WShedPixels; // Vektor fuer alle Wasserscheidenpixel

    int RegNodeCount;

    void DeleteRegion(int RegNumber);

    int BuildGraph(Image& ret);// Knotengraph aufbauen

    void CompRegion4(Image sourceImg, Image labImg, Image workImg, int x, int y, int gr);

    EdgeGraph* EG;        // den Kantengraph aus dem Regionengraph erstellen
    EdgeGraph* ComputeEdgeGraph();

    RegionGraph();
    RegionGraph(Image source, Image labImg, Image& retImg);
    virtual ~RegionGraph();
  };

// vollstaendige Grapheniteration durchfuehren:
// 1. Regionengraph berechnen
// 2. in Kantengraph transformieren
// 3. WST auf Kantengraph ausfuehren
// 4. Kantengraph in Regionenbild ueberfuehren
  int GraphIter(Image Original, Image WSImg, Image& GrwImg, int Treshold);

  int DGrwImage(Image Original, Image WSImg, Image& GrwImg);

}
#endif

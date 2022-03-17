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
    void setLabel(int Label);

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

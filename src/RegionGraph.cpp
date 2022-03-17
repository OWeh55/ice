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

#include <stdlib.h>

#include "macro.h"

#include "RegionGraph.h"

// ------------------------------------------------------------------------

namespace ice
{

  Node::Node()
  {

    Label = 0;
  }

  Node::~Node()
  {

    Nb.clear();
  }

// Regionenknoten-Konstruktor
  RegionNode::RegionNode()
  {
    Label = 0;

    PixelAnz = 0;

    GrwMw = 0;
    GrwSum = 0;

    GrwQuadSum = 0;
    GrwVar = 0;

    GrwMin = 0;
    GrwMax = 0;
  }

// Regionenknoten-Destruktor
  RegionNode::~RegionNode()
  {

    WSHDPixel.clear();
    RegionPixel.clear();
  }

// Kantenknoten-Konstruktor
  EdgeNode::EdgeNode()
  {

    Reg1 = -1;
    Reg2 = -1;
    WSHED = false;
    Status = -2;
    Distance = 0;
    GRWWert = -1;
    Index = -1;
    Label = 0;
    WSPGRWSum = 0;
    deleted = false;
  }

// Kantenknoten-Destruktor
  EdgeNode::~EdgeNode()
  {

    WSHDPixel.clear();
  }

// hat der Knoten den Knoten mit der Nummer NodeNumber als Nachbarn?
  bool Node::IsConnected(int NodeNumber)
  {

    int i;

    if (!Nb.empty())
      for (i = 0; i < int(Nb.size()); i++) if (Nb[i] == NodeNumber)
          {
            return true;
          }

    return false;
  }

// Knotengrad zurueckliefern
  int Node::GetDegree()
  {

    return Degree;
  }

// Knotennummer vergeben
  void Node::setLabel(int number)
  {

    Label = number;
  }

// ------------------- Anfang der Funktionen, die die NodeList veraendern --------------

// Nachbar zu Knoten hinzufuegen, Knotennummer in Nachbarliste eintragen
  void Node::AddEdge(int NodeNumber)
  {

    INTVECTOR::iterator IntIter;

    if (NodeNumber != -1)
      {
        if (!Nb.empty())
          for (IntIter = Nb.begin(); IntIter != Nb.end(); ++IntIter) if (*IntIter == NodeNumber)
              {
                return;
              }

        Nb.push_back(NodeNumber);
        Degree = Nb.size(); // Knotengrad erhoehen
      }
  }

// Nachbar eines Knotens loeschen
  bool Node::DeleteEdge(int Number)
  {

    if (!Nb.empty())
      {
        int i;

        for (i = 0; i < int(Nb.size()); i++)
          if (Nb[i] == Number)
            {
              Nb.erase(Nb.begin() + i);  // Knotengrad aktualisieren
              Degree--;
            }

        return true;
      }
    else
      {
        return false;
      }

    return true;
  }

// Wasserscheidenpixel des Kanten-Knoten in working-Bild einzeichnen
  void EdgeNode::drawWSHDPixel(Image working, int color)
  {

    int n;

    if (!WSHDPixel.empty())
      for (n = 0; n < int(WSHDPixel.size()); n++)
        {
          struct STPoint temp = WSHDPixel[n];
          PutVal(working, temp.x, temp.y, color);
        }
  }

// ------------------- Ende der Funktionen, die die NodeList veraendern --------------

// durchschnittlichen Grauwert der Region berechnen
  void RegionNode::CompGRWMW()
  {
    if (PixelAnz == 0)
      {
        GrwMw = 0;
      }
    else
      {
        GrwMw = GrwSum / PixelAnz;
      }
  }

// Varianz der Grauwerte der Region berechnen
  void RegionNode::CompGRWVAR()
  {

    int n;
    GrwQuadSum = 0;

    if (!RegionPixel.empty())
      for (n = 0; n < int(RegionPixel.size()); n++)
        {
          struct STPoint temp = RegionPixel[n];
          GrwQuadSum += (float)pow((float)temp.grw - GrwMw, 2);
        }

    if (PixelAnz == 1 || PixelAnz == 0)
      {
        GrwVar = 0;  // bei nur einem Pixel, ist Varianz Null
      }
    else
      {
        GrwVar = sqrt((float)1 / (PixelAnz - 1) * GrwQuadSum);
      }
  }

// Regionenpixel in working-Bild einzeichnen
  void RegionNode::drawRegion(Image working, int color)
  {

    int n;

    if (!RegionPixel.empty())

      // alle Regionenpixel betrachten
      for (n = 0; n < int(RegionPixel.size()); n++)
        {
          struct STPoint temp = RegionPixel[n];
          PutVal(working, temp.x, temp.y, color);
        }
  }

// Waserscheidenpixel der Region in das working-Bild einzeichnen
  void RegionNode::drawWSHDPixel(Image working, int color)
  {

    int n;

    if (!WSHDPixel.empty())

      // alle WS-Pixel betrachten
      for (n = 0; n < int(WSHDPixel.size()); n++)
        {
          struct STPoint temp = WSHDPixel[n];
          PutVal(working, temp.x, temp.y, color);
        }
  }

// zur Region angrenzende Regionen feststellen und in Nachbarschaftsliste einfuegen
// Grundlage: 8er Nachbarschaft
  void RegionNode::CompNeighborRegions8(Image WorkingImg)
  {

    int x, y;
    int n;
    int xn, yn;

    // alle Wasserscheidenpunkt betrachten
    for (n = 0; n < int(WSHDPixel.size()); n++)
      {
        struct STPoint temp = WSHDPixel[n];
        x = temp.x;
        y = temp.y;
        // Nachbarn betrachten
        ForAll8Nbrs(WorkingImg, x, y, xn, yn,
        {
          if (Label != GetVal(WorkingImg, xn, yn))
            // wenn Nachbarpunkt in anderer Region liegt, dann in die Nachbarschaftsliste des Knotens einfuegen
            AddEdge(GetVal(WorkingImg, xn, yn) - 1);
        };)
      }
  }

// zur Region angrenzende Regionen feststellen und in Nachbarschaftsliste einfuegen
// Grundlage: 4er Nachbarschaft
  void RegionNode::CompNeighborRegions4(Image WorkingImg)
  {

    int n;
    int x, y;
    int xn, yn;

    // alle Wasserscheidenpunkt betrachten
    for (n = 0; n < int(WSHDPixel.size()); n++)
      {
        struct STPoint temp = WSHDPixel[n];
        x = temp.x;
        y = temp.y;
        // Nachbarn betrachten
        ForAll4Nbrs(WorkingImg, x, y, xn, yn,
        {
          if (Label != GetVal(WorkingImg, xn, yn))
            // wenn Nachbarpunkt in anderer Region liegt, dann in die Nachbarschaftsliste des Knotens einfuegen
            AddEdge(GetVal(WorkingImg, xn, yn) - 1);
        };)
      }
  }

// Ende - Funktionen Klasse "Region"
// -------------------------------------------------------------------------------------------
// Anfang - Funktionen Klasse "RegionGraph"

  RegionGraph::RegionGraph()
  {

    RegNodeCount = 0;
  }

// Regionengraph mit Bilder initialisieren
#define FNAME "RegionGraph()"
  RegionGraph::RegionGraph(Image source, Image labImg, Image& retImg)
  {

    RegNodeCount = 0;

    if (!IsImg(source) || !IsImg(labImg) || !IsImg(retImg))
      {
        // Eingabebilder gültig?
        throw IceException(FNAME, M_INVALID_IMAGE);
      }

    CopiedLabImg = NewImg(labImg, true);  //CopyImg(labImg,CopiedLabImg);

    CopiedSourceImg = NewImg(source, true); //CopyImg(source,CopiedSourceImg);

    WorkingImg = NewImg(labImg);
    WorkingImg.set(0);

    BuildGraph(retImg);
  }
#undef FNAME

// Regionengraph-Destruktor
  RegionGraph::~RegionGraph()
  {

    WShedPixels.clear();
  }

// Kantengraph-Konstruktor
  EdgeGraph::EdgeGraph()
  {

    EdgeNodeCount = 0;
    current_label = 0;
    current_dist = 0;
  }

// Kantengraph-Destruktor
  EdgeGraph::~EdgeGraph()
  {

    edges.clear();
    Q.clear();
    EdgeNodeAccessLUT.clear();
    SortedEdgeNodeAccessLUT.clear();
    RegionAccessLUT.clear();
  }

// Region aus Graph entfernen
  void RegionGraph::DeleteRegion(int RegNumber)
  {
    if (regions[RegNumber])
      {
        regions[RegNumber] = nullptr;
      }
  }

  int EdgeGraph::getIndex(int r1, int r2)
  {

    // Startadresse aus erstem Vektor holen
    int startR1 = RegionAccessLUT[r1];
    int endR1 = RegionAccessLUT[r1 + 1];

    int i;    // jetzt: in zweitem Vektor ab VEK[r1] bzw. VEK[r2] suchen

    for (i = startR1; i < endR1; i++)
      {
        if (EdgeNodeAccessLUT[i].to == r2)
          {
            return EdgeNodeAccessLUT[i].IndexNbr;
          }
      }

    return -1;
  }

  bool EdgeGraph::setIndex(int r1, int r2, int index)
  {

    int start = RegionAccessLUT[r1];
    int end = RegionAccessLUT[r1 + 1];

    int i;

    for (i = start; i < end; i++)
      {
        if (EdgeNodeAccessLUT[i].to == r2)
          {
            EdgeNodeAccessLUT[i].IndexNbr = index;
            return true;
          }
      }

    return false;
  }

// Nachbarschaft der Wasserscheidenpixel feststellen
  void EdgeGraph::distributeToEdges8(Image WorkingImg, int x, int y)
  {

    set<int> nb_set;
    set<int>::iterator it;
    set<int>::iterator it2;

    int xn, yn;
    // alle Nachbarregionen in Set eintragen
    ForAll8Nbrs(WorkingImg, x, y, xn, yn,
    {
      if (GetVal(WorkingImg, xn, yn) != 0)
        {
          nb_set.insert(GetVal(WorkingImg, xn, yn));
        }
    });

    int index = -1;

    for (it = nb_set.begin(); it != nb_set.end(); it++)
      {
        it2 = it;
        it2++;

        for (it2 = it2; it2 != nb_set.end(); it2++)
          {
            index = getIndex(*it - 1, *it2 - 1);

            if (index >= 0)
              {
                // Wasserscheidenpixel Kantenknoten zuteilen
                struct STPoint temp = {x, y, 0};
                edges[index]->WSHDPixel.push_back(temp);
                edges[index]->WSPGRWSum += GetVal(CopiedSourceImg, x, y);
              }
          }
      }
  }

// Wasserscheidentransformation auf Kantengraph
#define FNAME "ComputeEdgeGraph"
  EdgeGraph* RegionGraph::ComputeEdgeGraph()
  {

    EG = new EdgeGraph();
    EG->CopiedSourceImg = NewImg(CopiedSourceImg);
    CopyImg(CopiedSourceImg, EG->CopiedSourceImg);

    int degreeSum = 0;
    int i, n;

    // Look-Up-Tabellen zur Graphentransformation berechnen
    for (i = 0; i < RegNodeCount; i++)
      {
        // wieviel Nachbarn hat Region i, d.h. wieviele "Nachbarn" gibt es im Graphen insgesamt
        EG->RegionAccessLUT.push_back(degreeSum);
        degreeSum += regions[i]->GetDegree();

        struct EdgeGraph::EdgeInfo tempEdgeInfo;
        tempEdgeInfo.IndexNbr = -1;
        tempEdgeInfo.to = -1;

        // soviele Eintraege fuer Region i in LUT wie Nachbarn
        for (n = 0; n < int(regions[i]->Nb.size()); n++)
          {
            tempEdgeInfo.to = regions[i]->Nb[n];
            EG->EdgeNodeAccessLUT.push_back(tempEdgeInfo);
          }
      }

    EG->RegionAccessLUT.push_back(degreeSum);  // wichtig! Index des letzten Elements

    EdgeNode* en = new EdgeNode();
    EG->EdgeNodeCount = 0;

    // Graphentransformation
    // Kantenknoten in Graph einfuegen, zuerst alle Regionenknoten untersuchen
    for (i = 0;   i < RegNodeCount; i++)
      {
        for (n = 0; n < int(regions[i]->Nb.size()); n++)
          {

            int nb = regions[i]->Nb[n];

            if (EG->getIndex(i, nb) == -1)
              {
                // mittleren Grauwert bzw. Differenz der beiden Regionen abspeichern
                en = new EdgeNode();
                // Grauwertdifferenz zwischen den Regionen berechnen
                en->GRWWert = (abs(regions[i]->GrwMw - regions[nb]->GrwMw)); // int GrwMw; + float GrwVar;

                en->Reg1 = i;
                en->Reg2 = nb;
                // neuen Kanten-Knoten zu Graph hinzufuegen
                EG->edges.push_back(en);

                EG->edges[i]->Index = EG->EdgeNodeCount;
                // Zugriff auf Kante zwischen i und nb moeglich machen
                RETURN_NULL_IF_FAILED(EG->setIndex(i, nb, EG->EdgeNodeCount));
                RETURN_NULL_IF_FAILED(EG->setIndex(nb, i, EG->EdgeNodeCount));

                EG->EdgeNodeCount++;
              }
          }
      }

    // Kantengraph berechnet!

    // Wasserscheidenpixel zu Kanten hinzufuegen
    // Wasserscheidenpixel untersuchen und Nachbarschaftsbeziehungen der Pixel feststellen

    if (!WShedPixels.empty())
      for (int n = 0; n < int(WShedPixels.size()); n++)
        {
          struct STPoint temp = WShedPixels[n];
          // Wasserscheidenpixel zu Kanten zuordnen
          EG->distributeToEdges8(WorkingImg, temp.x, temp.y);
        }

    // Kantenknoten aufsteigend sortieren (Distribution-Sort=Bucket-Sort wie bei urspruenglichem Ansatz)
    for (i = 0;   i < EG->EdgeNodeCount; i++)
      {

        EdgeNode* en = EG->edges[i];
        int r1 = en->Reg1;
        int r2 = en->Reg2;

        int j;

        for (j = EG->RegionAccessLUT[r1]; j < EG->RegionAccessLUT[r1 + 1]; j++)
          if (EG->EdgeNodeAccessLUT[j].to != r2)
            {
              en->AddEdge(EG->EdgeNodeAccessLUT[j].IndexNbr);
            }

        for (j = EG->RegionAccessLUT[r2]; j < EG->RegionAccessLUT[r2 + 1]; j++)
          if (EG->EdgeNodeAccessLUT[j].to != r1)
            {
              en->AddEdge(EG->EdgeNodeAccessLUT[j].IndexNbr);
            }

        EG->Sorted[en->GRWWert].push_back(i);
      }

    return EG;
  }
#undef FNAME

// Berechnung 1.1. der Wasserscheidentransformation auf Graphen
  bool EdgeGraph::compNb1(int node)
  {

    int n;

    for (n = 0; n < int(edges[node]->Nb.size()); n++)
      {
        int nb = edges[node]->Nb[n];

        // Knoten hat Wasserscheiden oder Becken als Nachbar?
        if (edges[nb]->Status == -3 || edges[nb]->Status > 0)
          {
            return true;
          }
      }

    return false;
  }

// Berechnung 1.2. der Wasserscheidentransformation auf Graphen
  bool EdgeGraph::compNb2(int node)
  {

    int n;
    bool WshedNb = false;

    // Nachbarschaft des Knotens untersuchen
    for (n = 0; n < int(edges[node]->Nb.size()); n++)
      {
        int nb = edges[node]->Nb[n];

        // Nachbar ist anderem Minimum nicht naeher und Nachbar ist anderem Becken zugehoerig und Knoten noch nicht verarbeitet
        if ((edges[nb]->Distance <= current_dist) && ((edges[nb]->Status > 0) || edges[node]->Status == -1))
          {
            // Nachbar Becken zugehoerig
            if (edges[nb]->Status > 0)
              {
                // Knoten noch nicht verarbeitet oder Wasserscheide
                if ((edges[node]->Status == -1) || (edges[node]->Status == -3 && WshedNb))
                  // Nachbarknoten ist Becken zugehoerig, Knoten mit selbem Label versehen
                  {
                    edges[node]->Status = edges[nb]->Status;
                  }
                else   // 2 Becken treffen aufeinander
                  if (edges[nb]->Status != edges[node]->Status)
                    {
                      edges[node]->Status = -3;
                      // Labels unterschiedlich, Trennung durch Wasserscheide in Knoten
                      WshedNb = false;
                    }
              }     // Nachbar ist Wasserscheide, Knoten noch nicht verarbeitet, Knoten wird Wasserscheide
            else if (edges[node]->Status == -1)
              {
                edges[node]->Status = -3;
                WshedNb = true;
              }
          }
        else      // Nachbar in der gleichen Hoehenstufe, aber noch nicht verarbeitet
          if ((edges[nb]->Status == -1) && (edges[nb]->Distance == 0))
            {
              // Distanz erhoehen und in Schlange aufnehmen
              edges[nb]->Distance++;
              Q.push_back(nb);
            }
      }

    return true;
  }

// Berechnung 1.3. der Wasserscheidentransformation auf Graphen
  bool EdgeGraph::compNb3(int node)
  {

    int n;          // neue Minima eintragen und Label vergeben

    for (n = 0; n < int(edges[node]->Nb.size()); n++)
      {
        int nb = edges[node]->Nb[n];

        if (edges[nb]->Status == -1)
          {
            edges[nb]->Status = current_label;
            Q.push_back(nb);
          }
      }

    return true;
  }

// Wasserscheidentransformation auf Kantengraph anwenden
  void EdgeGraph::ComputeWatershedTrans()
  {

    int k, i;
    current_dist = 0;
    current_label = 0;

// 1. von Level 0 bis Level GRWMax
    for (k = 0; k < 256; k++)
      {
        // 1.1. fuer jedes Level Pixel in Schlange aufnehmen,
        //      wenn ein Nachbarschaftspixel schon zu einer Region zugehoerig ist oder Wasscheidenpixel ist
        for (i = 0; i < int(Sorted[k].size()); i++)
          {
            edges[Sorted[k][i]]->Status = -1;

            // Kante zu Beckenknoten  oder Wasserscheidenknoten benachbart?
            if (compNb1(Sorted[k][i]))
              {
                edges[Sorted[k][i]]->Distance = 1;
                Q.push_back(Sorted[k][i]);
              }
          }

        // 1.2. Punkte, die an Regionengrenzen liegen, verarbeiten
        //    aktuelle Distanz von Region ist 1
        // Distanz-Trennelement in FIFO-Schlange einfuegen
        Q.push_back(-1);

        if (!Q.empty())
          do      // Kopfelement der Schlange lesen
            {
              int ind = Q[0];
              Q.pop_front();

              // Element ist Distanz-Trennelement?
              if (ind == -1)
                {
                  // letztes Element?
                  if (Q.empty())
                    {
                      goto ende;
                    }
                  else
                    {
                      // Distanz erhoehen
                      current_dist++;
                      // Distanz-Trennelement fuer naechste Distanzstufe einfuegen
                      Q.push_back(-1);
                      ind = Q[0];
                      Q.pop_front();
                    }
                }

              // Status des aktuellen Kantenknoten berechnen
              compNb2(ind);
            }
          while (!Q.empty());

ende:
        // 1.3. Punkte, die isoliert liegen, d.h. noch nicht in 1.1. oder 1.2. verarbeitet wurden
        //      als neue Minima in Markierungsbild m1 mit neuem Label eintragen und Einlfusszone
        //      der Minima berechnen (alle angrenzenden Punkte der gleichen Hoehenstufe)

        for (i = 0; i < int(Sorted[k].size()); i++)
          {
            // neue Minima eintragen
            edges[Sorted[k][i]]->Distance = 0;

            if (edges[Sorted[k][i]]->Status == -1)
              {
                // neues Label vergeben
                current_label++;

                Q.push_back(Sorted[k][i]);
                // Becken rund um neues Minima fuellen
                edges[Sorted[k][i]]->Status = current_label;

                if (!Q.empty())
                  do
                    {
                      int ind = Q[0];
                      Q.pop_front();
                      // Nachbarpixel schon verarbeitet? wenn nicht, in Schlange aufnehmen
                      compNb3(ind);
                    }
                  while (!Q.empty());
              }
          }
      }
  }

// Wasserscheidenlinine zwischen den angrenzenden Regionen in Bild i1 einzeichnen
  void EdgeGraph::drawWSHEDs(Image i1)
  {

    int k, i;
    i1.set(i1.maxval);

    for (k = 0; k < 256; k++)
      for (i = 0; i < int(Sorted[k].size()); i++)

        {
          edges[Sorted[k][i]]->drawWSHDPixel(i1, 0);
        }
  }

// Wasserscheidenlinien einzeichnen, die nach der Wasserscheidentransformation
// auf dem Kantengraphen uebriggeblieben sind
  void EdgeGraph::drawAllWSHEDs(Image i1, Image& wsheds, int tresh)
  {

    int k, i;
    i1.set(i1.maxval);
    wsheds = NewImg(i1);
    wsheds.set(i1.maxval);

    for (k = 0; k < 256; k++)
      for (i = 0; i < int(Sorted[k].size()); i++)

        // Kante ist als Wasserscheide markiert?
        if (edges[Sorted[k][i]]->Status == -3)
          {
            // Wasserscheidenlinie zw. den angrenzenden Regionen einzeichnen
            edges[Sorted[k][i]]->drawWSHDPixel(wsheds, 0);
          }
        else

          // Wasserscheidenlinie einzeichnen, wenn Grauwertdifferenz zwischen den Regionen
          // groesser als der "tresh"-Schwellwert ist
          if (edges[Sorted[k][i]]->GRWWert > tresh)
            {
              edges[Sorted[k][i]]->drawWSHDPixel(wsheds, 0);
            }

  }

  void RegionGraph::CompRegion4(Image sourceImg, Image labImg, Image workImg, int x, int y, int RegionNbr)
  {

    int n;
    RegionNode* currReg = regions[RegionNbr];
    struct STPoint temp = {x, y, GetVal(sourceImg, x, y)};
    // aktuellen Start-Pixel zur RegionPixelliste hinzufuegen
    currReg->RegionPixel.push_back(temp);
    // Pixel als bearbeitet markieren
    PutVal(workImg, x, y, workImg.maxval);
    // Grauwertsumme initialisieren
    currReg->GrwSum = 0;
    currReg->PixelAnz++;// Pixelanzahl erhoehen

    regions[RegionNbr]->Label = GetVal(labImg, x, y);

    currReg->GrwMw = GetVal(sourceImg, x, y);
    currReg->GrwMin = currReg->GrwMw;
    currReg->GrwMax = currReg->GrwMw;

    struct STPoint t = { -1, -1, -1};

    int xpos, ypos;
    int grwNb;

    // erster Schleifendurchlauf mit Startpunkt x,y in RegionPixel, danach
    // Nachbarpixel der gefundenen Regionenpixel betrachten
    if (!currReg->RegionPixel.empty())
      for (n = 0; n < int(currReg->RegionPixel.size()); n++)
        {
          temp = currReg->RegionPixel[n];
          xpos = temp.x;
          ypos = temp.y;
          // Grauwert des Punktes xpos,ypos im Originalbild
          int grw = GetVal(sourceImg, xpos, ypos);

          // minimalen und maximalen Grauwert der Region ermitteln
          if (currReg->GrwMin < grw)
            {
              currReg->GrwMin = grw;
            }

          if (currReg->GrwMax > grw)
            {
              currReg->GrwMax = grw;
            }

          // Grauwert des Pixel zur Mittelwertsumme addieren
          currReg->GrwSum += grw;

          int xn, yn;
          // 4er Nachbarschaft des Regionenpunktes betrachten
          ForAll4Nbrs(workImg, xpos, ypos, xn, yn,
          {
            // Punkt schon verarbeitet?
            if ((GetVal(workImg, xn, yn) == 0))
              {
                // Punkt ist Regionenpixel?
                if (GetVal(labImg, xn, yn) != 0)   // Pixel != Wasserscheide?
                  {

                    grwNb = GetVal(sourceImg, xn, yn);

                    struct STPoint temp2 = t;
                    temp2.x = xn;
                    temp2.y = yn;
                    temp2.grw = grwNb;
                    // Punkt in RegionPixel aufnehmen
                    currReg->RegionPixel.push_back(temp2);
                    // Punkt als verarbeitet markieren und Gesamtpixelanzahl erhoehen
                    PutVal(workImg, xn, yn, workImg.maxval);
                    currReg->PixelAnz++;
                  }
                else
                  {
                    // Pixel ist Wasserscheidenpixel
                    struct STPoint temp2 = t;
                    temp2.x = xn;
                    temp2.y = yn;
                    temp2.grw = 0;
                    // Punkt in RegionPixel aufnehmen und als verarbeitet markieren
                    currReg->WSHDPixel.push_back(temp2);
                    PutVal(workImg, xn, yn, 1);
                  }
              }
          };
                     )
        }
  }

// Regionengraph berechnen
#define FNAME "BuildGraph"
  int RegionGraph::BuildGraph(Image& retImg)
  {
    // WorkingImg initialisieren
    WorkingImg.set(0);
    RegNodeCount = 0;

    if (!IsImg(retImg))
      throw IceException(FNAME, M_INVALID_IMAGE);

    for (int yw = 0; yw < CopiedSourceImg.ysize; yw++)
      for (int xw = 0; xw < CopiedSourceImg.xsize; xw++)
        {
          // Pixel (=Region) noch nicht bearbeitet
          if (GetVal(CopiedLabImg, xw, yw) != 0)
            {
              // Pixel ist keine Wasserscheide
              if (GetVal(WorkingImg, xw, yw) == 0)
                {
                  // neue Region einfuegen
                  regions.push_back(new RegionNode());
                  // alle Punkte berechnen, die in der Region liegen
                  CompRegion4(CopiedSourceImg, CopiedLabImg, WorkingImg, xw, yw, RegNodeCount);
                  // Regionenknoten etikettieren
                  regions[RegNodeCount]->setLabel(RegNodeCount + 1);
                  // durchschnittlichen Grauwert und Grauwertvarianz der Punkte der Regione berechnen
                  regions[RegNodeCount]->CompGRWMW();
                  regions[RegNodeCount]->CompGRWVAR();
                  // Wasserscheidenpixel einzeichnen
                  regions[RegNodeCount]->drawWSHDPixel(WorkingImg, 0);
                  // Regionenzaehler erhoehen
                  RegNodeCount++;
                }
            }
          else        // Pixel ist Wasserscheide
            {
              struct STPoint temp = {xw, yw, 0};
              // zu Wasserscheidenpixeln hinzufuegen
              WShedPixels.push_back(temp);

            }
        }

    WorkingImg.set(WorkingImg.maxval);

    if (!WShedPixels.empty())

      // Wasserscheidenpixel einzeichnen (neues Bild aufgrund Fehlerbeseitigung)
      for (int n = 0; n < int(WShedPixels.size()); n++)
        {
          struct STPoint temp = WShedPixels[n];
          PutVal(WorkingImg, temp.x, temp.y, 0);
        }

    WorkingImg = NewImg(WorkingImg->xsize, WorkingImg->ysize, RegNodeCount + 1);
    WorkingImg.set(0);

    for (int i = 0;   i < RegNodeCount; i++)
      {
        regions[i]->drawRegion(WorkingImg, regions[i]->Label); // und Label ist RegNodeCount+1!!!!! //->GrwMw);
        regions[i]->drawWSHDPixel(WorkingImg, 0);
      }

    // Nachbarschaften der Regionen berechnen
    for (int i = 0;   i < RegNodeCount; i++)
      {
        regions[i]->CompNeighborRegions8(WorkingImg);
      }

    CopyImg(WorkingImg, retImg);
    return OK;
  }
#undef FNAME

// Bild mit durchschnittlichen Grauwert fuer Regionen zeichnen
#define FNAME "DGrwImage"
  int DGrwImage(Image Original, Image WSImg, Image& GrwImg)
  {

    if (!IsImg(Original) || !IsImg(WSImg) || !IsImg(GrwImg))
      throw IceException(FNAME, M_INVALID_IMAGE);

    Image mark;
    mark.create(Original->xsize, Original->ysize, 2);

    GrwImg.set(0);
    mark.set(0);

    int grw, grwsum;
    int xn, yn, xkoor, ykoor;
    int n;

    FIFOList PQ;
    for (int y = 0; y < WSImg.ysize; y++)
      for (int x = 0; x < WSImg.xsize; x++)
        {

          if (GetVal(WSImg, x, y) == 0)
            {
              PutVal(mark, x, y, 1);
            }
          else if (GetVal(mark, x, y) == 0)
            {

              PQ.clear();

              struct STPoint p = {x, y, 0};
              grwsum = GetVal(Original, x, y);

              PQ.push_back(p);

              if (!PQ.empty())
                {
                  for (n = 0; n < int(PQ.size()); n++)
                    {
                      p = PQ[n];
                      xkoor = p.x;
                      ykoor = p.y;

                      ForAll4Nbrs(Original, xkoor, ykoor, xn, yn,
                      {
                        if (GetVal(mark, xn, yn) == 0 && GetVal(WSImg, xn, yn) != 0)
                          {
                            PutVal(mark, xn, yn, 1);
                            grwsum += GetVal(Original, xn, yn);
                            struct STPoint p2 = p;
                            p2.x = xn;
                            p2.y = yn;
                            p2.grw = 0;
                            PQ.push_back(p2);
                          }
                      };)
                    }

                  grw = grwsum / n;

                  for (n = 0; n < int(PQ.size()); n++)
                    {
                      PutVal(GrwImg, PQ[n].x, PQ[n].y, grw);
                    }
                } // ! empty
            }
        }
    return OK;
  }
#undef FNAME

  /*************************************************/
// vollstaendige Grapheniteration durchfuehren:
// 1. Regionengraph berechnen
// 2. in Kantengraph transformieren
// 3. WST auf Kantengraph ausfuehren
// 4. Kantengraph in Regionenbild ueberfuehren

#define FNAME "GraphIter"
  int GraphIter(Image Original, Image WSImg, Image& GrwImg, int Treshold)
  {
    // Bilder initialisert und Schwellwert fuer Wasserscheidenlinien groesser 0 ?
    if (!IsImg(Original) || !IsImg(WSImg) || !IsImg(GrwImg) || Treshold < 0)
      throw IceException(FNAME, M_WRONG_PARAMETER);

    GrwImg.set(255);
    Image BlWh = NewImg(WSImg);
    RegionGraph RG;
    EdgeGraph* EG;
    RG = RegionGraph(Original, WSImg, BlWh); // Regionengraph mit Bildern initialisieren
    EG = RG.ComputeEdgeGraph();   // Kantengraph berechnen

    if (EG == nullptr)
      throw IceException(FNAME, M_NOT_INITIALIZED);

    EG->ComputeWatershedTrans();  // WST auf Kantengraph ausfuehren

    Image WSheds = NewImg(BlWh);
    EG->drawAllWSHEDs(BlWh, WSheds, Treshold); // Wasserscheidenlinien, die durch die WST uebrig geblieben sind, einzeichnen
    // durchschnittlichen Grauwert
    DGrwImage(Original, WSheds, GrwImg);
    return OK;
  }
#undef FNAME
}

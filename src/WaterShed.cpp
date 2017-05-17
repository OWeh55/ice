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

//  Stephan Wagner, Feb. 2005
//  W. Ortmann 9/05,12/11

#include <malloc.h>

#include "base.h"
#include "IceException.h"
#include "drawline.h"
#include "paint.h"

#include "WaterShed.h"
#include "RegionGraph.h"

// ---------------------------------------------------------------------------
namespace ice
{
  static int WST1(Image& i, Image& ret);
  static int WST2(Image& i, Image& ret);

// Wasserscheidentransformation:
// Startfunktion fuer WST1 - klassischer Flutungsansatz und WST2 - Markeransatz
#define FNAME "WST"
  void WST(const Image& in, Image& WSImg, WstMode mode)
  {
    try
      {
        MatchImg(in, WSImg);

        Image temp;
        bool needs_temp = in == WSImg;

        if (needs_temp)
          {
            /* Eingabebild=Zielbild*/
            temp = NewImg(in, true);  /* temporaeres Eingabebild anlegen*/
          }
        else
          {
            temp = in;
          }

        switch (mode)
          {
          case WstMode::classic:
            WST1(temp, WSImg);
            break;
          case WstMode::marker:
            WST2(temp, WSImg);
            break;
          default:
            throw IceException(FNAME, M_WRONG_MODE);
          }
      }
    RETHROW;
  }
#undef FNAME

  int** dnew(int x, int y)
  {
    // Speicher fuer dyn. Array anfordern
    int** m = new int* [y];

    for (int j = 0; j < y; j++)
      {
        m[j] = new int [x];
      }

    return m;
  }

  void ddelete(int** m, int x, int y)
  {
    // Speicher fuer dyn. Array freigeben

    for (int j = 0; j < y; j++)
      {
        delete [] m[j];
      }

    delete [] m;
  }

// Wasserscheidenalgorithmus mit FIFO-Warteschlangen und Distanzfunktion

#define FNAME "WST1"
  static int WST1(Image& i, Image& ret)
  {
    int** m1; // WS-Markierungen

    // keine ICE-Matrix, weil Array schneller
    Image m3; // Bild zum Zwischenspeichern der Distanzen

    int max = 0;
    int xsize = i->xsize;
    int ysize = i->ysize;
    int x, y;
    int xn, yn;

    // Randeffekte durch schwarze Umrandung des Bildes ausschließen
    setborder(i, i.maxval);

    // pro Hoehenstufe ein Vektor fuer Punkte
    // nach Durchfuehren von Bucket-Sort:
    //    schneller Zugriff auf alle Punkte eines Hoehenlevels
    PointVECTOR* b2 = new PointVECTOR[i.maxval + 1];
    // FIFO-Punkt-Schlange
    FIFOList Q;

    int current_label = 1;
    //    int gr = 0;
    int n, j;

    bool WshedNb;// Wasserscheide setzen aufgrund von anderem Wasserscheidenpixel in der Nachbarschaft

    // Speicher fuer dyn. Array allokieren
    m1 = dnew(xsize, ysize);

    // Distanzbild initialisieren
    m3 = NewImg(i->xsize, i->ysize, 10000);

    // WS-Markierungsbild initialisieren
    setM(m1, x, y, xsize, ysize, INIT);
    // Distanzbild initialisieren
    m3.set(0);

    // Vorbereitung: Pixel nach aufsteigendem Grauwert sortieren

    for (int y = 0; y < i.ysize; y++)
      for (int x = 0; x < i.xsize; x++)
        {
          int level = GetVal(i, x, y);

          if (level > max)
            {
              max = level;
            }

          struct STPoint temp = {x, y, level};
          b2[level].push_back(temp);
        }

// 1. von Level 0 bis Level GRWMax

    x = 0;
    y = 0;

    for (j = 0; j <= max; j++)
      {
        // 1.1. fuer jedes Level Pixel in Schlange aufnehmen,
        //      wenn ein Nachbarschaftspixel schon zu einer Region zugehoerig ist oder Wasscheidenpixel ist
        if (!b2[j].empty())
          for (n = 0; n < int(b2[j].size()); n++)
            {

              struct STPoint temp;
              // Punkt aus Schlange der Hoehe j lesen
              temp = b2[j][n];
              x = temp.x;
              y = temp.y;

              // Pixel als zur aktuellen Hoehenstufe zugehoerig markieren
              PutValM(m1, x, y, MASK);

              // liegt Punkt am Objektrand oder an einer Wasserscheide?
              bool isObjRandPixel = false;

              ForAll8Nbrs(i, x, y, xn, yn,
              {
                // Nachbarschaft auf Regionenzugehoerigkeit bzw. Wasserscheiden testen
                if (GetValM(m1, xn, yn) > 0 || GetValM(m1, xn, yn) == WSHD)
                  {
                    isObjRandPixel = true;
                    goto check;
                  }
              });
check:

              if (isObjRandPixel)
                {
                  // wenn Punkt an Objektrand liegt, Distanz auf 1 setzen
                  PutVal(m3, x, y, 1);
                  // in FIFO-Schlange aufnehmen
                  Q.push_back(temp);
                }
            }

        // 1.2. Punkte, die an Regionengrenzen liegen, verarbeiten
        //    aktuelle Distanz von Region ist 1
        int curdist = 1;
        struct STPoint temp = { -1, -1, 0}; // Trennelement fuer verschiedene Distanzstufen
        Q.push_back(temp);        // Trennelement einfuegen

        if (!Q.empty())         // FIFO-Schlange leer?
          do
            {

              temp = Q.front();
              Q.pop_front();      // Punkt aus Schlange lesen
              x = temp.x;
              y = temp.y;
              //gr = temp.grw;

              if ((temp.x == -1))     // Element ist Trennelement?
                {
                  if (Q.empty())
                    {
                      goto ende;
                    }
                  else
                    {
                      curdist++;    // naechste Distanzstufe
                      Q.push_back(temp);  // wenn x=-1 und y=-1 : temp als Markierungselement wieder hinten anfgen
                      temp = Q.front(); // naechsten Punkt lesen
                      x = temp.x;
                      y = temp.y;
                      //gr = temp.grw;
                      Q.pop_front();
                    }
                }

              WshedNb = false;      // Wasserscheidenflag fuer Punkt x,y
#if 0
              struct STPoint temp;
              temp.x = 0;
              temp.y = 0;
              temp.grw = 0;
#endif
              int xn, yn;
              // Nachbarschaft des aktuellen Punktes untersuchen
              ForAll8Nbrs(i, x, y, xn, yn,
              {
                // m1: WS-Markierungswert des Punktes, m3: Distanzwert
                //     INIT=0 , wenn in hoeherem Grauwertlevel als dem aktuellem
                //     MARK=-1, wenn Punkt in aktuellem Grauwertlevel
                //     WSHD=-3, wenn Punkt Wasserscheide
                //     m1  > 0, wenn Punkt zu Region zugehoerig

                int label = GetValM(m1, x, y);
                //  int dist=GetVal(m3,x,y);
                // m1: WS-Markierungswert des Nachbarpunktes, m3: Distanzwert
                int labelNb = GetValM(m1, xn, yn);
                int distNb = GetVal(m3, xn, yn);

                // Nachbarpunkt ist anderem Minimum nicht naeher und
                // Nachbar ist einem Becken zugehoerig oder Wasserscheide
                if ((distNb <= curdist) && ((labelNb > 0) || (labelNb == WSHD)))
                  {
                    if (labelNb > 0)
                      {
                        if ((label == MASK) || (label == WSHD && WshedNb))
                          {
                            // Nachbarpixel ist Beckenpixel und Punkt x,y noch nicht verarbeitet
                            // oder Wasserscheide durch Einfluss einer Wasserscheide in der Umgebung
                            // in diesen Faellen Punkt x,y dem Becken des Nachbarpunktes zuordnen
                            PutValM(m1, x, y, labelNb);
                          }
                        // wenn 2 Becken aneinanderstossen, wird Punkt x,y Wasserscheide
                        else if (labelNb != label && label != WSHD)
                          {
                            PutValM(m1, x, y, WSHD);
                            // Wasserscheide durch Aufeinandertreffen zweier Becken entstanden
                            WshedNb = false;
                          }
                      }
                    else if ((label == MASK))
                      {
                        // Nachbar ist Wasserscheide und Punkt x,y noch nicht verarbeitet
                        PutValM(m1, x, y, WSHD);
                        // Wasserscheide in Punkt x,y wird durch Einfluss einer Wasserscheide gesetzt
                        WshedNb = true;
                      }
                  }
                else {      // Nachbarpunkt ist in aktuellem Grauwertlevel und noch nicht verarbeitet
                    if ((labelNb == MASK) && (distNb == 0))
                      {
                        // Distanz erhoehen
                        PutVal(m3, xn, yn, curdist + 1);
                        struct STPoint temp2;
                        temp2.x = xn;
                        temp2.y = yn;
                        temp2.grw = 0;
                        // in FIFO-Schlange aufnehmen und in einem der naechsten Durchlaeufe verarbeiten
                        Q.push_back(temp2);
                      }
                  }
              });

            }
          while (!Q.empty());

        // 1.3. Punkte, die isoliert liegen, d.h. noch nicht in 1.1. oder 1.2. verarbeitet wurden,
        //      als neue Minima in Markierungsbild m1 mit neuem Label eintragen und Einflusszone
        //      der Minima berechnen (alle angrenzenden Punkte der gleichen Hoehenstufe)

ende:

        if (!b2[j].empty())           // alle Punkte der Hoehe j untersuchen
          for (n = 0; n < int(b2[j].size()); n++)
            {

              temp = b2[j][n];
              x = temp.x;
              y = temp.y;
              //gr = temp.grw;

              PutVal(m3, x, y, 0);      // Distanz 0 setzen (Punkt entweder WS, Beckenpixel (also schon verarbeitet) oder jetzt neues Minimum)

              if (GetValM(m1, x, y) == MASK)   // Pixel wurde noch nicht bearbeitet, MARK Wert in m2
                {
                  current_label++;    // neues Label

                  Q.push_back(temp);    // Punkt x,y in FIFO-Schlange aufnehmen
                  // Beckennummer eintragen
                  PutValM(m1, x, y, current_label);

                  // Einflusszone berechnen
                  if (!Q.empty())     // noch Punkte in der FIFO-Schlange?
                    do          // solange Punkte zu neuem Minima hinzuzufuegen sind...
                      {
                        temp = Q.front();
                        x = temp.x;
                        y = temp.y;
                        //gr = temp.grw;

                        Q.pop_front();  // naechsten Punkt aus FIFO-Schlange lesen

                        int xn, yn;
                        // Nachbarn von x,y untersuchen
                        ForAll8Nbrs(i, x, y, xn, yn,
                        {
                          // Nachbar ist in der gleichen Hoehenstufe und noch nicht verarbeitet?
                          if (GetValM(m1, xn, yn) == MASK)
                            {
                              struct STPoint temp2 = b2[j][n];
                              temp2.x = xn;
                              temp2.y = yn;
                              temp2.grw = current_label;
                              // Punkt in FIFO-Schlange einfuegen
                              Q.push_back(temp2);
                              // Nachbarpunkt zu neuem Becken hinzufuegen
                              PutValM(m1, xn, yn, current_label);
                              // Distanz 0 setzen, da zu Becken hinzugefuegt
                              PutVal(m3, xn, yn, 0);
                            }
                        });
                      }
                    while (!Q.empty());
                }
            }

      }

    for (int y = 0; y < ysize; y++)   // Wasserscheiden von WSHED=-3 auf 0 setzen
      for (int x = 0; x < xsize; x++)
        if (m1[y][x] == WSHD)
          {
            m1[y][x] = 0;
          }

    cout << current_label << endl;

    // Test, ob Regionenzahl in Zielbild passt
    if (ret.maxval < current_label)
      {
        ddelete(m1, xsize, ysize);
        throw IceException(FNAME, M_LOWRANGE);
      }

    // Markierungsarray m1 in Bild uebertragen
    for (int y = 0; y < ret.ysize; y++)
      for (int x = 0; x < ret.xsize; x++)
        PutVal(ret, x, y, GetValM(m1, x, y));

    setborder(ret, 0);    // Rahmen um Ausgangsbild setzen. Bildrand als Regionengrenzen markieren

    ddelete(m1, xsize, ysize);
    Q.clear();
    return OK;
  }
#undef FNAME

// verschiedene Labels an Markerregionen vergeben
  int fillStartRegion(Image& m1, Image& m2, int x, int y, int currLab)
  {

    FIFOList PQ;
    struct STPoint temp1 = {x, y, 1};
    PQ.push_back(temp1);
    PutVal(m1, x, y, currLab);

    int xn, yn;
    int n;

    if (!PQ.empty())
      for (n = 0; n < int(PQ.size()); n++)
        {
          temp1 = PQ[n];
          x = temp1.x;
          y = temp1.y;

          ForAll8Nbrs(m1, x, y, xn, yn,
          {
            if (GetVal(m2, xn, yn) == DONE && GetVal(m1, xn, yn) == 0)
              {
                PutVal(m1, xn, yn, currLab);
                struct STPoint temp = temp1;
                temp.x = xn;
                temp.y = yn;
                temp.grw = 0;
                PQ.push_back(temp);
              }
          };)
        }

    PQ.clear();
    return OK;
  }

// Wasserscheidenalgorithmus basierend auf Markeransatz
#define FNAME "WST2"
  static int WST2(Image& i, Image& ret)
  {
    int** m1; // dyn. Array fuer WS-Markierungen, kein Image, weil Anzahl der Labels noch nicht bekannt und keine negativen Werte moeglich
    // keine ICE-Matrix, weil Array schneller

    int xmax, ymax;
    int x, y;
    int xn, yn;

    FIFOList Q;         // FIFO-Punkt-Schlange fuer WST
    FIFOList PQ;  // FIFO-Schlange fuer Labelvergabe der Initialmarker

    setborder(i, i.maxval);  // Seiteneffekte durch schwarze Umrandung des Bildes ausschließen (kein Übersprechen von Regionen)

    // pro Hoehenstufe ein Vektor fuer Punkte
    // nach Durchfuehren von Bucket-Sort: schneller Zugriff auf alle Punkte eines Hoehenlevelctor

    vector <PointVECTOR> b2;
    b2.resize(i.maxval + 1);

    int xpos = 0, ypos = 0;
    //    int gr = 0;
    int max = 0;
    int n, j, k;

    int curlab = 1;

    bool WshedNb; // true : Punkt ist WS aufgrund einer Wasserscheide in der Nachbarschaft
    // false: Punkt ist WS aufgrund zweier benachbarter Becken (Nachbarn mit unterschiedlichen Labeln)
    struct STPoint stp1;

    xmax = i->xsize;
    ymax = i->ysize;

    // Speicher fuer dyn. Array allokieren
    m1 = (int**) malloc(sizeof(int*) * ymax);

    for (j = 0; j < ymax; j++)
      {
        m1[j] = (int*) malloc(sizeof(int) * xmax);

        if (m1[j] == NULL)
          throw IceException(FNAME, M_NO_MEM);
      }

    // WS-Markierungsbild initialisieren
    setM(m1, x, y, xmax, ymax, INIT);

    // Vorbereitung: Pixel nach aufsteigendem Grauwert sortieren
    for (int y = 0; y < i.ysize; y++)
      for (int x = 0; x < i.xsize; x++)
        {
          int level = GetVal(i, x, y);

          if (level > max)
            {
              max = level;
            }

          struct STPoint stptemp = {x, y, level};
          b2[level].push_back(stptemp);
        }

    if (!b2[0].empty())
      {
        for (n = 0; n < int(b2[0].size()); n++)
          {
            // alle Keimpunkte mit MASK initialisieren
            stp1 = b2[0][n];
            xpos = stp1.x;
            ypos = stp1.y;
            //gr = stp1.grw;
            PutValM(m1, xpos, ypos, MASK);
          }

        for (n = 0; n < int(b2[0].size()); n++)
          {
            // Labelvergabe an die Markerregionen
            stp1 = b2[0][n];
            xpos = stp1.x;
            ypos = stp1.y;
            //gr = stp1.grw;
            PQ.clear();

            // Fuellalgorithmus fuer die Becken durchfuehren
            if (GetValM(m1, xpos, ypos) == MASK)
              {
                PQ.push_back(stp1);
                // neue Region = neues Label: Beckennummer erhoehen
                curlab++;
                // Beckenlabel in Markierungsarray schreiben
                PutValM(m1, xpos, ypos, curlab);

                if (!PQ.empty())
                  for (k = 0; k < int(PQ.size()); k++)
                    {
                      stp1 = PQ[k];
                      x = stp1.x;
                      y = stp1.y;
                      // Nachbarn schon verarbeitet? zum Marker zugehoerig? dann markieren
                      ForAll8Nbrs(i, x, y, xn, yn,
                      {
                        if (GetValM(m1, xn, yn) == MASK)
                          {
                            PutValM(m1, xn, yn, curlab);
                            struct STPoint stp2 = stp1;
                            stp2.x = xn;
                            stp2.y = yn;
                            stp2.grw = 0;
                            PQ.push_back(stp2);
                          }
                      };)
                    }
              }
          }
      }

// 1. von Level 0 bis Level GRWMax
    for (j = 1; j <= max; j++)
      {
        // 1.1. fuer jedes Level Pixel in Schlange aufnehmen,
        //      wenn ein Nachbarschaftspixel schon zu einer Region zugehoerig ist oder Wasscheidenpixel ist
        if (!b2[j].empty())
          {
            for (n = 0; n < int(b2[j].size()); n++)
              {
                // Punkt aus Schlange der Hoehe j lesen
                stp1 = b2[j][n];
                xpos = stp1.x;
                ypos = stp1.y;
                //gr = stp1.grw;
                // Pixel als zur aktuellen Hoehenstufe zugehoerig markieren
                PutValM(m1, xpos, ypos, MASK);
                // liegt Punkt am Objektrand oder an einer Wasserscheide?
                bool isObjRandPixel = false;
                // Nachbarschaft auf Regionenzugehoerigkeit bzw. Wasserscheiden testen
                int xn, yn;
                ForAll8Nbrs(i, xpos, ypos, xn, yn,
                {
                  if (GetValM(m1, xn, yn) > 0)
                    {
                      isObjRandPixel = true;
                      goto check;
                    }
                });
check:        // wenn Punkt an Objektrand liegt, in FIFO-Schlange aufnehmen

                if (isObjRandPixel)
                  {
                    Q.push_back(stp1);
                  }
              }
          }

        // 1.2. Punkte, die an Regionengrenzen liegen, verarbeiten
        //    aktuelle Distanz von Region ist 1
        struct STPoint temp = { -1, -1, 0};

        if (!Q.empty()) // FIFO-Schlange leer? (keine Punkte in dieser Hoehenstufe zu verarbeiten)
          do
            {
              temp = Q.front();
              Q.pop_front();      // Punkt aus Schlange lesen
              x = temp.x;
              y = temp.y;
              //gr = temp.grw;

              WshedNb = false;      // Wasserscheidenflag fuer Punkt x,y
              // Wasserscheide fuer x,y aus der Nachbarschaft des Punktes begruendet
#if 0
              struct STPoint temp;
              temp.x = 0;
              temp.y = 0;
              temp.grw = 0;
              int xn, yn;
#endif
              ForAll8Nbrs(i, x, y, xn, yn,
              {
                // m1: WS-Markierungswert des Punktes
                //     INIT=0 , wenn in hoeherem Grauwertlevel als dem aktuellem
                //     MARK=-1, wenn Punkt in aktuellem Grauwertlevel
                //     WSHD=-3, wenn Punkt Wasserscheide
                //     m1  > 0, wenn Punkt zu Region zugehoerig
                int label = GetValM(m1, x, y);

                int labelNb = GetValM(m1, xn, yn);

                if ((labelNb > 0) || (labelNb == WSHD))
                  {
                    if (labelNb > 0)
                      {
                        if ((label == MASK) || (label == WSHD && WshedNb))
                          {
                            // Nachbarpixel ist Beckenpixel und Punkt x,y noch nicht verarbeitet
                            // oder Wasserscheide durch Einfluss einer Wasserscheide in der Umgebung
                            // in diesen Faellen Punkt x,y dem Becken des Nachbarpunktes zuordnen
                            PutValM(m1, x, y, labelNb);
                          }
                        else if (labelNb != label)
                          {
                            // wenn 2 Becken aneinanderstossen, wird Punkt x,y Wasserscheide
                            PutValM(m1, x, y, WSHD);
                            // Wasserscheide durch Aufeinandertreffen zweier Becken entstanden
                            WshedNb = false;
                          }
                      }
                    else if ((label == MASK))
                      {
                        // Nachbar ist Wasserscheide und Punkt x,y noch nicht verarbeitet
                        // Wasserscheide in Punkt x,y wird durch Einfluss einer Wasserscheide gesetzt
                        PutValM(m1, x, y, WSHD);
                        WshedNb = true;
                      }
                  }
                else {
                    if (labelNb == MASK)
                      {
                        // Nachbarpunkt ist in aktuellem Grauwertlevel und noch nicht verarbeitet
                        struct STPoint temp2;
                        temp2.x = xn;
                        temp2.y = yn;
                        temp2.grw = 0;
                        // in FIFO-Schlange aufnehmen und in einem der naechsten Durchlaeufe von 1.2. verarbeiten
                        Q.push_back(temp2);
                      }
                  }
              });

            }
          while (!Q.empty());

        // Rekursion: Punkte, die noch nicht verarbeitet wurden, "mitschleifen"
        for (n = 0; n < int(b2[j].size()); n++)
          {
            stp1 = b2[j][n];

            // Pixel schon verarbeitet?
            if (GetValM(m1, stp1.x, stp1.y) == MASK)
              {
                // nein: in naechst hoehere Schlange einfuegen, alle Punkte muessen verarbeitet werden
                b2[j + 1].push_back(stp1);
              }
          }
      }

    for (int y = 0; y < ymax; y++)
      for (int x = 0; x < xmax; x++)
        {
          if (m1[y][x] == WSHD)
            {
              m1[y][x] = 0;
            }

          if (m1[y][x] < 0)
            {
              m1[y][x] = curlab;
            }
        }

    // Rueckgabebild
    if (ret.maxval < curlab)
      throw IceException(FNAME, M_LOWRANGE);

    for (int y = 0; y < ret.ysize; y++)
      for (int x = 0; x < ret.xsize; x++)
        PutVal(ret, x, y, GetValM(m1, x, y));

    setborder(ret, 0);    // Rahmen um Ausgangsbild setzen. Bildrand als Regionengrenzen markieren

    return OK;
  }
#undef FNAME

  bool PixelIsMaximum(const Image& i, int x, int y)
  {
    // wenn 1 Nachbar groesser ist, kann aktuell betrachtetes Pixel kein Maximum mehr sein.
    // Es wird dann abgebrochen und false zurueckgeliefert.

    int val = GetVal(i, x, y);
    int xn, yn;

    ForAll8Nbrs(i, x, y, xn, yn,
    {
      if (GetVal(i, xn, yn) > val) return false;
    };)
    return true;
  }

  bool PixelIsMinimum(const Image& i, int x, int y)
  {

    // wenn Nachbarn kleiner sind, kann aktuell betrachtetes Pixel kein Minimum mehr sein.
    // Es wird dann abgebrochen und false zurueckgeliefert.

    int xn, yn;
    int val = GetVal(i, x, y);

    ForAll8Nbrs(i, x, y, xn, yn,
    {
      if (GetVal(i, xn, yn) < val) return false;
    };)

    return true;
  }

  int PixelIsMinimum2(const Image& i, int x, int y)
  {
    // wenn Nachbarn kleiner sind, kann aktuell betrachtetes Pixel
    // kein Minimum mehr sein. Es wird dann abgebrochen und false
    // zurueckgeliefert. Wird nur von getImgMinimas2 aufgerufen,
    // daher keine Fehlerbehandlung

    int xmax = i->xsize;
    int ymax = i->ysize;
    int gp = GetVal(i, x, y); // Wert des Pixels
    int gh;
    int second = i.maxval; // Kleinster Wert in der Umgebung
    //                     = Zweitkleinster Wert, falls (x,y) Minimum ist

    // erste Spalte (links)
    if (x > 0)
      {
        gh = GetVal(i, x - 1, y);

        if (gh < gp)
          {
            return -1;
          }

        second = GetVal(i, x - 1, y);

        if (y > 0)
          {
            gh = GetVal(i, x - 1, y - 1);

            if (gh < gp)
              {
                return -1;
              }

            if (second > gh)
              {
                second = gh;
              }
          }

        if (y < ymax)
          {
            gh = GetVal(i, x - 1, y + 1);

            if (gh < gp)
              {
                return -1;
              }

            if (second > gh)
              {
                second = gh;
              }
          }
      }

    // dritte Spalte (rechts)
    if (x < xmax)
      {
        gh = GetVal(i, x + 1, y);

        if (gh < gp)
          {
            return -1;
          }

        if (second > gh)
          {
            second = gh;
          }

        if (y > 0)
          {
            gh = GetVal(i, x + 1, y - 1);

            if (gh < gp)
              {
                return -1;
              }

            if (second > gh)
              {
                second = gh;
              }
          }

        if (y < ymax)
          {
            gh = GetVal(i, x + 1, y + 1);

            if (gh < gp)
              {
                return -1;
              }

            if (second > gh)
              {
                second = gh;
              }
          }
      }

    // mittlere Spalte (oben und unten)
    if (y > 0)
      {
        gh = GetVal(i, x, y - 1);

        if (gh < gp)
          {
            return -1;
          }

        if (second > gh)
          {
            second = gh;
          }
      }

    if (y < ymax)
      {
        gh = GetVal(i, x, y + 1);

        if (gh < gp)
          {
            return -1;
          }

        if (second > gh)
          {
            second = gh;
          }
      }

    return second;
  }

// Bildminima in Bild io markieren
  int getImgMinimas2(const Image& i, Image& io)
  {

    Image temp = NewImg(i);

    // Minima zu Plateaupixel zurecht stutzen
    for (int y = 0; y < i.ysize; y++)
      for (int x = 0; x < i.xsize; x++)
        {
          int val = PixelIsMinimum2(i, x, y);

          if (val != -1)
            {
              PutVal(temp, x, y, val);  // Punkt x,y ist Minimum: mit zweitkleinstem Wert ersetzen
            }
          else
            {
              PutVal(temp, x, y, GetVal(i, x, y));  // Punkt kein Minimum: Wert uebernehmen
            }
        }
    CopyImg(temp, io);
    return OK;
  }

// Minima von Bild i in Bild io markieren
#define FNAME "getImgMinimas"
  int getImgMinimas(const Image& i, Image& io)
  {

    if (!IsImg(i) || !IsImg(io))
      throw IceException(FNAME, M_WRONG_PARAM);

    for (int y = 0; y < i.ysize; y++)
      for (int x = 0; x < i.xsize; x++)
        {
          if (PixelIsMinimum(i, x, y))
            {
              PutVal(io, x, y, 0);
            }
        }
    return OK;
  }
#undef FNAME

// Maxima von Bild i in Bild io markieren
#define FNAME "getImgMaximums"
  int getImgMaximums(const Image& i, Image& io)
  {

    if (!IsImg(i) || !IsImg(io))
      throw IceException(FNAME, M_WRONG_PARAM);

    for (int y = 0; y < i.ysize; y++)
      for (int x = 0; x < i.xsize; x++)
        {
          if (PixelIsMaximum(i, x, y))
            {
              PutVal(io, x, y, 0);
            }
        }

    return OK;
  }
#undef FNAME

// Anzahl der Nachbarregionen bestimmen
  int NbRegions(const Image& i, int x, int y)
  {

    set<int> nb_set;
    set<int>::iterator it;

    int grwsum = 0;
    int size_set = 0 ;
    int count = 0;
    int xn, yn;

    INTVECTOR int1;

    ForAll8Nbrs(i, x, y, xn, yn,
    {
      if (GetVal(i, xn, yn) != 0)
        {
          nb_set.insert(GetVal(i, xn, yn));
        }
    });

    for (it = nb_set.begin(); it != nb_set.end(); it++)
      {
        grwsum += *it;
      }

    size_set = nb_set.size();

    if (size_set == 0)
      {
        count = 1;
      }
    else
      {
        count = size_set;
      }

    return grwsum / count;
  }

// Marker in Bild i1 loeschen, die aus weniger als treshold Pixeln bestehen
#define FNAME "DeleteSmallMarkers"
  int deleteSmallMarkers(Image& i1, int treshold)
  {

    Image status;
    status.create(i1);
    status.set(0);

    if (!IsImg(i1) || treshold < 0)     // Eingabebild gueltig
      throw IceException(FNAME, M_WRONG_PARAM);

    struct STPoint temp;

    // fuer das gesamte Bild Markerpixelmengen bestimmen und wenn Pixelanzahl kleiner treshold, loeschen
    for (int yl = 0; yl < i1.ysize; yl++)
      for (int xl = 0; xl < i1.xsize; xl++)
        {

          // Punkt ist Markerpixel und noch nicht bearbeitet?
          if (GetVal(i1, xl, yl) == 0 && GetVal(status, xl, yl) == 0)
            {

              FIFOList PQ;

              temp.x = xl;
              temp.y = yl;
              temp.grw = GetVal(i1, xl, yl);
              PQ.push_back(temp);

              if (!PQ.empty())
                for (int n = 0; n < int(PQ.size()); n++)
                  {
                    int x, y, xn, yn;
                    x = PQ[n].x;
                    y = PQ[n].y;

                    // alle 8 Nachbarpixel untersuchen, ob sie aktuellen Marker gehoeren, wenn ja in Punktschlange einfuegen
                    ForAll8Nbrs(i1, x, y, xn, yn,
                    {
                      if (GetVal(i1, xn, yn) == 0 && GetVal(status, xn, yn) == 0)
                        {
                          PutVal(status, xn, yn, 1);
                          struct STPoint t2 = temp;
                          t2.x = xl;
                          t2.y = yl;
                          t2.grw = GetVal(i1, xn, yn);
                          PQ.push_back(t2);
                        }
                    });
                  }

              if (int(PQ.size()) < treshold)   // Marker loeschen, wenn er weniger als treshold Pixel enthaelt.
                {
                  for (int n = 0; n < int(PQ.size()); n++)
                    {
                      PutVal(i1, PQ[n].x, PQ[n].y, i1.maxval);
                    }
                }
            }
        }

    return OK;
  }
#undef FNAME

// Wasserscheidenlinien loeschen und durch gemittelten Grauwert der angrenzenden Regionen ersetzen
#define FNAME "delWSHDPixels"
  int delWSHDPixels(const Image& GrwImg, Image& WSHEDDeleted)
  {

    if (!IsImg(GrwImg) || !IsImg(WSHEDDeleted))
      throw IceException(FNAME, M_WRONG_PARAM);

    int xn, yn;

    for (int y = 0; y < GrwImg.ysize; y++)
      for (int x = 0; x < GrwImg.xsize; x++)
        {

          int GRWSum = 0;
          int RegPixelAnz = 0;

          if (GetVal(GrwImg, x, y) == 0)          // Punkt ist Wasserscheidenlinie
            {
              ForAll8Nbrs(GrwImg, x, y, xn, yn,     // Nachbarschaft des Punktes betrachten

                          if (GetVal(GrwImg, xn, yn) != 0)    // Nachbar ist kein WS-Pixel
              {
                GRWSum += GetVal(GrwImg, xn, yn); // Grauwertsumme aktualisieren
                  RegPixelAnz++;          // Anzahl der betrachteten Punkte erhoehen
                }
                         );

              // durschnittlichen Grauwert der Nachbarpunkte in Rueckgabebild eintragen
              if (RegPixelAnz != 0)
                {
                  PutVal(WSHEDDeleted, x, y, (GRWSum / RegPixelAnz));
                }
              else
                {
                  PutVal(WSHEDDeleted, x, y, GetVal(GrwImg, x, y));
                }
            }                     // wenn keine WS, Grauwert uebernehmen
          else
            {
              PutVal(WSHEDDeleted, x, y, GetVal(GrwImg, x, y));
            }
        }
    return OK;
  }
#undef FNAME

  /*************************************************/

// Verteilung von Regionengroesse und Grauwert in einem Histogramm einzeichnen
#define FNAME "dGRWHistogramm"
  int dGRWHistogramm(Image Original, Image WSImg, Image& GrwImg)  //,Histogram h) {
  {

    Image Histo;
    Histo.create(300, 300, 255);
    Histo.set(255);

    if (!IsImg(Original) || !IsImg(WSImg) || !IsImg(GrwImg))
      {
        // Eingabebilder ungleich NULL?
        throw IceException(FNAME, M_WRONG_PARAM);
      }

    int maxval = WSImg.maxval;

    vector<long> regGrw(maxval + 1);
    vector<int> regPix(maxval + 1);

    for (int i = 1; i < maxval + 1; i++)        // Initialisieren
      {
        regGrw[i] = 0;
        regPix[i] = 1;
      }

    for (int y = 0; y < WSImg.ysize; y++)
      for (int x = 0; x < WSImg.xsize; x++)
        {
          regGrw[GetVal(WSImg, x, y)] += GetVal(Original, x, y); // Grauwertsumme der Grauwerte aktualisieren
          regPix[GetVal(WSImg, x, y)]++;    // Anzahl der Pixel in der Region erhoehen
        }

    regGrw[0] = 0;              // Grauwert fuer Wasserscheiden - Label 0!!

    int scale2Grw = Original->xsize / 255;

    Line(5, 290, 275, 290, 0, Histo);     // Histogramm zeichnen
    Line(10, 295, 10, 15, 0, Histo);
    Text("Grw", 275, 290, 0, 0, Histo);
    Text("Size", 5, 5, 0, 0, Histo);

    for (int i = 1; i < maxval + 1; i++)        // fuer alle Regionen maxval=hoechstes Label
      {
        regGrw[i] = (regGrw[i] / regPix[i]);    // durchschnittlichen Grauwert der Region berechnen
        int wurzel = (int)sqrt(float(regPix[i]));

        int regSize = (290 - wurzel / scale2Grw);     // skalieren

        if (regSize < 0)
          {
            regSize = 0;
          }

        PutVal(Histo, 10 + regGrw[i], regSize, 0);    // Region i hinsichtlich Groesse und durchschnittlichen Grauwert einzeichnen
      }

    for (int y = 0; y < WSImg.ysize; y++)
      for (int x = 0; x < WSImg.xsize; x++)
        PutVal(GrwImg, x, y, regGrw[GetVal(WSImg, x, y)]);  // neue Grauwerte in Ausgabebild eintragen

    return OK;
  }
#undef FNAME
}


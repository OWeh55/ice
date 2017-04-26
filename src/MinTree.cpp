/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2011 FSU Jena, Digital Image Processing Group
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

#include "distance.h"
#include "assignment.h"

#include "MinTree.h"

using namespace std;

namespace ice
{

  bool MinTree::findMin(const vector<bool>& b, int& minidx)
  {
    bool ret = false;
    double minval = 0.0;
    minidx = -1;

    // startwert für Minimum
    for (int k = 0; k < nPoints && minidx < 0; k++)
      if (! b[k])
        {
          minidx = k;
          minval = edgeLength[k];
          ret = true;
        }

    if (ret)   // wenn erster Punkt gefunden
      {
        // weitere Punkte testen
        for (int k = minidx; k < nPoints; k++)
          {
            if ((! b[k]) && edgeLength[k] < minval)
              {
                minval = edgeLength[k];
                minidx = k;
              }
          }
      }

    return ret;
  }

#define FNAME "MinTree::MinTree"
#if 0
  MinTree::MinTree(const vector<Point>& pointList):
    nPoints(pointList.size()), tree(nPoints), edgeLength(nPoints)
  {
    if (nPoints < 1)
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    // zum Start ist nPoints-1 der erste und einzige Punkt des Baumes == Wurzel

    vector<bool> isInTree(nPoints, false);
    isInTree[nPoints - 1] = true;

    // Kanten
    // für Punkte ausserhalb des Baumes (isInTree == false): kürzeste Verbindung dieses Punktes zum Baum
    // für Punkte im Baum (isInTree == true): eine Kante des Baumes
    // initialisierung: Verbindung mit einzigem Punkt des Baumes nPoints - 1
    vector<unsigned int> qu(nPoints, nPoints - 1);

    // (Quadrat der) Distanz des Punktes zum Baum (isInTree==false)
    // Länge der Kante i <-> qu[i] (isInTree==true)

    Point root = pointList[nPoints - 1];

    for (int i = 0; i < nPoints - 1; i++)
      {
        edgeLength[i] = Distance2(pointList[i], root);
      }

    edgeLength[nPoints - 1] = 0.0;

    int mini;

    // solange unverarbeitete Punkte: wähle nächsten Punkt
    while (findMin(isInTree, mini))
      {
        // neue Kante: mini <-> qu[mini]
        tree.setFather(mini, qu[mini]);
        isInTree[mini] = true;

        // Update der (Quadrate der) Distanzen Punkt <-> Baum
        // neuer Punkt mini des Baumes koennte Verbindungen anderer Knoten zum Baum verkuerzen

        const Point& pmin = pointList[mini];
        for (int i = 0; i < nPoints; i++)
          {
            if (! isInTree[i])
              {
                // i <-> mini ist Kandidat für eine kürzere Verbindung zum Baum
                // double newDistance = Distance2(pmin, pointList[i]);
                double newDistance = (pmin - pointList[i]).r2();
                if (newDistance < edgeLength[i])   // kuerzer ?!
                  {
                    edgeLength[i] = newDistance;
                    qu[i] = mini;
                  }
              }
          }
      }
  }
#endif

  MinTree::MinTree(const std::vector<std::vector<double> >& pointList,
                   const VectorDistance& vd):
    nPoints(pointList.size()), tree(nPoints), edgeLength(nPoints)
  {
    if (nPoints < 1)
      throw IceException(FNAME, M_EMPTY_POINTLIST);

    // zum Start ist nPoints-1 der erste und einzige Punkt des Baumes == Wurzel

    vector<bool> isInTree(nPoints, false);
    isInTree[nPoints - 1] = true;

    // Kanten
    // für Punkte ausserhalb des Baumes (isInTree==false): kürzeste Verbindung dieses Punktes zum Baum
    // für Punkte im Baum (isInTree==true): eine Kante des Baumes
    // initialisierung: Verbindung mit einzigem Punkt des Baumes nPoints-1
    vector<unsigned int> qu(nPoints, nPoints - 1);

    // (Quadrat der) Distanz des Punktes zum Baum (isInTree==false)
    // Länge der Kante i <-> qu[i] (isInTree==true)

    vector<double> root = pointList[nPoints - 1];

    for (int i = 0; i < nPoints - 1; i++)
      {
        edgeLength[i] = vd(pointList[i], root);
      }

    edgeLength[nPoints - 1] = 0.0;

    int mini;

    // solange unverarbeitete Punkte: wähle nächsten Punkt
    while (findMin(isInTree, mini))
      {
        // neue Kante: mini <-> qu[mini]
        tree.setFather(mini, qu[mini]);
        isInTree[mini] = true;

        // Update der (Quadrate der) Distanzen Punkt <-> Baum
        // neuer Punkt mini des Baumes könnte Verbindungen anderer Knoten zum Baum
        const vector<double>& pmin = pointList[mini];

        // verkürzen
        for (int i = 0; i < nPoints; i++)
          {
            if (! isInTree[i])
              {
                // i <-> mini ist Kandidat für eine kürzere Verbindung zum Baum
                // double newDistance = Distance2(pmin, pointList[i]);
                double newDistance = vd(pmin, pointList[i]);
                if (newDistance < edgeLength[i])   // kuerzer ?!
                  {
                    edgeLength[i] = newDistance;
                    qu[i] = mini;
                  }
              }
          }
      }
  }

  double MinTree::getTree(Forest& result) const
  {
    double treelen = 0.0;

    result.Reset(nPoints);

    for (int i = 0; i < tree.Size(); i++)
      {
        int father = tree.Father(i);
        if (father != Forest::rootval)   // Knoten ist nicht die Wurzel
          {
            result.setFather(i, father);
            treelen += sqrt(edgeLength[i]);
          }
      }

    return treelen;
  }

  double MinTree::getForest(Forest& result, double maxlen) const
  {
    double treelen = 0.0;

    // Baum um Kanten bereinigen, die länger als maxlen sind
    // dabei die Länge der Kanten summieren

    double maxlen2 = maxlen * maxlen;

    result.Reset(nPoints);

    for (int i = 0; i < tree.Size(); i++)
      {
        int father = tree.Father(i);

        if (father != Forest::rootval)   // Knoten ist nicht die Wurzel
          {
            if (maxlen == 0.0 || edgeLength[i] < maxlen2)
              {
                result.setFather(i, father);
                treelen += sqrt(edgeLength[i]);
              }
          }
      }

    return treelen;
  }

  double MinTree::getCluster(Forest& result, int nCluster) const
  {
    double maxlen = 0.0;
    if (nCluster > 1)
      {
        int nEdges = nPoints - 1;

        vector<double> edgeLengthSorted(nEdges);

        int k = 0;
        for (int i = 0; i < nPoints; i++)
          {
            if (! tree.isRoot(i))   // Knoten ist nicht die Wurzel
              {
                edgeLengthSorted[k] = edgeLength[i];
                k++;
              }
          }

        sort(edgeLengthSorted.begin(), edgeLengthSorted.end());

        if (nCluster >= nEdges)
          {
            nCluster = nEdges - 1;
          }

        maxlen = sqrt(edgeLengthSorted[nEdges + 1 - nCluster]);
      }
    return getForest(result, maxlen);
  }

  MinTree::MinTree(const Matrix& distances):
    nPoints(distances.rows()), tree(nPoints), edgeLength(nPoints)
  {
    if (nPoints < 1 || distances.cols() != nPoints)
      throw IceException(FNAME, M_WRONG_FORMAT);

    // zum Start ist nPoints-1 der erste und einzige Punkt des Baumes = Wurzel

    vector<bool> isInTree(nPoints, false);
    isInTree[nPoints - 1] = true;

    // Kanten
    // für Punkte ausserhalb des Baumes (isInTree==false): kürzeste Verbindung dieses Punktes zum Baum
    // für Punkte im Baum (isInTree==true): eine Kante des Baumes
    // initialisierung: Verbindung mit einzigem Punkt des Baumes nPoints-1
    vector<unsigned int> qu(nPoints, nPoints - 1);

    // (Quadrat der) Distanz des Punktes zum Baum (isInTree==false)
    // Länge der Kante i <-> qu[i] (isInTree==true)

    int rootIndex = nPoints - 1;

    for (int i = 0; i < nPoints - 1; i++)
      {
        edgeLength[i] = distances[i][rootIndex];
      }
    edgeLength[nPoints - 1] = 0.0;

    int mini;

    // solange unverarbeitete Punkte: wähle nächsten Punkt
    while (findMin(isInTree, mini))
      {
        // neue Kante: mini <-> qu[mini]
        tree.setFather(mini, qu[mini]);
        isInTree[mini] = true;

        // Update der (Quadrate der) Distanzen Punkt <-> Baum
        // neuer Punkt mini des Baumes könnte Verbindungen anderer Knoten zum Baum
        // verkürzen
        for (int i = 0; i < nPoints; i++)
          {
            if (! isInTree[i])
              {
                // i <-> mini ist Kandidat für eine kürzere Verbindung zum Baum
                double newDistance = distances[mini][i];

                if (newDistance < edgeLength[i])   // kuerzer ?!
                  {
                    edgeLength[i] = newDistance;
                    qu[i] = mini;
                  }
              }
          }
      }
  }
#undef FNAME
#define FNAME "computeMinTree"
  double computeMinTree(const std::vector<Point>& pointlist, Forest& f,
                        double maxlen)
  {
    MinTree mt(pointlist);
    return mt.getForest(f, maxlen);
  }

  double computeMinTree(const Matrix& pointList, Forest& f, double maxlen)
  {
    vector<Point> pl;

    for (int i = 0; i < pointList.rows(); i++)
      {
        pl.push_back(Point(pointList[i]));
      }

    return computeMinTree(pl, f, maxlen);
  }
#undef FNAME

#define FNAME "cutShortBranches"
  int cutShortBranches(Forest& f, const vector<Point>& points, double minlen)
  {
    for (int i = 0; i < f.Size(); i++)
      {
        if (!f.isRoot(i) && f.getRefCount(i) == 0)   // Blatt
          {
            unsigned int next = f.Father(i);
            double len = Distance(points[i], points[next]);

            while (!f.isRoot(next) && f.getRefCount(next) == 1)
              {
                int k = next;
                next = f.Father(k);
                len += Distance(points[k], points[next]);
              }

            if (len < minlen)
              {
                unsigned int next = f.Father(i);
                f.setFather(i);

                while (!f.isRoot(next) && f.getRefCount(next) == 1)
                  {
                    int k = next;
                    next = f.Father(k);
                    f.setFather(k);
                  }
              }
          }
      }

    return OK;
  }

  int cutShortBranches(Forest& f, const Matrix& points, double minlen)
  {
    vector<Point> pl;

    for (int i = 0; i < points.rows(); i++)
      {
        pl.push_back(Point(points[i]));
      }

    return cutShortBranches(f, pl, minlen);
  }
#undef FNAME
#define FNAME "SplitToBranches"
  vector<vector<int> > SplitToBranches(Forest f)
  {
    vector<vector<int> > res;

#if 0
    int nTree = f.nTree(true); // number of trees

    if (nTree > 1)
      {
        throw IceException(FNAME, "More than one tree", ERROR);
      }

#endif
    // ein Blatt suchen
    int blattindex = -1;

    for (int i = 0; i < f.Size() && blattindex < 0; i++)
      {
        if (f.isLeaf(i))
          {
            blattindex = i;
          }
      }

    // Blatt muss existieren!
    if (blattindex < 0)
      {
        // Message(FNAME, "No tree", ERROR);
        return res;
      }

    //    cout << "Root -> " << blattindex << ": " << refcount[blattindex] << endl;
    f.makeRoot(blattindex);

    for (int i = 0; i < f.Size(); i++)
      {
        if (!f.isRoot(i) &&
            (
              (f.getRefCount(i) == 0) || // Blatt
              (f.getRefCount(i) > 1))   // Verzweigung
           )
          {
            // cout << "## " << i << " (" << refcount[i] << ") " ;
            vector<int> path;
            int aidx = f.Father(i);
            // cout << " -> " << aidx << " (" << refcount[aidx] << ") ";
            path.push_back(i);
            path.push_back(aidx);

            while (!f.isRoot(aidx) && f.getRefCount(aidx) == 1)
              {
                aidx = f.Father(aidx);
                path.push_back(aidx);
                // cout << " -> " << aidx << " (" << refcount[aidx] << ") ";cout.flush();
                // getchar();
              }

            res.push_back(path);
            // cout << endl;
          }
      }

    return res;
  }
#undef FNAME
}

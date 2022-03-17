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
#ifndef MINTREE_H
#define MINTREE_H

#include <vector>
#include "vectordistance.h"
#include "Matrix.h"
#include "forest.h"

namespace ice
{
  double computeMinTree(const std::vector<Point>& pointlist, Forest& f,
                        double maxlen = 0.0);
  double computeMinTree(const Matrix& pointList, Forest& f,
                        double maxlen = 0.0);

  class MinTree
  {
  public:
    template<typename T>
    MinTree(const std::vector<T>& pointList):
      nPoints(pointList.size()), tree(nPoints), edgeLength(nPoints)
    {
      if (nPoints < 1)
        {
          throw IceException("MinTree::MinTree", M_EMPTY_POINTLIST);
        }

      // zum Start ist nPoints-1 der erste und einzige Punkt des Baumes == Wurzel

      std::vector<bool> isInTree(nPoints, false);
      isInTree[nPoints - 1] = true;

      // Kanten
      // für Punkte ausserhalb des Baumes (isInTree == false): kürzeste Verbindung dieses Punktes zum Baum
      // für Punkte im Baum (isInTree == true): eine Kante des Baumes
      // initialisierung: Verbindung mit einzigem Punkt des Baumes nPoints - 1
      std::vector<unsigned int> qu(nPoints, nPoints - 1);

      // (Quadrat der) Distanz des Punktes zum Baum (isInTree==false)
      // Länge der Kante i <-> qu[i] (isInTree==true)

      T root = pointList[nPoints - 1];

      for (int i = 0; i < nPoints - 1; i++)
        {
          edgeLength[i] = (pointList[i] - root).length2();
        }

      edgeLength[nPoints - 1] = 0.0;

      int mini;

      // solange unverarbeitete Punkte: wähle nächsten Punkt
      while (findMin(isInTree, mini))
        {
          // std::cout << mini << std::endl;
          // neue Kante: mini <-> qu[mini]
          tree.setFather(mini, qu[mini]);
          isInTree[mini] = true;

          // Update der (Quadrate der) Distanzen Punkt <-> Baum
          // neuer Punkt mini des Baumes koennte Verbindungen anderer Knoten zum Baum verkuerzen

          const T& pmin = pointList[mini];
          for (int i = 0; i < nPoints; i++)
            {
              if (! isInTree[i])
                {
                  // i <-> mini ist Kandidat für eine kürzere Verbindung zum Baum
                  // double newDistance = Distance2(pmin, pointList[i]);
                  double newDistance = (pmin - pointList[i]).length2();
                  if (newDistance < edgeLength[i])   // kuerzer ?!
                    {
                      edgeLength[i] = newDistance;
                      qu[i] = mini;
                    }
                }
            }
        }
    }

    MinTree(const std::vector<std::vector<double> >& pointlist,
            const VectorDistance& vd = EuclideanVectorDistance());
    MinTree(const Matrix& distances);
    double getTree(Forest& f) const;
    double getForest(Forest& f, double maxLength = 0.0) const;
    double getCluster(Forest& f, int nCluster) const;

  private:
    bool findMin(const std::vector<bool>& b,
                 int& minidx);

    int nPoints;
    Forest tree;
    std::vector<double> edgeLength;
  };

  std::vector<std::vector<int> > SplitToBranches(Forest f);

  int cutShortBranches(Forest& f, const std::vector<Point>& pointlist,
                       double minlen);
  int cutShortBranches(Forest& f, const Matrix& pointlist,
                       double minlen);
}
#endif

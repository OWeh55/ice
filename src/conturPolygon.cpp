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

#include "freeman.h"
#include "contools.h"
#include "fit.h"
#include "polygonim.h"

#include "conturPolygon.h"

namespace ice
{
#define FNAME "pointInside"
  bool pointInside(double x, double y, const Matrix& pl)
  {

    if (pl.rows() < 3 || pl.cols() < 2)
      throw IceException(FNAME, M_MATRIXFORMAT);

    Matrix plw = Matrix(pl);
    Matrix pcl = Matrix(1, 2);

    if (pl[0][0] != pl[pl.rows() - 1][0] || pl[0][1] != pl[pl.rows() - 1][1])
      {
        plw.append(plw[0]);
      }

    int cr_nr = 0;

    for (int i = 0; i < plw.rows() - 1; i++)
      {
        //Eckpunkte
        if (x == plw[i][0] && y == plw[i][1])
          {
            return true;
          }

        //Punkte auf waagerechten Abschnitten
        if (y == plw[i][1] && y == plw[i + 1][1] && ((x >= plw[i][0] && x <= plw[i + 1][0]) || (x <= plw[i][0] && x >= plw[i + 1][0])))
          {
            return true;
          }

        if ((plw[i][1] <= y && plw[i + 1][1] > y) || (plw[i][1] > y && plw[i + 1][1] <= y))
          {
            double h1 = (x - plw[i][0]) * (plw[i + 1][1] - plw[i][1]);
            double h2 = (y - plw[i][1]) * (plw[i + 1][0] - plw[i][0]);

            //Punkt auf Kante
            if (h1 == h2)
              {
                return true;
              }

            if (plw[i + 1][1] - plw[i][1] > 0)
              {
                if (h1 < h2)
                  {
                    cr_nr++;
                  }
              }
            else
              {
                if (h1 > h2)
                  {
                    cr_nr++;
                  }
              }
          }
      }

    return cr_nr % 2;
    /*
    //ORIGINAL: Punkte auf Kante nicht innerhalb!
    int cr_nr=0;
    for(int i=0;i<plw.rows()-1;i++){
    if((plw[i][1]<=y && plw[i+1][1]>y) || (plw[i][1]>y && plw[i+1][1]<=y)){
    double h1 = (y-plw[i][1])/double(plw[i+1][1]-plw[i][1]);
    if(x<plw[i][0]+h1*(plw[i+1][0]-plw[i][0]))
    cr_nr++;
    }
    }
    return cr_nr%2;
    */
  }

  bool pointInside(const Point& p, const Matrix& pl)
  {
    return pointInside(p.x, p.y, pl);
  }

  bool pointInside(double x, double y, const Contur& c)
  {
    if (!c.isValid())
      throw IceException(FNAME, M_INVALID_CONTUR);

    int nPoints = c.Number();

    if (nPoints < 3)
      throw IceException(FNAME, M_TOO_LESS_POINTS);

    Matrix pl = Matrix(nPoints, 2);
    int hx, hy;

    for (int i = 0; i < nPoints; i++)
      {
        c.getPoint(i, hx, hy);
        pl[i][0] = hx;
        pl[i][1] = hy;
      }

    return pointInside(x, y, pl);
  }

  bool pointInside(const Point& p, const Contur& c)
  {

    return pointInside(p.x, p.y, c);
  }
#undef FNAME

#define FNAME "conturFromPolygon"
  int conturFromPolygon(const Matrix& pl, Contur& c)
  {

    if (pl.rows() < 3 || pl.cols() < 2)
      throw IceException(FNAME, M_MATRIXFORMAT);

    Matrix plw(pl);
    Matrix pcl(1, 2);

    if (pl[0][0] != pl[pl.rows() - 1][0] || pl[0][1] != pl[pl.rows() - 1][1])
      {
        plw.append(pl[0]);
      }

    if (!c.isValid())
      {
        c.reset();
      }

    //erst Startpunkt und dessen Nachbarn hinzufuegen, damit last und
    //prelast definiert, danach "aeusseren Bogen" zum naechsten Punkt schlagen

    bool leftobj = false;
    int freeman = 0;
    int x, y;

    int stx = (int)plw[0][0];
    int sty = (int)plw[0][1];
    c.setStart(stx, sty);

    // last added point
    int lastaddx = stx;
    int lastaddy = sty;

    freeman = 8;
    leftobj = false;

    while (!leftobj)
      {
        freeman--;

        if (freeman == -1)
          {
            freeman = 7;
          }

        Freeman(freeman).move(lastaddx, lastaddy, x, y);

        if (!pointInside(x, y, pl))
          {
            leftobj = true;
          }
      }

    while (leftobj)
      {
        freeman--;

        if (freeman == -1)
          {
            freeman = 7;
          }

        Freeman(freeman).move(lastaddx, lastaddy, x, y);

        if (pointInside(x, y, pl))
          {
            leftobj = false;
          }
      }

    c.add(x, y);
    lastaddx = x;
    lastaddy = y;

    //bis Startpunkt erneut hinzugefuegt
    while (!(lastaddx == stx && lastaddy == sty))
      {
        // Freeman-Richtung vom letzten zum vorletzten Konturpunkt, von da
        // aus aeusseren Bogen zum naechsten innneren Punkt schlagen
        freeman = (freeman + 4) % 8;
        leftobj = true;

        while (leftobj)
          {
            freeman--;

            if (freeman == -1)
              {
                freeman = 7;
              }

            Freeman(freeman).move(lastaddx, lastaddy, x, y);

            if (pointInside(x, y, pl))
              {
                leftobj = false;
              }
          }

        c.add(x, y);
        lastaddx = x;
        lastaddy = y;
      }

    if (c.isHole())
      {
        c.invertDir();
      }

    return OK;
  }
#undef FNAME
}

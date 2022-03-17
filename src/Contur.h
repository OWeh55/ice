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
#ifndef _CONTUR_H
#define _CONTUR_H

#include <vector>

#include "defs.h"
#include "base.h"
#include "freeman.h"
#include "Point.h"
#include "Window.h"

namespace ice
{
  class Contur
  {
  public:

    Contur();  // Standard-Konstruktor;
    Contur(const Contur& c);  // Copy-Konstruktor;
    explicit Contur(IPoint p); // Konstruktor mit gegebenem Startpunkt;
    Contur(int x, int y); // Konstruktor mit gegebenem Startpunkt;

    ~Contur();

    int reset();        // re-initialisation
    int reset(IPoint p);  // re-initialisation with new start point p
    int reset(int x, int y);  // re-initialisation with new start point (x,y)

    int setStart(IPoint p);
    int setStart(int x, int y)
    {
      return setStart(IPoint(x, y));
    }

    // Aufbau der Kontur
    int add(Freeman dir);   // Richtung anhängen

    int add(IPoint p); // Verbindung zu Punkt p anhängen
    int add(int x, int y) // Verbindung zu Punkt (x,y) anhängen
    {
      return add(IPoint(x, y));
    }

    void close() // close Contur = return to start point
    {
      if (isvalid && !isclosed)
        {
          add(start);
        }
    }

    int add(const Contur& c); // Contur anhängen

    Contur& operator=(const Contur&);

    friend Contur operator+(const Contur& c1, const Contur& c2);

    inline int isValid() const
    {
      return isvalid;
    };

    inline int isHole() const
    {
      return ishole;
    };

    inline int isClosed() const
    {
      return isclosed;
    };

    inline int Number() const
    {
      return nDirectionCodes;
    } ;

    inline double Length() const
    {
      return length;
    } ;

    inline IPoint Start() const
    {
      return start;
    };

    inline IPoint End() const
    {
      return endp;
    };

    //    void getPoints(std::vector<Point>& pl, bool close = false) const;
    //    void getPoints(std::vector<IPoint>& pl, bool close = false) const;

#define FNAME "Contur::getPoints"
    template<typename T>
    void getPoints(std::vector<T>& pl, bool close = false) const
    {
      pl.clear();

      if (!isvalid)
        throw IceException(FNAME, M_NOT_INITIALIZED);

      int nPoints = nDirectionCodes + 1;
      // if contur contains more than one point
      // and is closed, the last point is equal to start
      // we leave this out of not close is wished
      if (isclosed && !close && nPoints > 1)
        {
          nPoints--;  // do not add start point again
        }

      pl.resize(nPoints);

      IPoint res = start;
      pl[0] = res;

      for (int i = 1; i < nPoints; i++)
        {
          data[i - 1].move(res);
          pl[i] = res;
        }
    }
#undef FNAME
    int getPoint(int nr, int& x, int& y) const;
    IPoint getPoint(int nr) const;

    int getRect(int& xi, int& yi, int& xa, int& ya) const;
    int getRect(Window& win) const;

    inline unsigned char DirCode(int i) const
    {
      return (unsigned char)data[i].Int();
    } ;

    inline const Freeman& getDirection(int i) const
    {
      return data[i];
    } ;

    inline const std::vector<Freeman>& getDirections() const
    {
      return data;
    }

    inline int getDirections(std::vector<Freeman>& fc) const
    {
      if (!isvalid)
        {
          return ERROR;
        }
      fc = data;
      return OK;
    }

    int getPairs(std::vector<IPoint>& pl, std::vector<Freeman>& fc) const;
    int getPairs(std::vector<IPoint>& opl, std::vector<IPoint>& upl) const;

    int invertDir();

  protected:
    int Init();   // Initialisierung
    int Init(IPoint p);   // Initialisierung mit Startpunkt

    int isvalid;
    int nDirectionCodes; // number of direction codes

    double length;

    IPoint start; /* Startpunkt der Kontur */
    IPoint endp;  /* Endpunkt der Kontur */

    int ishole;         /* true: Lochkontur, false: Objektcontur*/
    int isclosed;       /* true: geschl. Kontur */

    int xmin;   /* Koordinaten des umschr. Rechtecks */
    int xmax;
    int ymin;
    int ymax;

    int curv;             // globale Krümmung
    std::vector<Freeman> data;

  private:
    mutable int cachedIndex;       // "Cache" für Punkt-Abfragen
    mutable IPoint cachedPoint;
  };
}
#endif

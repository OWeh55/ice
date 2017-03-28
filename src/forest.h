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
#ifndef FOREST_H
#define FOREST_H
#include <algorithm>

#include <stdlib.h>
#include <vector>

#include "defs.h"

namespace ice
{
  //============================================================================================0

  /** eine Klasse zur Speicherung einer Menge Bäume,
   * die alle Elemente aus einer Basis-Menge beinhalten
   * Jedes Element der Basismenge ist Bestandteil eines Baumes.
   * Nach der Initialisierung bildet jedes Element seinen
   * eigenen Baum.
   * Die Menge wird auf ein Array abgebildet, welches
   * jeweils den Verweis (index) auf den Vater enthält.
   * Das Root-Element erhält einen speziellen Wert.
   */

  class Forest
  {
  public:
    static const int rootval = -1;

    Forest(int s = 0): size(s)
    {
      if (size > 0)
        {
          sarray = new int[size];
          carray = new int[size];
          init();
        }
      else
        {
          sarray = nullptr;
          carray = nullptr;
        }
    }

    Forest(const Forest& f): size(f.size)
    {
      sarray = new int[size];
      carray = new int[size];

      for (int i = 0; i < size; i++)
        {
          sarray[i] = f.sarray[i];
          carray[i] = f.carray[i];
        }
    }

    friend void swap(Forest& f1, Forest& f2)
    {
      std::swap(f1.sarray, f2.sarray);
      std::swap(f1.carray, f2.carray);
      std::swap(f1.size, f2.size);
    }

    const Forest& operator=(Forest f)
    {
      swap(*this, f);
      return *this;
    }

    virtual ~Forest()
    {
      delete [] sarray;
      delete [] carray;
    }

    virtual void Reset(int n)
    {
      if (n != size)
        {
          delete [] sarray;
          delete [] carray;
          sarray = new int[n];
          carray = new int[n];
          size = n;
        }

      init();
    }

    virtual int Size() const
    {
      return size;
    }

    virtual int nTree(bool ignoreZeroLengthTrees = false) const;

    //    virtual void getRefCount(std::vector<int> &ct) const;

    virtual int getRefCount(int x) const
    {
      return carray[x];
    }

    virtual int getNeighborCount(int x) const;

    virtual int Father(int x) const
    {
      return sarray[x];
    }

    virtual void setFather(int x, int f = rootval);

    virtual bool isRoot(int x) const
    {
      return sarray[x] == rootval;
    }

    virtual bool isLeaf(int x) const
    {
      return (sarray[x] != rootval) && (carray[x] == 0);
    }

    void makeRoot(int x)
    {
      int father = sarray[x]; // wer ist mein Vater ?

      if (father != rootval) // ich bin noch nicht Root
        {
          makeRoot(father);      // Vater zu Root machen...
          setFather(father, x);  // ... und zu meinem Kind erklären!
          setFather(x, rootval); // ich werde Root
        }
    }

    int Depth(int x) const
    {
      int depth = 0;

      while ((x = sarray[x]) != rootval)
        depth++;

      return depth;
    }

    int Root(int x) const
    {
      while (sarray[x] != rootval)
        {
          x = sarray[x];
        }

      return x;
    }

    bool isIsolated(int x) const
    {
      return carray[x] == 0;
    }

  protected:
    int* sarray;
    int* carray;
    int size;

  private:
    void init()
    {
      // initialisation - every element is isolated point
      for (int i = 0; i < size; i++)
        {
          sarray[i] = rootval;
          carray[i] = 0;
        }
    }
  }; // Forest
} // namespace
#endif

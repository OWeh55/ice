#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <vector>
#include "forest.h"

namespace ice
{
  class DisjointSets
  {
    /* Disjoint sets, bei denen jedes Element der Basismenge in
     * eine der disjunkten Mengen gehört
     * im Startzustand bildet jedes Element seine eigene Menge
     * Durch Verlinken jedes Elements einer Menge mit einem
     * anderen Element der Menge entsteht für jede Menge ein Baum.
     * Die Wurzel des Baumes verlinkt zu sich selbst und
     * ist der Repräsentant der Menge.
     */

    DisjointSets(const DisjointSets& s) = delete;

  public:

    DisjointSets(int s): sarray(s)
    {
      init();
    };

    ~DisjointSets()
    {
    }

    int find(int x) const
    {
      // Finds the representative of the set, given an element
      int r = sarray[x];

      if (r != x)
        {
          // Wurzel noch nicht gefunden, rekursiver abstieg
          r = find(r);
          // gefundene Wurzel direkt als Vater eintragen
          // dadurch wird der Weg für die nächste Suche
          // abgekürzt
          sarray[x] = r;
        }

      return r;
    }

    int linkRoot(int b1, int b2)
    {
      // combine 2 sets, given their _representative_
      sarray[b2] = b1;
      return b1;
    }

    int link(int s1, int s2)
    {
      // combine 2 sets, given one element of each set
      return linkRoot(find(s1), find(s2));
    }
  protected:
    mutable std::vector<int> sarray;

    void init()
    {
      // initialisation - every element is root of it´s own tree
      for (unsigned int i = 0; i < sarray.size(); i++)
        sarray[i] = i;
    }

  }; // class DisjointSets
} // namespace ice
#endif

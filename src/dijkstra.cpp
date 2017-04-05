#include <queue>

#include "macro.h"
#include "arith.h"
#include "contools.h"
#include "dijkstra.h"

using namespace std;

namespace ice
{

// Minimaler Wertbereich für Markierungsbild.
// 0 = unbehandelt, 1= ziel, 2-9 = Richtungskode+2 für minimalen pfad

  const int minrange = 10;

  struct PointX: public IPoint
  {
    // Point with value and direction
    double val;
    Freeman dir;
    PointX(int x, int y, double v, int d): IPoint(x, y), val(v), dir(d) {}
    PointX(IPoint s, double v, int d): IPoint(s), val(v), dir(d) {}
    bool operator<(const PointX& second) const
    {
      // invers zu Operator < double, da priority_queue falsch herum arbeitet
      return val > second.val;
    }
  };

  // cost function
  double cf(const Image& img, const PointX& p)
  {
    return GetVal(img, p) + 1;
  }

#define FNAME "Dijkstra"
  Contur Dijkstra(const Image& img, IPoint start, Image& marker)
  {
    try
      {
        Contur res;
        priority_queue<PointX> heap;

        int dx, dy;

        MatchImg(img, marker, dx, dy);

        if (marker->maxval < minrange)
          throw IceException(FNAME, M_LOWRANGE, WRONG_PARAM);

        if (!Inside(img, start))
          throw IceException(FNAME, M_OUTSIDE, WRONG_PARAM);

        // prepare marker image:
        // 0           => 0 - unhandled
        // all markers => 1 - destination
        //
        BinImg(marker, marker, 1, 1);

        if (GetVal(marker, start.x, start.y) != 0)
          throw IceException(FNAME, M_WRONG_STARTPOINT, WRONG_PARAM);

        PointX ap(start, 0, 0);
        heap.push(ap);

        bool ready = false;

        while (!ready && heap.size() > 0)
          {
            ap = heap.top();
            heap.pop();
            int state = GetVal(marker, ap);

            if (state == 1)   // reached destination
              {
                ready = true;
              }

            if (state < 2)   // point unhandled
              {
                PutVal(marker, ap, ap.dir.Int() + 2);

                if (!ready)
                  {
                    for (int dir = 0; dir < 8 && !ready; dir++)
                      {
                        PointX np(ap);
                        Freeman(dir).move(np);

                        if (Inside(marker, np))
                          {
                            int mrk = GetVal(marker, np);

                            if (mrk < 2)   // unbehandelt
                              {
                                if (dir & 1)
                                  {
                                    np.val += cf(img, np) * 1.414;
                                  }
                                else
                                  {
                                    np.val += cf(img, np);
                                  }

                                np.dir = dir;
                                heap.push(np);
                              }
                          }
                      }
                  }
              }
          }

        res.SetStart(ap.x, ap.y);

        while (!(ap == start))
          {
            res.Add(ap.x, ap.y);
            int dir = (GetVal(marker, ap) - 2 + 4) % 8;
            Freeman(dir).move(ap);
          }

        res.Add(start);
        res.InvDir();
        return res;
      }
    RETHROW;
  }

  Contur Dijkstra(const Image& img, IPoint start, IPoint end)
  {
    Image mark = NewImg(img->xsize, img->ysize, minrange);
    clearImg(mark);
    PutVal(mark, end, 1);
    return Dijkstra(img, start, mark);
  }
#undef FNAME
}

#ifndef FREEMAN_H
#define FREEMAN_H

#include "Point.h"

namespace ice
{

  class Freeman
  {
  public:
    Freeman(): dircode(0) {}

    Freeman(int dir): dircode(dir & 7) {}

    Freeman(const Freeman& sec): dircode(sec.dircode) {}

    void move(int x, int y, int& a, int& b) const
    {
      a = x;
      b = y;
      move(a, b);
    }

    void move(int& x, int& y) const
    {
      x += dx[dircode];
      y += dy[dircode];
    }

    void move(IPoint& ps, IPoint& pd) const
    {
      move(ps.x, ps.y, pd.x, pd.y);
    }

    void move(IPoint& p) const
    {
      move(p.x, p.y);
    }

    IPoint step(IPoint ps) const
    {
      IPoint res = ps;
      move(res);
      return res;
    }

    Freeman Inverse() const
    {
      return *this + 4;
    }

    Freeman& operator+=(const Freeman& sec)
    {
      dircode = (dircode + sec.dircode) & 7;
      return *this;
    }
    Freeman operator+(const Freeman& sec) const
    {
      Freeman res = *this;
      res += sec;
      return res;
    }

    Freeman& operator-=(const Freeman& sec)
    {
      dircode = (dircode + sec.dircode) & 7;
      return *this;
    }
    Freeman operator-(const Freeman& sec) const
    {
      Freeman res = *this;
      res -= sec;
      return res;
    }

    Freeman& operator++()
    {
      dircode = (dircode + 1) & 7;
      return *this;
    }
    Freeman operator++(int dummy)
    {
      int h = dircode;
      dircode = (dircode + 1) & 7;
      return Freeman(h);
    }

    Freeman& operator--()
    {
      dircode = (dircode - 1) & 7;
      return *this;
    }
    Freeman operator--(int dummy)
    {
      int h = dircode;
      dircode = (dircode - 1) & 7;
      return Freeman(h);
    }

    bool operator==(const Freeman& sec) const
    {
      return dircode == sec.dircode;
    }

    int Int() const
    {
      return dircode;
    }

  private:
    int dircode;
    static const int dx[8];
    static const int dy[8];
  };
} // namespace

#endif

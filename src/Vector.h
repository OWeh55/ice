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
#ifndef VECTORT_H
#define VECTORT_H

#ifndef _MSC_VER
#define FILE_SEP ','
#else
#define FILE_SEP '#'
#endif

#include <algorithm>
#include <iostream>
#include <vector>
#include <iomanip>

#include "defs.h"
#include "IceException.h"
#include "numbase.h"
#include "Point.h"

namespace ice
{
  template <typename T>
  class VectorT
  {
  public:
    typedef T value_type;
  protected:
    T* data;
    unsigned int dim;
    unsigned int allocated;

    static unsigned int nextallocationsize(unsigned int adim)
    {
      // calculates allocation size from wished dim
      // strategy: allocations size is power of 2
      unsigned int a = 1;

      while (a < adim)
        {
          a = a + a;
        }

      return a;
    }

    void _alloc(unsigned int allocationsize)
    {
      if (allocationsize > 0)
        {
          allocated = allocationsize;
          data = new T[allocated];
        }
      else
        {
          allocated = 0;
          data = nullptr;
        }
    }

    void alloc(unsigned int dimension)
    {
      // internal method for new allocation
      dim = dimension;
      _alloc(nextallocationsize(dim));
    }

    inline bool need_realloc(unsigned int n)
    {
      return n > allocated || n * 2 < allocated;
    }

    void realloc(unsigned int n)
    {
      // change of size without keeping content
      if (need_realloc(n))
        {
          delete [] data;
          alloc(n);
        }
      else
        {
          dim = n;
        }
    }

    void resize(unsigned int n)
    {
      // change of size keeping content
      if (n == 0)   // nothing to keep
        {
          delete [] data;
          alloc(0);
        }
      else
        {
          if (need_realloc(n))
            {
              if (dim == 0)   // nothing to keep
                {
                  alloc(n);
                }
              else
                {
                  T* olddata = data;
                  int keep = dim;

                  if (n < dim)
                    {
                      keep = n;
                    }

                  alloc(n);

                  for (int i = 0; i < keep; i++)
                    {
                      data[i] = olddata[i];
                    }

                  delete [] olddata;
                }
            }
          else
            {
              dim = n;
            }
        }
    }

  public:
    void clear()
    {
      // special case of resize and realloc
      delete data;
      alloc(0);
    }

    T& at(int i)
    {
      return data[i];
    };
    const T& at(int i) const
    {
      return data[i];
    };

    inline int Size() const
    {
      return dim;
    };

    // compatibility
    inline unsigned int size() const
    {
      return dim;
    };
    unsigned int capacity() const
    {
      return allocated;
    }

    bool empty() const
    {
      return dim == 0;
    }

    inline const T* getDataPointer() const
    {
      return data;
    }
    inline T* getDataPointer()
    {
      return data;
    }

    void Resize(int newdim)
    {
      if (newdim < 0)
        throw IceException("Vector::Resize", M_WRONG_DIM);

      resize(newdim);
    }

    // Konstruktoren
    VectorT(): data(nullptr), dim(0), allocated(0) {}

    explicit VectorT(unsigned int n): data(nullptr), dim(n)
    {
      alloc(n);

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] = 0;
        }
    }

    explicit VectorT(unsigned int n, T* extern_data)
    {
      alloc(n);

      for (unsigned int i = 0; i < n; i++)
        {
          data[i] = extern_data[i];
        }
    }

    VectorT(const VectorT& v): data(nullptr), dim(v.dim)
    {
      alloc(v.dim);

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] = v.data[i];
        }
    }

    explicit VectorT(const std::vector<T>& v): data(nullptr), dim(v.size())
    {
      alloc(v.size());

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] = v[i];
        }
    }

    template <class TI>
    explicit VectorT(const VectorT<TI>& v)
    {
      alloc(v.size());

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] = (T)v[i];
        }
    }

    explicit VectorT(pointT<T> p)
    {
      alloc(2);
      data[0] = p.x;
      data[1] = p.y;
    }

    VectorT(T v1, T v2)
    {
      alloc(2);
      data[0] = v1;
      data[1] = v2;
    }

    VectorT(T v1, T v2, T v3)
    {
      alloc(3);
      data[0] = v1;
      data[1] = v2;
      data[2] = v3;
    }

    VectorT(T v1, T v2, T v3, T v4)
    {
      alloc(4);
      data[0] = v1;
      data[1] = v2;
      data[2] = v3;
      data[3] = v4;
    }

    VectorT(T v1, T v2, T v3, T v4, T v5)
    {
      alloc(5);
      data[0] = v1;
      data[1] = v2;
      data[2] = v3;
      data[3] = v4;
      data[4] = v5;
    }

    VectorT(T v1, T v2, T v3, T v4, T v5, T v6)
    {
      alloc(6);
      data[0] = v1;
      data[1] = v2;
      data[2] = v3;
      data[3] = v4;
      data[4] = v5;
      data[5] = v6;
    }

    VectorT(T v1, T v2, T v3, T v4, T v5, T v6, T v7)
    {
      alloc(7);
      data[0] = v1;
      data[1] = v2;
      data[2] = v3;
      data[3] = v4;
      data[4] = v5;
      data[5] = v6;
      data[6] = v7;
    }

    // Destruktor

    ~VectorT()
    {
      delete [] data;
    }

    friend void swap(VectorT& first, VectorT& second)
    {
      using std::swap;
      swap(first.dim, second.dim);
      swap(first.allocated, second.allocated);
      swap(first.data, second.data);
    }

    // Typwandlungs-Operatoren
    operator pointT<T>() const
    {
      if (dim != 2)
        {
          throw IceException("Vector::Point", M_WRONG_DIM);
        }
      return pointT<T>(data[0], data[1]);
    }

    // Zuweisung

    VectorT& operator = (VectorT v)
    {
      swap(*this, v);
      return *this;
    }

    // indizierter Zugriff auf Elemente
    const T& operator[](unsigned int i) const
    {
      if (i >= dim)
        throw IceException("Vector::operator[]", M_WRONG_INDEX);

      return data[i];
    }

    T& operator[](unsigned int i)
    {
      if (i >= dim)
        throw IceException("Vector::operator[]", M_WRONG_INDEX);

      return data[i];
    }

    VectorT operator()(unsigned int i1, unsigned int i2) const
    {
      if ((i1 > i2 + 1) || (i2 >= dim))
        throw IceException("Vector::operator()", M_WRONG_INDEX);

      VectorT ret(i2 - i1 + 1);

      for (unsigned int i = i1; i <= i2; i++)
        {
          ret.data[i - i1] = data[i];
        }

      return ret;
    }

    void setV(T x1, T x2, T x3, T x4)
    {
      if (dim < 4)
        throw IceException("Vector::setV", M_WRONG_DIM);

      data[3] = x4;

      setV(x1, x2, x3);
    }

    void setV(T x1, T x2, T x3)
    {
      if (dim < 3)
        throw IceException("Vector::setV", M_WRONG_DIM);

      data[2] = x3;
      setV(x1, x2);
    }

    void setV(T x1, T x2)
    {
      if (dim < 2)
        throw IceException("Vector::setV", M_WRONG_DIM);

      data[1] = x2;
      setV(x1);
    }

    void setV(T x1)
    {
      if (dim < 1)
        throw IceException("Vector::setV", M_WRONG_DIM);

      data[0] = x1;
    }

    void set(T val)
    {
      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] = val;
        }
    }

    void Exchange(unsigned int i1, unsigned int i2)
    {
      if ((i1 >= dim) || (i2 >= dim))
        throw IceException("Vector::Exchange", M_WRONG_INDEX);

      std::swap(data[i1], data[i2]);
    }

    void Delete(unsigned int i1, unsigned int i2)
    {
      if ((i1 > i2) || (i2 >= dim))
        throw IceException("Vector::Delete", M_WRONG_INDEX);

      unsigned int diff = i2 - i1 + 1;
      unsigned int newdim = dim - diff;

      for (unsigned int i = i1; i < newdim; i++)
        {
          data[i] = data[i + diff];
        }

      resize(newdim);
    }

    void Delete(unsigned int i1)
    {
      if (i1 >= dim)
        throw IceException("Vector::Delete", M_WRONG_INDEX);

      int newdim = dim - 1;

      for (unsigned int i = i1; i < dim; i++)
        {
          data[i] = data[i + 1];
        }

      resize(newdim);
    }

    void Sort(int order = 0)
    {
      switch (order)
        {
        case 0:
          sort(data, data + dim, std::less<T>());
          break;
        case 1:
          sort(data, data + dim, std::greater<T>());
          break;
        }
    }

    // arithmetische Operatoren
#define FNAME "VectorT::operator +"
    const VectorT& operator+=(const VectorT& h)
    {
      if (dim != h.dim)
        throw IceException(FNAME, M_WRONG_DIM);

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] += h.data[i];
        }

      return *this;
    }

    VectorT operator +(const VectorT& h) const
    {
      VectorT res = *this;
      res += h;
      return res;
    }

#undef FNAME
#define FNAME "VectorT::operator -"
    const VectorT& operator -= (const VectorT& rhs)
    {
      if (dim != rhs.dim)

        throw IceException(FNAME, M_WRONG_DIM);

      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] -= rhs.data[i];
        }
      return *this;
    }

    VectorT operator - (const VectorT& h) const
    {
      VectorT res = *this;
      res -= h;
      return res;
    }

    VectorT operator - () const
    {
      VectorT p(dim);

      for (unsigned int i = 0; i < p.dim; i++)
        {
          p.data[i] = -data[i];
        }

      return p;
    }
#undef FNAME

#define FNAME "VectorT::distance"
    T Distance(const VectorT& h) const
    {
      T dist = 0.0;

      if (dim != h.dim)
        throw IceException(FNAME, M_WRONG_DIM);

      for (unsigned i = 0; i < dim; i++)
        {
          dist += Sqr(data[i] - h.data[i]);
        }
      return sqrt(dist);
    }

    friend T Distance(const VectorT& g, const VectorT& h)
    {
      return g.Distance(h);
    }
#undef FNAME

    const VectorT& operator *= (double v)
    {
      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] *= v;
        }

      return *this;
    }

    VectorT operator *(T k) const
    {
      VectorT p = *this;
      p *= k;
      return p;
    }

    VectorT& operator /= (double v)
    {
      for (unsigned int i = 0; i < dim; i++)
        {
          data[i] /= v;
        }

      return *this;
    }

    VectorT operator /(T k) const
    {
      VectorT p = *this;
      p /= k;
      return p;
    }

    friend VectorT operator *(T k, const VectorT& h)
    {
      return h * k;
    }

    T operator *(const VectorT& h) const
    {
      T prod = 0;

      if (dim != h.dim)
        throw IceException("Vector::operator*", M_WRONG_DIM);

      for (unsigned int i = 0; i < dim; i++)
        {
          prod += data[i] * h.data[i];
        }

      return prod;
    }

    friend VectorT operator && (const VectorT& v, T vz)
    {
      VectorT p(v.dim + 1);

      for (unsigned int i = 0; i < v.dim; i++)
        {
          p.data[i] = v.data[i];
        }

      p.data[v.dim] = vz;
      return p;
    }

    void Append(T v)
    {
      unsigned int next = dim;
      resize(dim + 1);
      data[next] = v;
    }

    void Append(const VectorT& a)
    {
      unsigned int next = dim;
      resize(dim + a.dim);

      for (unsigned int i = 0; i < a.dim; i++)
        {
          data[next] = a[i];
          next++;
        }
    }

    double Length2() const
    {
      double l = 0;

      for (unsigned int i = 0; i < dim; i++)
        {
          l += Sqr(data[i]);
        }

      return l;
    }

    double length2() const
    {
      double l = 0;

      for (unsigned int i = 0; i < dim; i++)
        {
          l += Sqr(data[i]);
        }

      return l;
    }

    double abs2() const
    {
      return Length2();
    }

    double Length() const
    {
      return sqrt(Length2());
    }

    double length() const
    {
      return sqrt(length2());
    }

    double abs() const
    {
      return Length();
    }

    double absL1() const
    {
      double l = 0;

      for (unsigned int i = 0; i < dim; i++)
        {
          l += fabs(data[i]);
        }

      return l;
    }

    void Normalize()
    {
      double l = Length();

      if (l == 0)
        throw IceException("Vector::Normalize", M_WRONG_PARAM);

      for (unsigned int j = 0; j < dim; j++)
        {
          data[j] /= l;
        }
    }

    operator std::vector<T>() const
    {
      return std::vector<T>(data, data + dim);
    }

    friend bool operator == (const VectorT& v1, const VectorT& v2)
    {
      if (v2.Size() != v1.Size())
        {
          return false;
        }

      for (int i = 0; i < v1.Size(); i++)
        if (v1[i] != v2[i])
          {
            return false;
          }

      return true;
    }

    friend bool operator != (const VectorT& v1, const VectorT& v2)
    {
      return !(v1 == v2);
    }

    friend std::ostream& operator<<(std::ostream& os, const VectorT& v)
    {
      std::locale cLocale("C");
      std::locale oldLocale = os.imbue(cLocale);
      std::streamsize width = os.width(0);
      os << "<";

      if (v.dim > 0)
        {
          for (unsigned int i = 0; i < v.dim - 1; i++)
            {
              os << std::setw(width) << v.data[i] << FILE_SEP ;
            }

          os << v.data[v.dim - 1];
        }

      os << ">";
      return os;
    }

#define FNAME "VectorT::operator>>(VectorT)"
    friend std::istream& operator>>(std::istream& is, VectorT& v)
    {
      char c = 0;
      v.clear();
      is >> c;

      if (c != '<')
        {
          is.clear();
          throw IceException(FNAME, M_WRONG_FILE);
        }

      if (is.peek() == '>')
        {
          // empty VectorT
          is.ignore(1, '\n');
          return is;
        }

      do
        {
          T d;
          is >> d >> c ;
          v.Append(d);

          if ((c != ',') && (c != '#') && (c != '>'))
            {
              is.clear();
              throw IceException(FNAME, M_WRONG_FILE);
            }
        }
      while (c != '>');

      return is;
    }
#undef FNAME

    template<typename Ts>
    friend VectorT<Ts> Normalize(const VectorT<Ts>& v);
    template<typename Ts>
    friend VectorT<Ts> Cross(const VectorT<Ts>& u, const VectorT<Ts>& v);
  };

  typedef VectorT<double> Vector;
  typedef VectorT<int> IVector;
}
#endif

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
#ifndef SMART_REFERENCE_H
#define SMART_REFERENCE_H

namespace ice
{
  template<typename T>
  class ManagedData;

  template<typename T>
  class SmartReference
  {
  public:
    SmartReference(): datapointer(new ManagedData<T>)
    {
      datapointer->nRefs = 1;
    }

    SmartReference(const T& d): datapointer(new ManagedData<T>(d))
    {
      datapointer->nRefs = 1;
    }

    SmartReference(const SmartReference& source):
      datapointer(source.datapointer)
    {
      ++datapointer->nRefs;
    }

    const SmartReference& operator=(const SmartReference& source)
    {
      freedata();
      datapointer = source.datapointer;
      ++datapointer->nRefs;
      return *this;
    }

    ~SmartReference()
    {
      freedata();
    }

    void freedata()
    {
      --datapointer->nRefs;
      if (datapointer->nRefs == 0)
        {
          delete datapointer;
        }
      datapointer = nullptr;
    }

    T* operator ->() const
    {
      return datapointer;
    }

    T& operator *() const
    {
      return *datapointer;
    }

  private:
    ManagedData<T>* datapointer;
  };

  template<typename T>
  class ManagedData: public T
  {
  public:
    ManagedData(): T() {}
    ManagedData(const T& d): T(d) {}
    friend class SmartReference<T>;
  private:
    int nRefs;
  };
}
#endif

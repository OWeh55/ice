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

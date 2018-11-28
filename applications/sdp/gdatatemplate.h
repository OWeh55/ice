#ifndef SDP_G_DATA_TEMPLATE_H
#define SDP_G_DATA_TEMPLATE_H

#include <Point.h>

#include "gdata.h"

template<class T, int type>
class GDataTemplate: public GData
{
public:
  /**
   * ctor which initializes with empty array
   */
  GDataTemplate(): GData() {}
  /**
   * ctor which initializes array with one given value
   */
  GDataTemplate(T value): GData(), datavector(1, value) {}
  /**
   * ctor which initializes array with an given vector
   */
  GDataTemplate(const std::vector<T>& value): GData(), datavector(value) {}
  /**
   * virtual dtor
   */
  virtual ~GDataTemplate() {};

  virtual GData* clone() const
  {
    return new GDataTemplate<T, type>(*this);
  }

  virtual DType getType() const
  {
    return DType(type, datavector.size());
  };

  virtual int getSize() const
  {
    return datavector.size();
  }

  virtual T& operator[](int i)
  {
    checkIndex(i);
    return datavector[i];
  }

  virtual const T& operator[](int i) const
  {
    checkIndex(i);
    return datavector[i];
  }

  virtual void resize(int s)
  {
    datavector.resize(s);
  }

  virtual std::vector<T>& getVector()
  {
    return datavector;
  }

  virtual const std::vector<T>& getVector() const
  {
    return datavector;
  }

  virtual void push_back(const T& data)
  {
    datavector.push_back(data);
  }

  //  virtual std::string toString() const;

protected:
  std::vector<T> datavector;
};

#if 0
template<>
std::string GDataTemplate<std::string, DType::string_type>::toString() const
{
  if (datavector.size() == 1)
    return datavector[0];
  else
    {
      std::string res = "<";
      for (int i = 0; i < (int)datavector.size(); ++i)
        res += datavector[i] + ",";
      res[res.length() - 1] = '>';
      return res;
    }
}

template<>
std::string GDataTemplate<ice::Point, DType::point>::toString() const
{
  if (datavector.size() == 1)
    return to_string(datavector[0]);
  else
    {
      std::string res = "<";
      for (int i = 0; i < (int)datavector.size(); ++i)
        res += to_string(datavector[i]) + ",";
      res[res.length() - 1] = '>';
      return res;
    }
}

template<class T, int type> std::string GDataTemplate<T, type>::toString() const
{
  if (datavector.size() == 1)
    return std::to_string(datavector[0]);
  else
    {
      std::string res = "<";
      for (int i = 0; i < (int)datavector.size(); ++i)
        res += std::to_string(datavector[i]) + ",";
      res[res.length() - 1] = '>';
      return res;
    }
}
#endif

#endif

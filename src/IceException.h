#ifndef _ICE_EXCEPTION_H
#define _ICE_EXCEPTION_H

#include <string>
#include <exception>

namespace ice {
  class IceException: public std::exception {
  public:
    IceException(const string &where,const string &message): 
      where_(where),message_(message)
    {
    }
    virtual string what() const
    {
      return where_ + " - " + message;
    }
    virtual string where() const
    {
      return where_;
    }
    virtual string message() const
    {
      return message_;
    }
    virtual ~IceException() {}
  private:
    string where_;
    string message_;
  }
}
#endif

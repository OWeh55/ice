#ifndef _ICE_EXCEPTION_H
#define _ICE_EXCEPTION_H

#include <string>
#include <exception>

namespace ice
{
  class IceException: public std::exception
  {
  public:
    IceException(const std::string& where, const std::string& message):
      where_(where), message_(message)
    {
    }
    IceException(const IceException& ex, const std::string& newWhere):
      where_(newWhere), message_(ex.what())
    {
    }
    IceException(const std::string& where, const std::string& message, int code):
      where_(where), message_(message), code(code)
    {
    }
    IceException(const std::string& where, const std::string& message, const std::string& emessage, int code):
      where_(where), message_(message + " " + emessage), code(code)
    {
    }
    virtual const char* what() const noexcept(true)
    {
      return (where_ + " - " + message_).c_str();
    }
    virtual std::string where() const
    {
      return where_;
    }
    virtual std::string message() const
    {
      return message_;
    }
    virtual ~IceException() {}
  private:
    std::string where_;
    std::string message_;
    int code = 0;
  };
}
#endif

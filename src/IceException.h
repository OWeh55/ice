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
      where_(newWhere), message_(ex.message_)
    {
      where_ += "/" + ex.where_;
    }

    IceException(const std::string& where,
                 const std::string& message,
                 const std::string& emessage):
      where_(where), message_(message + " (" + emessage + ")")
    {
    }

    virtual const char* what() const noexcept(true) override
    {
      fullmessage = where_ + " - " + message_;
      return fullmessage.c_str();
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
    mutable std::string fullmessage;
  };
}

#define RETHROW catch (IceException &ex) { throw IceException(ex, FNAME); }
#endif

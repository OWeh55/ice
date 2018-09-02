#ifndef IM_EXCEPTIONS_H
#define IM_EXCEPTIONS_H

#include <string>
#include <exception>
#include "gdtype.h"

class SdpException: public std::exception
{
protected:
  std::string where;
  std::string msg;
public:
  SdpException(): where("unknown"), msg("unknown") {}
  SdpException(const std::string& where, const std::string& msg):
    where(where), msg(msg) {}

  virtual void setWhere(const std::string& w)
  {
    where = w;
  }

  virtual const char* what() const throw()
  {
    return (where + ": " + msg).c_str();
  }
  virtual ~SdpException() throw() {}
};

class WrongTypeException: public SdpException
{
public:
  WrongTypeException(const std::string& name, int expected, int got):
    SdpException(name, "expected " + DType::getString(expected) + ", got " + DType::getString(got))
  { }
};

class TypeMatchException: public SdpException
{
public:
  TypeMatchException(const std::string& name, int type1, int type2):
    SdpException(name, "Types do not match " + DType::getString(type1) + " <> " + DType::getString(type2))
  { }
};

class FileException: public SdpException
{
public:
  FileException(const std::string& where,
                const std::string& fn,
                const std::string& op = "open"):
    SdpException(where, "Cannot " + op + " file " + fn) { }
};

class EndOfSeriesException: public SdpException
{
public:
  EndOfSeriesException(const std::string& modul):
    SdpException(modul, "No further image") {};
};

#endif

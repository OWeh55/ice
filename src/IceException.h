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
                 const std::string& eMessage):
      where_(where), message_(message + " (" + eMessage + ")")
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

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
#ifndef STRTOOL_H
#define STRTOOL_H

/********************************************************
 * Utilities zur Stringbehandlung
 ********************************************************/

#include <string>
#include <sstream>

namespace ice
{
  std::string Split(std::string& s, const std::string& ss, int del = true, int need_del = false);

  void SplitFilename(std::string s, std::string& path, std::string& basename);
  void SplitFilename(std::string s, std::string& path, std::string& basename, std::string& ext);

  template<typename T>
  std::string NumberString(T val, int width = 0)
  {
    std::ostringstream os;
    if (width < 0)
      {
        os.fill('0');
        width = -width;
      }
    else
      {
        os.fill(' ');
      }

    os.width(width);

    if (os << val)
      {
        return os.str();
      }

    return "conversion error";
  }

  template<typename T>
  std::string NumberString(T val, int width, unsigned int prec)
  {
    std::ostringstream o;
    o.width(width);

    if (prec > 0)
      {
        o.precision(prec);
      }

    if (o << val)
      {
        return o.str();
      }

    return "conversion error";
  }

  std::string CharString(char c);

  int Integer(const std::string& s);
  double Double(const std::string& s);

  std::string trim(const std::string& s);
  std::string lowercase(const std::string& s);
  std::string uppercase(const std::string& s);

  std::wstring WString(const std::string& str);
  std::string String(const std::wstring& str);
}

#endif /* STRTOOL_H */

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
/*************************************************************
   Tools zur Stringbehandlung
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sstream>

#include "defs.h"
#include "strtool.h"
#include "IceException.h"

namespace ice
{

  using namespace std;

#define FNAME "Split"
  string Split(string& s, const string& ss, int del, int need_del)
  {
    string hs;
    string::size_type pos = s.find(ss);

    if (pos == string::npos)
      {
        // delimiter not found
        if (!need_del)
          {
            // split w/o delimiter = split all
            hs = s;

            if (del)
              {
                s = "";
              }
          }
        else
          {
            // can't split without del
            hs = "";
          }
      }
    else
      {
        hs = s.substr(0, pos);

        if (del)
          {
            s.erase(0, pos + 1);
          }
      }

    return hs;
  }
#undef FNAME

  void SplitFilename(std::string s, std::string& path, std::string& basename)
  {
    // split path and basename
    string::size_type  pos = s.find_last_of('/');

    if (pos == string::npos)   /* no path */
      {
        path = "";
        basename = s;
      }
    else
      {
        path = s.substr(0, pos);
        basename = s.substr(pos + 1);
      }
  }

  void SplitFilename(std::string s, std::string& path, std::string& basename, std::string& ext)
  {
    SplitFilename(s, path, basename);

    // split basename and extension
    string::size_type pos = basename.find_last_of('.');

    if (pos == string::npos)
      {
        ext = "";
      }
    else
      {
        ext = basename.substr(pos + 1);
        basename = basename.substr(0, pos);
      }
  }

#define FNAME "CharString"
  string CharString(char c)
  {
    string hs = "";
    return hs + c;
  }
#undef FNAME

  int Integer(const string& s)
  {
    return atol(s.c_str());
  }

  double Double(const string& s)
  {
    return atof(s.c_str());
  }

  string lowercase(const string& s)
  {
    string hs;
    string::size_type i;

    for (i = 0; i < s.length(); i++)
      {
        hs += tolower(s[i]);
      }

    return hs;
  }

  string uppercase(const string& s)
  {
    string hs;
    string::size_type i;

    for (i = 0; i < s.length(); i++)
      {
        hs += toupper(s[i]);
      }

    return hs;
  }

  string trim(const string& s)
  {
    string::size_type i1 = 0;
    string::size_type i2 = s.size() - 1;

    while ((i1 <= i2) && (s[i1] == ' '))
      {
        i1++;
      }

    while ((i2 >= i1) && (s[i2] == ' '))
      {
        i2--;
      }

    return s.substr(i1, i2 - i1 + 1);
  }

#define FNAME "WString"
  wstring WString(const string& str)
  {
    wstring res;
    int wchars = mbstowcs(nullptr, str.c_str(), 0);

    if (wchars < 0)
      throw IceException(FNAME, M_WRONG_CODING);

    wchar_t* ws = new wchar_t[wchars + 2];
    mbstowcs(ws, str.c_str(), wchars + 1);
    res = ws;
    delete [] ws;
    return res;
  }
#undef FNAME
#define FNAME "String"
  string String(const wstring& str)
  {
    string res;
    int chars = wcstombs(nullptr, str.c_str(), 0);

    if (chars < 0)
      throw IceException(FNAME, M_WRONG_CODING);

    char* ms = new char[chars + 2];
    wcstombs(ms, str.c_str(), chars + 1);
    res = ms;
    delete [] ms;
    return res;

  }
#undef FNAME
}

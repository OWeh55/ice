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

#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>

#include "dirfunc.h"
#include "strtool.h"

namespace ice
{
  int charequal(const char* s1, const char* s2)
  {
    // compare first letter if filename/filemask
    if (*s1 == '?')
      {
        return true;
      }

    if (*s2 == '?')
      {
        return true;
      }

    return *s1 == *s2;
  }

  int CompareString(const char* s1, const char* s2)
  {
    /* Test ob beide Strings zu Ende */
    if (*s1 == '\0' && *s2 == '\0')
      {
        return true;
      }

    /* wenn ein String laenger als der andere, dann sind sie ungleich */
    if (*s1 == '\0' || *s2 == '\0')
      {
        return false;
      }

    /* der Stern in s2 */
    if (*s2 == '*')
      {
        s2++;

        while (*s1 != '\0')
          if (CompareString(s1++, s2))
            {
              return true;
            }

        return *s2 == '\0';
      }

    /* der Normalfall */
    if (!charequal(s1, s2))
      {
        return false;
      }

    return CompareString(s1 + 1, s2 + 1);
  }

  void Directory(std::vector<std::string>& zz, std::string path, const std::string& mask, int mode)
  {
    std::vector<std::string> z;
    DIR* dir;
    struct dirent* direntry;

    if (path.empty())   // current directory
      {
        path = ".";
      }

    if ((dir = opendir(path.c_str())) != NULL)
      {
        while ((direntry = readdir(dir)) != NULL)
          {
            if (CompareString(direntry->d_name, mask.c_str()))
              {
                std::string basename = direntry->d_name;
                std::string file = path + "/" + basename;
                struct stat buf;

                if (stat(file.c_str(), &buf) == 0)
                  {
                    bool selected = false;

                    if ((mode & DIR_FILE) && (S_ISREG(buf.st_mode)))
                      {
                        selected = true;
                      }

                    if ((mode & DIR_DIR) && (S_ISDIR(buf.st_mode)) && basename != "." && basename != "..")
                      {
                        selected = true;
                      }

                    if (selected)
                      {
                        if (mode & DIR_WITHPATH)
                          {
                            z.push_back(path + "/" + direntry->d_name);
                          }
                        else
                          {
                            z.push_back(direntry->d_name);
                          }
                      }
                  }
              }
          }

        closedir(dir);
      }

    sort(z.begin(), z.end());

    for (int i = 0; i < (int)z.size(); i++)
      {
        zz.push_back(z[i]);
      }
  }

  void Directory(std::vector<std::string>& z, const std::string& s, int mode)
  {
    std::string mask, path;
    SplitFilename(s, path, mask);
    Directory(z, path, mask, mode);
  }
}

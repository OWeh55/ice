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

#include "strtool.h"
#include "dirfunc.h"
#include "visual/screen.h"
#include "visual/xio.h"
#include "visual/menu.h"

namespace ice
{
  void frame(int x1, int y1, int x2, int y2, string title)
  {
    unsigned int width = x2 - x1 - 1; // width of inner space

    // upper border (with title)
    if (title.length() < width - 1 && title.length() > 0)
      {
        title = " " + title + " ";
      }

    unsigned int right = width / 2;
    unsigned int left = width - right;
    unsigned int tlen = 0;
    if (title.length() <= width)
      {
        tlen = title.length();
        unsigned int tright = tlen / 2;
        unsigned int tleft = tlen - tright;
        left -= tleft;
        right -= tright;
      }

    SetAlphaCursor(x1, y1);
    PutChar(R_LO); // upper left corner
    for (unsigned int n = 0; n < left; n++)   // upper border (left)
      {
        PutChar(R_OU);  // upper right corner
      }
    for (unsigned int n = 0; n < tlen; n++)   // upper border (left)
      {
        PutChar(title[n]);  // title
      }
    for (unsigned int n = 0; n < right; n++)   // upper border (right)
      {
        PutChar(R_OU);  // upper right corner
      }
    PutChar(R_RO);

    for (int y = y1 + 1; y < y2; y++)
      {
        SetAlphaCursor(x1, y);
        PutChar(R_LR); // left border
        for (unsigned int n = 0; n < width; n++)
          {
            Printf(" ");  // space
          }
        PutChar(R_LR); // right border
      }

    SetAlphaCursor(x1, y2);
    PutChar(R_LU);
    for (unsigned int n = 0; n < width; n++)
      {
        PutChar(R_OU);
      }
    PutChar(R_RU);
  }

#define FNAME "Menu"

  static char findselection(const string& item)
  {
    char first = 0;
    char selection = 0;
    for (unsigned int i = 0; i < item.length() - 1; i++)
      {
        char c = tolower(item[i]);
        if (c == '~')
          {
            selection = tolower(item[i + 1]);
            i++;
          }
        else if (c == '#')
          {
            i++;  // ignore attribute
          }
        else
          {
            if (first == 0)
              if (c != '.')
                {
                  first = c;
                }
          }
      }
    if (selection != 0)
      {
        return selection;
      }
    if (first != 0)
      {
        return first;
      }
    return ' ';
  }

  static string extendeditem(const string& item)
  {
    char selection = findselection(item);
    return selection + item;
  }

  int findnextitem(const vector<string>& items, int current, char sel)
  {
    int i = (current + 1) % items.size();
    sel = tolower(sel);
    while (i != current && items[i][0] != sel)
      {
        i = (i + 1) % items.size();
      }
    return i;
  }

  void printstring(const string& ts, int dx)
  {
    TextAttribs attr_old = GetTextAttributes();

    unsigned int idx = 1;
    int pos = 1;

    while ((idx < ts.length()) && (pos < dx))
      {
        if (ts[idx] == '~')
          {
            SetTextAttributes(TextAttribs(TextAttribs::Highlighted));  // high
            idx++;
            if ((idx < ts.length()) && (pos < dx))
              {
                PutChar(ts[idx]);
                pos++;
              }
            SetTextAttributes(TextAttribs(TextAttribs::NotHighlighted));  // not high
          }
        else if (ts[idx] == '#')
          {
            char attr = ts[idx + 1];
            switch (attr)
              {
              case 'H':
                SetTextAttributes(TextAttribs::Highlighted);
                break;
              case 'h':
                SetTextAttributes(TextAttribs::NotHighlighted);
                break;
              case 'I':
                SetTextAttributes(TextAttribs::Inverted);
                break;
              case 'i':
                SetTextAttributes(TextAttribs::NotInverted);
                break;
              case 'R':
                SetTextAttributes(TextAttribs::FgColorRed);
                break;
              case 'G':
                SetTextAttributes(TextAttribs::FgColorGreen);
                break;
              case 'B':
                SetTextAttributes(TextAttribs::FgColorBlue);
                break;
              case 'Y':
                SetTextAttributes(TextAttribs::FgColorYellow);
                break;
              case 'K':
                SetTextAttributes(TextAttribs::FgColorBlack);
                break;
              case 'C':
                SetTextAttributes(TextAttribs::FgColorCyan);
                break;
              case 'M':
                SetTextAttributes(TextAttribs::FgColorMagenta);
                break;
              case 'W':
                SetTextAttributes(TextAttribs::FgColorWhite);
                break;
              case 'r':
                SetTextAttributes(TextAttribs::BgColorRed);
                break;
              case 'g':
                SetTextAttributes(TextAttribs::BgColorGreen);
                break;
              case 'b':
                SetTextAttributes(TextAttribs::BgColorBlue);
                break;
              case 'y':
                SetTextAttributes(TextAttribs::BgColorYellow);
                break;
              case 'k':
                SetTextAttributes(TextAttribs::BgColorBlack);
                break;
              case 'c':
                SetTextAttributes(TextAttribs::BgColorCyan);
                break;
              case 'm':
                SetTextAttributes(TextAttribs::BgColorMagenta);
                break;
              case 'w':
                SetTextAttributes(TextAttribs::BgColorWhite);
                break;
              }
            idx++;
          }
        else
          {
            PutChar(ts[idx]);
            pos++;
          }
        idx++;
      }
    while (pos < dx)
      {
        PutChar(' ');  // fill with spaces
        pos++;
      }
    SetTextAttributes(attr_old);
  }

  void printitem(const vector<string>& items, int current, int selected, int x, int y, int dx)
  {
    SetAlphaCursor(x, y);
    if (current == selected)
      {
        PutChar('*');  //  mark selected
      }
    else
      {
        PutChar(' ');
      }

    printstring(items[current], dx);
  }

  int Menu(const vector<string>& zz,
           int x1, int y1, int x2, int y2,
           bool restore, const string& title)
  {
    vector<string> Men = zz; // internal (not constant) menu list

    if (zz.empty())
      {
        return ERROR;
      }

    /* convert strings with keys */
    for (vector<string>::iterator a = Men.begin(); a != Men.end(); a++)
      {
        *a = extendeditem(*a);
      }

    bool abort = false;
    bool done = false;

    TextAttribs old_attr = GetTextAttributes();

    if (restore)
      {
        PushAlpha();
      }

    // defaultwerte box
    if (x1 < 0)
      {
        x1 = 2;
      }
    if (y1 < 0)
      {
        y1 = 2;
      }
    if (x2 < 0)
      {
        x2 = 30;
      }
    if (y2 < 0)
      {
        y2 = 23;
      }

    frame(x1, y1, x2, y2, title);
    SetAttribute(-1, -1, -1, 0); // Intensität normal

    int x = x1 + 1; // item start x
    int dx = x2 - x1 - 1; // maximum item length

    int y = y1 + 1; // first item y

    int selected_item = 0; /* nummer des gewählten Menu-Punktes */
    int first_item = 0; /* nummer des ersten dargestellten punktes */
    while (!abort && !done)
      {
        x = x1 + 1;
        dx = x2 - x1 - 1;
        y = y1 + 1; // coordinates of item-strings

        int current_item = first_item;
        // Menu darstellen
        while ((y < y2) && (current_item < (int)Men.size()))   // till end of list or end of screen
          {
            printitem(Men, current_item, selected_item, x, y, dx);
            y++;
            current_item++;
            // next
          }
        int last_item = current_item - 1; // last index

        SetAlphaCursor(0, 0);

        int ein = GetCharW(); // input key
        switch (ein)
          {
          case K_Return:
            done = true;
            break;
          case K_Escape:
            abort = true;
            break;

          case K_Home:
            selected_item = 0;
            break;

          case K_End:
            selected_item = Men.size() - 1;
            break;

          case '+':
          case K_Down: // down
            selected_item++;
            break;

          case K_PageDown:// page down
            selected_item += 8;
            break;

          case '-':
          case K_Up:// up
            selected_item--;
            break;

          case K_PageUp:// page up
            selected_item -= 8;
            break;

          default:
          {
            char sel = (char)ein;
            selected_item = findnextitem(Men, selected_item, sel);
          }
          break;
          }
        // limit range of selection
        if (selected_item < 0)
          {
            selected_item = 0;
          }
        if (selected_item >= (int)Men.size())
          {
            selected_item = Men.size() - 1;
          }

        //
        if (selected_item < first_item)
          {
            first_item = selected_item;
          }
        if (selected_item > last_item)
          {
            first_item += (selected_item - last_item);
          }

      }

    SetTextAttributes(old_attr);
    if (restore)
      {
        PopAlpha();
      }

    if (done)
      {
        return selected_item;
      }
    else
      {
        return ERROR;
      }
  }

  int Menu(const vector<string>& zz, const vector<int>& id,
           int x1, int y1, int x2, int y2,
           bool restore, const string& title)
  {
    if (zz.size() != id.size())
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);
    int sel = Menu(zz, x1, y1, x2, y2, restore, title);
    if (sel >= 0)
      {
        sel = id[sel];
      }
    return sel;
  }

  int Menu(const vector<string>& zz, const IVector& id,
           int x1, int y1, int x2, int y2,
           bool restore, const string& title)
  {
    if (zz.size() != id.size())
      throw IceException(FNAME, M_DIFFERENT_LISTSIZE);
    int sel = Menu(zz, x1, y1, x2, y2, restore, title);
    if (sel >= 0)
      {
        sel = id[sel];
      }
    return sel;
  }
#undef FNAME

#define FNAME "FileSel"
  string FileSel(const string& mask,
                 int x1, int y1, int x2, int y2,
                 bool restore,
                 const string& title)
  {
    vector<string> men;
    int i;
    Directory(men, mask);
    if (!men.empty())
      {
        i = Menu(men, x1, y1, x2, y2, restore, title);
        if (i < 0)
          {
            return "";
          }
        return men[i];
      }
    return "";
  }

  bool SelFile(string mask,
               std::string& filename, std::string& dirname,
               int mode, const string& title,
               int x1, int y1, int x2, int y2)
  {
    string path;
    SplitFilename(mask, path, mask);
    if (path.empty())
      {
        path = ".";
      }
    bool cont = true;
    bool ok = false;
    while (cont)
      {
        vector<string> dir;
        int dirnr = 0;
        if (mode & DIR_SELPATH)   // directory list is needed to change directory
          {
            dir.push_back("..");
            Directory(dir, path, "*", DIR_DIR);
            for (unsigned int i = 0; i < dir.size(); i++)
              {
                dir[i] = "[~" + dir[i] + "]";
              }

            dirnr = dir.size();
          }
        else
          {
            if (mode & DIR_DIR)
              {
                Directory(dir, path, mask, DIR_DIR);
                for (unsigned int i = dirnr; i < dir.size(); i++)
                  {
                    dir[i] = "~" + dir[i];
                  }
              }
          }
        if (mode & DIR_FILE)
          {
            Directory(dir, path, mask, DIR_FILE);
            for (unsigned int i = dirnr; i < dir.size(); i++)
              {
                dir[i] = "~" + dir[i];
              }
          }

        if (dir.empty())
          {
            cont = false;
          }

        string mtitle = title + " (" + path + ")";
        int i = Menu(dir, x1, y1, x2, y2, true, mtitle);
        if (i < 0)
          {
            cont = false;
          }
        else
          {
            if (i < dirnr)
              {
                string selpath = dir[i].substr(2, dir[i].length() - 3);
                path += "/" + selpath;
              }
            else
              {
                filename = dir[i].substr(1);
                dirname = path;
                ok = true;
                cont = false;
              }
          }
      }
    return ok;
  }

  bool SelFile(const std::string& mask, std::string& filename)
  {
    string path;
    bool rc = SelFile(mask, filename, path);
    filename = path + "/" + filename;
    return rc;
  }

#undef FNAME
}

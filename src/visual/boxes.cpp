/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <sys/types.h>
#include <fcntl.h>

#if !WIN32 && !__BORLANDC__
#include <unistd.h>
#endif

#include <sys/stat.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <vector>

#include "IceException.h"
#include "defs.h"
#include "visual/screen.h"
#include "visual/xio.h"
//#include "string.h"
#include "visual/boxes.h"

namespace ice
{
#define FNAME "Box"
  int Box(int x1, int y1, int x2, int y2, bool frame)
  {
    int x, y;
    int xm, ym;
    xm = AlphaSizeX() - 1;
    ym = AlphaSizeY() - 1;
    if (
      (x1 < 0) || (x1 > x2) || (x2 > xm) ||
      (y1 < 0) || (y1 > y2) || (y2 > ym)
    )
      throw IceException(FNAME, M_WRONG_PARAM);

    if (frame)
      {
        SetAlphaCursor(x1, y1);
        PutChar(R_LO);
        for (x = x1 + 1; x < x2; x++)
          {
            PutChar(R_OU);
          }
        PutChar(R_RO);
        for (y = y1 + 1; y < y2; y++)
          {
            SetAlphaCursor(x1, y);
            PutChar(R_LR);
            for (x = x1 + 1; x < x2; x++)
              {
                PutChar(' ');
              }
            PutChar(R_LR);
          }
        SetAlphaCursor(x1, y2);
        PutChar(R_LU);
        for (x = x1 + 1; x < x2; x++)
          {
            PutChar(R_OU);
          }
        if ((x2 < xm) || (y2 < ym))   /* avoid rolling */
          {
            PutChar(R_RU);
          }
      }
    else
      {
        for (y = y1; y <= y2; y++)
          {
            SetAlphaCursor(x1, y);
            for (x = x1; x <= x2; x++)
              {
                PutChar(' ');
              }
          }
      }
    return OK;
  }
#undef FNAME

#define FNAME "SetBox"
  int SetBox(int xs, int ys, int where)
  {
    int x, y;
    int xm, ym;

    xm = AlphaSizeX() - 1;
    ym = AlphaSizeY() - 1;

    if ((xs > xm - 2) || (ys > ym - 2))
      throw IceException(FNAME, M_WRONG_PARAM);

    x = 0;
    y = 0;
    switch (where & (B_LEFT | B_RIGHT))
      {
      case 0:
        x = (xm - xs - 1) / 2;
        break;
      case B_LEFT:
        x = 0;
        break;
      case B_RIGHT:
        x = xm - xs - 1;
        break;
      case B_RIGHT|B_LEFT:
        x = 0;
        xs = xm - 1;
        break;
      }

    switch (where & (B_TOP | B_BOTTOM))
      {
      case 0:
        y = (ym - ys - 1) / 2;
        break;
      case B_TOP:
        y = 0;
        break;
      case B_BOTTOM:
        y = ym - ys - 1;
        break;
      case B_TOP|B_BOTTOM:
        y = 0;
        ys = ym - 1;
        break;
      }

    Box(x, y, x + xs + 1, y + ys + 1);

    SetAlphaCursor(x + 1, y + 1);
    return OK;
  }
#undef FNAME

#define FNAME "MessageBox"
  int MessageBox(const char* ps, int where)
  {
    char* ss[100];
    char s[STRLEN];
    strcpy(s, ps);
    int rc, xs, ys;
    int i, xa, ya;
    xs = 0;
    ys = 1;
    i = 0;
    xa = 0;
    ss[0] = s;
    while (s[i] != 0)
      {
        if (s[i] == '\n')
          {
            s[i] = '\0';
            ss[ys] = s + i + 1;
            if (xa > xs)
              {
                xs = xa;
              }
            xa = 0;
            ys++;
          }
        else
          {
            xa++;
          }
        i++;
      }
    if (xa > xs)
      {
        xs = xa;
      }
    if ((rc = SetBox(xs, ys, where)) != OK)
      {
        return rc;
      }
    GetAlphaCursor(&xa, &ya);
    for (i = 0; i < ys; i++)
      {
        SetAlphaCursor(xa, ya + i);
        Printf("%s", ss[i]);
      }
    return OK;
  }

  int MessageBox(const string& ps, int where)
  {
    vector<string> ss(1);
    unsigned int xs, ys;
    int rc;
    int i, xa, ya;
    xs = 0; // maximale Breite
    ys = 1; // Höhe
    i = 0;

    for (unsigned int c = 0; c < ps.length(); c++)
      {
        if (ps[c] == '\n')
          {
            if (ss[i].length() > xs)
              {
                xs = ss[i].length();  // maximale Breite aktualisieren
              }
            ys++;             // Höhe = Zeilenzahl aktualisieren
            i++;
            ss.push_back("");
          }
        else
          {
            ss[i] += ps[c];
          }
      }

    // letztes Wort extra behandeln
    if (ss[i].length() > xs)
      {
        xs = ss[i].length();  // maximale Breite aktualisieren
      }

    if ((rc = SetBox(xs, ys, where)) != OK)
      {
        return rc;
      }

    GetAlphaCursor(&xa, &ya);
    for (unsigned int i = 0; i < ys; i++)
      {
        SetAlphaCursor(xa + (xs - ss[i].length()) / 2, ya + i);
        Print(ss[i]);
      }
    return OK;
  }
#undef FNAME

#define FNAME "Ask"
  int Ask(const char* p, const char* yns)
  {
    char a;
    char yn[2];
    char s[STRLEN];
    if (strlen(yns) < 2)
      throw IceException(FNAME, M_WRONG_PARAM);
    yn[0] = tolower(yns[0]);
    yn[1] = tolower(yns[1]);
    sprintf(s, "%s (%c/%c)", p, yn[0], yn[1]);
    PushAlpha();
    SetAttribute(C_WHITE, C_GREEN, 0, 1);
    MessageBox(s, B_CENTER);
    do
      {
        a = tolower(GetChar());
      }
    while ((a != yn[0]) && (a != yn[1]));
    PopAlpha();
    return (a == yn[0]);
  }

  int Ask(const string& p, const string& yns)
  {
    char a;
    char yn[2];
    string s;
    if (yns.length() < 2)
      throw IceException(FNAME, M_WRONG_PARAM);
    yn[0] = toupper(yns[0]);
    yn[1] = toupper(yns[1]);
    s = p + "\n" + yn[0] + " / " + yn[1];
    PushAlpha();
    SetAttribute(C_WHITE, C_GREEN, 0, 1);
    MessageBox(s, B_CENTER);
    do
      {
        a = toupper(GetChar());
      }
    while ((a != yn[0]) && (a != yn[1]));
    PopAlpha();
    return (a == yn[0]);
  }
#undef FNAME

#define FNAME "Enter"
  int Enter(const char* s)
  {
    int c;
    PushAlpha();
    SetAttribute(C_WHITE, C_GREEN, 0, 1);
    SetAlphaCursor(10, 12);
    MessageBox(s, B_CENTER);
    do
      {
        c = GetChar();
      }
    while ((c != 13) && (c != 27));
    PopAlpha();
    return (c == 13);
  }

  int Enter(const string& s)
  {
    int c;
    PushAlpha();
    SetAttribute(C_WHITE, C_GREEN, 0, 1);
    SetAlphaCursor(10, 12);
    MessageBox(s, B_CENTER);
    do
      {
        c = GetChar();
      }
    while ((c != 13) && (c != 27));
    PopAlpha();
    return (c == 13);
  }
#undef FNAME
}

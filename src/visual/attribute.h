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
#ifndef __ATTRIBUTE_H
#define __ATTRIBUTE_H

namespace ice
{
  // the following enums should provide better readability in the main
  // function, e.g. instead of SetAttribute (4, 0, true, false); the
  // user can write SetAttribute (FgColorBlue | BgColorBlack | Inverted);

  class TextAttribs
  {
  public:
    static const int FgColorBlack = 0x1;
    static const int FgColorRed = 0x3;
    static const int FgColorGreen = 0x5;
    static const int FgColorBlue = 0x7;
    static const int FgColorYellow = 0x9;
    static const int FgColorCyan = 0xB;
    static const int FgColorMagenta = 0xD;
    static const int FgColorWhite = 0xF;
    static const int Foreground = 0xF;

    static const int BgColorBlack = 0x10;
    static const int BgColorRed = 0x30;
    static const int BgColorGreen = 0x50;
    static const int BgColorBlue = 0x70;
    static const int BgColorYellow = 0x90;
    static const int BgColorCyan = 0xB0;
    static const int BgColorMagenta = 0xD0;
    static const int BgColorWhite = 0xF0;
    static const int Background = 0xF0;

    static const int Highlighted = 0x100;
    static const int NotHighlighted = 0x200;
    static const int Highlight = 0x300;

    static const int Inverted = 0x400;
    static const int NotInverted = 0x800;
    static const int Invert = 0xC00;

    TextAttribs(): attr(FgColorWhite | BgColorBlue) {}
    TextAttribs(int at): attr(at) {}

    void setSpecificAttribute(int at, int mask)
    {
      attr = (attr & ~mask) | (at & mask);
    }
    int getSpecificAttribute(int mask) const
    {
      return attr & mask;
    }

    void setForeground(int at)
    {
      setSpecificAttribute(at, Foreground);
    }
    void setBackground(int at)
    {
      setSpecificAttribute(at, Background);
    }
    void setHighlight(int at)
    {
      setSpecificAttribute(at, Highlight);
    }
    void setInvert(int at)
    {
      setSpecificAttribute(at, Invert);
    }

    int getForeground() const
    {
      return getSpecificAttribute(Foreground);
    }
    int getBackground() const
    {
      return getSpecificAttribute(Background);
    }
    int getHighlight() const
    {
      return getSpecificAttribute(Highlight);
    }
    int getInvert() const
    {
      return getSpecificAttribute(Invert);
    }

    TextAttribs operator| (TextAttribs second) const
    {
      return TextAttribs(attr | second.attr);
    }

    TextAttribs operator& (TextAttribs second) const
    {
      return TextAttribs(attr & second.attr);
    }

    void Apply(const TextAttribs& neu)
    {
      int val;
      if ((val = neu.getForeground()) > 0)
        {
          setForeground(val);
        }
      if ((val = neu.getBackground()) > 0)
        {
          setBackground(val);
        }
      if ((val = neu.getHighlight()) > 0)
        {
          setHighlight(val);
        }
      if ((val = neu.getInvert()) > 0)
        {
          setInvert(val);
        }
    }

    operator int() const
    {
      return attr;
    }
  private:
    int attr;
  };

}
#endif // ifndef ATTRIBUTE_H

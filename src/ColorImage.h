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
#ifndef _COLOR_IMAGE_H
#define _COLOR_IMAGE_H

#include <string>
#include "ColorValue.h"
#include "numbase.h"
#include "base.h"
#include "fitfn.h"
#include "arith.h"
#include "defs.h"

namespace ice
{
  // ----------------------------------------------------------------------

  class ColorImage final
  {

  public:
    typedef ColorValue ValueType;
    // constructors and destructor
    ColorImage(): red(), green(), blue(), xsize(0), ysize(0), maxval(0), title() {};
    ColorImage(const ColorImage& src, const Window& window,
               const std::string& title = "");

    /*virtual*/
    ColorImage operator()(const Window& window, const std::string& title = "")
    {
      return ColorImage(*this, window, title);
    }

    ColorImage(const Image& rot, const Image& gruen, const Image& blau, const std::string& title = "");

    /*virtual*/
    ~ColorImage() {};

    // [re-]creation of real colorimage
    // given sizes
    void create(int SizeX, int SizeY, int maxValue, const std::string& title = "");
    // given template colorimage
    void create(const ColorImage& templateImg, const std::string& title = "");
    // given template image
    void create(const Image& templateImg, const std::string& title = "");

    // copy = recreate with new image size and deep copy of source data
    void copy(const ColorImage& src);
    void copy(const Image& src);

    // named constructors
    static ColorImage createColorImage(int sizeX, int sizeY, int maxValue, const std::string& title = "");
    static ColorImage createColorImage(const ColorImage& src, const std::string& title = "", bool copy = false);
    static ColorImage createColorImage(const Image& src, const std::string& title = "", bool copy = false);

    // check for valid ColorImage:
    // component images must be valid and have the same size
    // must not be the same data object
    /*virtual*/
    bool isValid() const;
    void destroy()
    {
      red.destroy();
      green.destroy();
      blue.destroy();
    }

    // set ColorImage to value
    // Set all pixel with colorvalue cv
    /*virtual*/
    void set(ColorValue cv) const;
    // set all pixel with grayvalues red, green, blue
    /*virtual*/
    void set(int red, int green, int blue) const;

    /*virtual*/
    ColorValue getPixel(int x, int y) const
    {
      return ColorValue(red.getIntensity(x, y),
                        green.getIntensity(x, y),
                        blue.getIntensity(x, y));
    }

    /*virtual*/ ColorValue getPixel(IPoint p) const
    {
      return ColorValue(red.getIntensity(p),
                        green.getIntensity(p),
                        blue.getIntensity(p));
    }

    /*virtual*/ ColorValue getPixelUnchecked(int x, int y) const
    {
      return ColorValue(red.getIntensityUnchecked(x, y),
                        green.getIntensityUnchecked(x, y),
                        blue.getIntensityUnchecked(x, y));
    }

    /*virtual*/ ColorValue getPixelUnchecked(IPoint p) const
    {
      return ColorValue(red.getIntensityUnchecked(p),
                        green.getIntensityUnchecked(p),
                        blue.getIntensityUnchecked(p));
    }

    /*virtual*/ ColorValue getPixel(double x, double y) const
    {
      return ColorValue(maxval - RoundInt(GetInterpolVal(red, x, y)),
                        maxval - RoundInt(GetInterpolVal(green, x, y)),
                        maxval - RoundInt(GetInterpolVal(blue, x, y)));
    }

    /*virtual*/ ColorValue getPixel(Point p) const
    {
      return ColorValue(maxval - RoundInt(GetInterpolVal(red, p.x, p.y)),
                        maxval - RoundInt(GetInterpolVal(green, p.x, p.y)),
                        maxval - RoundInt(GetInterpolVal(blue, p.x, p.y)));
    }

    /*virtual*/ void setPixel(int x, int y, ColorValue rgb) const
    {
      red.setIntensity(x, y, rgb.red);
      green.setIntensity(x, y, rgb.green);
      blue.setIntensity(x, y, rgb.blue);
    }

    /*virtual*/ void setPixel(IPoint p, ColorValue rgb) const
    {
      red.setIntensity(p, rgb.red);
      green.setIntensity(p, rgb.green);
      blue.setIntensity(p, rgb.blue);
    }

    /*virtual*/ void setPixelUnchecked(int x, int y, ColorValue rgb) const
    {
      red.setIntensityUnchecked(x, y, rgb.red);
      green.setIntensityUnchecked(x, y, rgb.green);
      blue.setIntensityUnchecked(x, y, rgb.blue);
    }

    /*virtual*/ void setPixelUnchecked(IPoint p, ColorValue rgb) const
    {
      red.setIntensityUnchecked(p, rgb.red);
      green.setIntensityUnchecked(p, rgb.green);
      blue.setIntensityUnchecked(p, rgb.blue);
    }

    /*virtual*/ void setPixelLimited(IPoint p, ColorValue rgb) const
    {
      setPixel(p, rgb.Limited(maxval));
    }

    /*virtual*/ void setPixelLimited(int x, int y, ColorValue rgb) const
    {
      setPixel(x, y, rgb.Limited(maxval));
    }

    /*virtual*/ void setPixelClipped(IPoint p, ColorValue rgb) const
    {
      if (inside(p))
        {
          setPixelLimited(p, rgb);
        }
    }

    /*virtual*/ void setPixelClipped(int x, int y, ColorValue rgb) const
    {
      if (inside(x, y))
        {
          setPixelLimited(x, y, rgb);
        }
    }

    /*virtual*/ void clear() const
    {
      set(maxval, maxval, maxval);
    }

    /*virtual*/
    const Image& redImage() const
    {
      return red;
    };

    Image& redImage()
    {
      return red;
    };

    /*virtual*/
    const Image& greenImage() const
    {
      return green;
    };

    Image& greenImage()
    {
      return green;
    };

    /*virtual*/
    const Image& blueImage() const
    {
      return blue;
    };

    Image& blueImage()
    {
      return blue;
    };

    /*virtual*/
    const Image& operator[](int i) const
    {
      switch (i)
        {
        case 0:
          return red;
        case 1:
          return green;
        case 2:
          return blue;
        default:
          return red/*Image()*/;
        }
    }

    /*virtual*/ bool inside(IPoint p) const
    {
      if (p.x < 0)
        {
          return false;
        }

      if (p.y < 0)
        {
          return false;
        }

      if (p.x >= xsize)
        {
          return false;
        }

      if (p.y >= ysize)
        {
          return false;
        }

      return true;
    }

    /*virtual*/ bool inside(const Point& p) const
    {
      if (p.x < -0.5)
        {
          return false;
        }

      if (p.y < -0.5)
        {
          return false;
        }

      if (p.x >= xsize - 0.5)
        {
          return false;
        }

      if (p.y >= ysize - 0.5)
        {
          return false;
        }

      return true;
    }

    /*virtual*/ bool inside(int x, int y) const
    {
      if (x < 0)
        {
          return false;
        }

      if (y < 0)
        {
          return false;
        }

      if (x >= xsize)
        {
          return false;
        }

      if (y >= ysize)
        {
          return false;
        }

      return true;
    }

    // Check if given window is inside an image
    /*virtual*/ bool inside(const Window& window) const
    {
      return inside(window.p1) && inside(window.p2);
    }

    /*virtual*/
    void checkSizes(const ColorImage& img2) const;
    void checkImages(const ColorImage& img2) const;

    /*virtual*/
    int read(const std::string& filename);
    /*virtual*/
    int write(const std::string& filename) const;

    /*
    virtual int invert();
    virtual int invert(const ColorImage &i1);
    */

    /*virtual*/
    std::string getTitle() const
    {
      return title;
    }
  protected:
    /*virtual*/
    bool makeValid();

    Image red;
    Image green;
    Image blue;

  public:
    int xsize;
    int ysize;
    int maxval;

    std::string title;
  };

  ColorImage newColorImage(int sizeX, int sizeY, int maxValue, const std::string& title = "");
  ColorImage newColorImage(const ColorImage& src, const std::string& title = "", bool copy = false);
  ColorImage newColorImage(const Image& src, const std::string& title = "", bool copy = false);

}
#endif  /* IFNDEF _BASE_H */

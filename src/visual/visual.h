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
//--------------------
// Visualisation
//--------------------

#ifndef _VISUAL_H
#define _VISUAL_H

#include <string>
#include "base.h"
#include "visual/attribute.h"
#include "visual/ImageWindow.h"
#include "ColorImage.h"

#define ICEGRAFICNAME "ICE Graphic"

namespace ice
{
  // OpenGraphic is deprecated and will have no effect
  int OpenGraphic(const std::string& windowname);

  // SetVirtualScreen is deprecated and will have no effect
  int SetVirtualScreen(int);

  // Display can be used to manipulate the image window
  // parameters: option=Action to be performed, may have the following values:
  //     RESET = all image windows are close
  //     ON = image window will come in the foreground
  //     OFF = image windows will be iconized
  int Display(int option);

  // Zooms the Image >Img<.
  // If >ZoomFactor< is greater than zero, each image point will
  // be displayed by a ZoomFactor times ZoomFactor sized rectangle,
  // if it is less than zero only every (-ZoomFactor)th point is drawn.
  // ZoomFactor zero means that the greatest zoom
  // factor, such that the image still fits its window, will be used.

  int Zoom(Visual v, int ZoomFactor, int = 0, int = 0);
  int Zoom(const ImageD& Img, int ZoomFactor, int = 0, int = 0);
  int Zoom(const Image& Img, int ZoomFactor, int = 0, int = 0);
  int Zoom(const Image& img);

  // Sets the color for the gray value >Entry< to the RGB color (RedValue, GreenValue, BlueValue).
  int SetGrayColor(unsigned int Entry,
                   unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);

  // Same as above, but affecting only the Image >Img<. This function will only have effects
  // Images that are visualized in gray value windows or overlay windows.
  int SetGrayColor(Image Img, unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);

  // This function only affects images that are displayed as gray value images at the moment,
  // and whose maximal gray value is greater or equal to >Last<. This function resets
  // the colors which will be used to paint a gray value pixel. Only gray values between
  // >First< and >Last< will be painted in a linear interpolated shade of gray. The rest will
  // be painted black or white. This function is useful for increasing contrast in pictures,
  // whose gray values lie close together.
  int SetGrayLUT(unsigned int First, unsigned int Last);

  // Same as above, but affecting only the Image >Img<. This function will only have effects
  // Images that are visualized in gray value windows or overlay windows.
  int SetGrayLUT(Image Img, unsigned int First, unsigned int Last);

  // This function affects all images that are displayed as overlay images. It changes the
  // overlay color at entry >Entry< to the specified RGB color.
  int SetOverlayColor(unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);
  // Same as above, but affecting only the Image >Img< (if it is visualized in an overlay window).
  int SetOverlayColor(Image Img, unsigned int Entry, unsigned char RedValue, unsigned char GreenValue, unsigned char BlueValue);

  // The next two function are deprecated, since there is no global color table anymore.
  // Instead each window will have its own color table.
  int GetGrayColor(int entry, int& red, int& green, int& blue);
  int GetOverlayColor(int entry, int& red, int& green, int& blue);

  //  Reading of color tables.
  int GetGrayColor(const Image& Img,
                   unsigned int Entry,
                   unsigned char& Red,
                   unsigned char& Green,
                   unsigned char& Blue);
  int GetOverlayColor(const Image& Img,
                      unsigned int Entry,
                      unsigned char& Red,
                      unsigned char& Green,
                      unsigned char& Blue);

  // Returns information about the visualization in general.
  // The returned information depends on the paramater >code<.
  int InfVis(int code);

  // Control visualization of Image(s)
  Visual Show(int Mode, Visual Img);
  Visual Show(int Mode,
              const ice::Image& Img,
              const std::string& name = "");

  Visual Show(int Mode,
              ice::ImageD& Img,
              const std::string& name = "");

  Visual Show(int Mode,
              const ice::Image& Image1,
              const ice::Image& Image2,
              const std::string& name = "");

  Visual Show(int Mode,
              const ice::Image& Image1,
              const ice::Image& Image2,
              const ice::Image& Image3,
              const std::string& name = "");

  Visual Show(int Mode,
              const ice::Image& Image1,
              const ice::Image& Image2,
              const ice::Image& Image3,
              const ice::Image& Image4,
              const ice::Image& Image5,
              const ice::Image& Image6,
              const std::string& name = "");

  Visual Show(int Mode,
              const ice::ColorImage& Image1,
              const ice::ColorImage& Image2,
              const std::string& name = "");

  Visual getVisual(const Image& img);
  Visual getVisual(const ImageD& img);
  Visual getVisual(const ColorImage& img);

  // Manipulation of the image windows cursor (will only have effects,
  // if the image is visualized).
  int Cursor(int Mode, const Image& Img);
  int Cursor(int Mode, const Image& Img, int x, int y);
  int Cursor(int Mode, const Image& Img, IPoint p);
}

#endif /* ifdef _VISUAL_H */

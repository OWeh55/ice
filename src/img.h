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
/*
 * img.h: definitions for a driver concept for imaging devives
 *              imaging devices are scanner, video digitizer,
 *              frame grabber or other devices wich
 *              deliver digitized images
 *              This defines a number of ioctl functions
 * Author:  Wolfgang Ortmann <noo@uni-jena.de>
 */

#ifndef img_h
#define img_h
namespace ice
{
  /* device number for imaging devices */
#define IMG_MAJOR       50
#define IMG_NAME        "/dev/img"

  /* type number for ioctl */
#define IMG_IO_BASE 1
#define IMG_IO_CUSTOM   8

  /* ioctl definitions */
  /* mode setting */
#define IMG_GET_MODES        _IOW(IMG_IO_BASE,1,long)
#define IMG_SET_MODE         _IOR(IMG_IO_BASE,2,long)
  /* predefined modes */
#define IMG_GRAY           0
#define IMG_COLOUR         1
#define IMG_GRAY_HIGHEST   2
#define IMG_COLOUR_HIGHEST 3
#define IMG_GRAY_MOVIE     4
#define IMG_COLOUR_MOVIE   5
#define IMG_GRAY_MOVIE_FIELD     6
#define IMG_COLOUR_MOVIE_FIELD   7
#define IMG_GREY           IMG_GRAY
#define IMG_COLOR          IMG_COLOUR
#define IMG_GREY_HIGHEST   IMG_GRAY_HIGHEST
#define IMG_COLOR_HIGHEST  IMG_COLOUR_HIGHEST
#define IMG_GREY_MOVIE     IMG_GRAY_MOVIE
#define IMG_COLOR_MOVIE    IMG_COLOUR_MOVIE
#define IMG_GREY_MOVIE_FIELD     IMG_GRAY_MOVIE_FIELD
#define IMG_COLOR_MOVIE_FIELD    IMG_COLOUR_MOVIE_FIELD
#define IMG_CUSTOMMODE   32
  /* bit masks for IMG_GET_MODES */
#define IMG_GRAY_MASK 1 << IMG_GRAY
#define IMG_COLOUR_MASK 1 << IMG_COLOUR
#define IMG_GRAY_HIGHEST_MASK 1 << IMG_GRAY_HIGHEST
#define IMG_COLOUR_HIGHEST_MASK 1 << IMG_COLOUR_HIGHEST
#define IMG_GRAY_MOVIE_MASK 1 << IMG_GRAY_MOVIE
#define IMG_COLOUR_MOVIE_MASK 1 << IMG_COLOUR_MOVIE
#define IMG_GRAY_MOVIE_MASK 1 << IMG_GRAY_MOVIE
#define IMG_COLOUR_MOVIE_MASK 1 << IMG_COLOUR_MOVIE
#define IMG_GRAY_MOVIE_FIELD_MASK 1 << IMG_GRAY_MOVIE_FIELD
#define IMG_COLOUR_MOVIE_FIELD_MASK 1 << IMG_COLOUR_MOVIE_FIELD

#define IMG_GREY_MASK IMG_GRAY_MASK
#define IMG_COLOR_MASK  IMG_COLOUR_MASK
#define IMG_GREY_HIGHEST_MASK IMG_GRAY_HIGHEST_MASK
#define IMG_COLOR_HIGHEST_MASK IMG_COLOUR_HIGHEST_MASK
#define IMG_GREY_MOVIE_MASK IMG_GRAY_MOVIE_MASK
#define IMG_COLOR_MOVIE_MASK IMG_COLOUR_MOVIE_MASK
#define IMG_GREY_MOVIE_MASK_FIELD IMG_GRAY_MOVIE_MASK_FIELD
#define IMG_COLOR_MOVIE_MASK_FIELD IMG_COLOUR_MOVIE_MASK_FIELD

#define IMG_QUERY_MODE       _IOW(IMG_IO_BASE,3,ModeDescription)
#define IMG_GET_PROPERTIES   _IOW(IMG_IO_BASE,4,ModeDescription)
#define IMG_GRAB              _IO(IMG_IO_BASE,5)
#define IMG_SET_CONTRAST     _IOR(IMG_IO_BASE,6,long)
#define IMG_SET_BRIGHTNESS   _IOR(IMG_IO_BASE,7,long)
#define IMG_GET_CONTRAST     _IOW(IMG_IO_BASE,8,long)
#define IMG_GET_BRIGHTNESS   _IOW(IMG_IO_BASE,9,long)
#define IMG_SET_SCANWINDOW   _IOR(IMG_IO_BASE,10,Window)
#define IMG_GET_SCANWINDOW   _IOW(IMG_IO_BASE,11,Window)
#define IMG_SET_SCALEFACTOR  _IOR(IMG_IO_BASE,12,long)
#define IMG_GET_SCALEFACTOR  _IOW(IMG_IO_BASE,13,long)
#define IMG_SET_IMAGENUMBER  _IOR(IMG_IO_BASE,14,long)
#define IMG_SET_FORMAT       _IOR(IMG_IO_BASE,15,long)
#define IMG_GET_FORMAT       _IOW(IMG_IO_BASE,16,long)
#define IMG_FMT_PGM 0
#define IMG_FMT_RAW 1
#define IMG_MAKEDEFAULT      _IOR(IMG_IO_BASE,17,long)

  typedef  struct window_img
  {
    int x1, y1;
    int x2, y2;
  } Window;

  typedef  struct modedescription
  {
    int ModeNumber;
    int width, height;
    int planes;
    int maxval;
    int interlace;
    int etime;
    int view;
  } ModeDescription;
}
#endif

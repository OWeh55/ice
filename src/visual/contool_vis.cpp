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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#include "defs.h"
#include "message.h"
#include "macro.h"

#include "Contur.h"
#include "contfeat.h"
#include "lineseg.h"
#include "draw.h"

#include "visual/mouse.h"
#include "visual/visual.h"

#include "contools.h"

using namespace std;

namespace ice
{
  /**********************************************************
    Interaktives Selektieren einer Kontur
  **********************************************************/
#define FNAME "SelContur"
  Contur SelContur(const Image& img, int force_close)
  {
    IPoint lastPoint, newPoint;
    Contur lastLine; // we are using Contur to store last selected Line, because
    // this makes the drawing exactly the same as for the whole contur

    Contur c;

    if (!IsImg(img))
      {
        Message(FNAME, M_WRONG_IMAGE, WRONG_POINTER);
        return c;
      }

    lastPoint = IPoint(img->xsize / 2, img->ysize / 2);
    newPoint = lastPoint;

    Cursor(SET, img, lastPoint);

    // first Selection = starting point
    while (!(Mouse(img, newPoint) & M_IN_WINDOW)); // wait for "mouse in window"

    bool haveFirstPoint = false;

    int mstat = Mouse(img, newPoint);
    while (!haveFirstPoint && (mstat & M_RIGHT_DOWN) == 0)
      {
        if (mstat == (M_MOVED | M_IN_WINDOW))
          {
            if (Inside(img, newPoint))
              {
                Cursor(SET, img, newPoint);
                lastPoint = newPoint;
              }
          }
        haveFirstPoint = (mstat & M_LEFT_DOWN) != 0;
        mstat = Mouse(img, newPoint);
      }

    if ((mstat & M_RIGHT_DOWN) != 0)
      {
        return c;  // aborted
      }

    lastPoint = newPoint;

    c.SetStart(lastPoint);
    lastLine.Reset(lastPoint);

    bool selecting = true;

    Cursor(OFF, img);

    while (selecting)
      {
        while (selecting && ((mstat = Mouse(img, newPoint)) & M_LEFT_DOWN) != 0)
          {
            // left + right button pressed
            if ((mstat & (M_RIGHT_DOWN | M_LEFT_DOWN | M_IN_WINDOW)) == (M_RIGHT_DOWN | M_LEFT_DOWN | M_IN_WINDOW))
              {
                // Draw(lastLine,img,0);
                lastLine.Reset(lastPoint);
                lastLine.Add(c.Start());
                MarkContur(lastLine, img->maxval, img);
                c.Close();
                selecting = false;
              }
            else
              {
                // left button only
                MarkContur(lastLine, 0, img);
                newPoint.x = max<int>(0, min<int> (img->xsize - 1, newPoint.x));
                newPoint.y = max<int>(0, min<int> (img->ysize - 1, newPoint.y));
                lastLine.Reset(lastPoint);
                lastLine.Add(newPoint);
                MarkContur(lastLine, img->maxval, img);
                c.Add(newPoint);
                lastLine.Reset(newPoint);
                lastPoint = newPoint;
              }
          }

        while (selecting && ((mstat = Mouse(img, newPoint)) & M_LEFT_DOWN) == 0)
          {
            // right button pressed
            if ((mstat & (M_RIGHT_DOWN | M_IN_WINDOW)) == (M_RIGHT_DOWN | M_IN_WINDOW))
              {
                MarkContur(lastLine, 0, img);
                lastLine.Reset(lastPoint);
                lastLine.Add(newPoint);
                MarkContur(lastLine, img->maxval, img);
                c.Add(newPoint);
                if (force_close)
                  {
                    lastLine.Reset(newPoint);
                    lastLine.Add(c.Start());
                    MarkContur(lastLine, img->maxval, img);
                    c.Close();
                  }
                selecting = false;
              }
            else
              {
                if ((mstat & (M_MOVED | M_IN_WINDOW)) == (M_MOVED | M_IN_WINDOW))
                  {
                    MarkContur(lastLine, 0, img);
                    newPoint.x = max<int>(0, min<int> (img->xsize - 1, newPoint.x));
                    newPoint.y = max<int>(0, min<int> (img->ysize - 1, newPoint.y));
                    lastLine.Reset(lastPoint);
                    lastLine.Add(newPoint);
                    MarkContur(lastLine, img->maxval, img);
                  }
              }
          }
      }
    return c;
  }
#undef FNAME
}

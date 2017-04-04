/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2012 FSU Jena, Digital Image Processing Group
 * Contact: ice@uni-jena.de
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

#include "freeman.h"
#include "conturfollowing.h"

namespace ice
{

#define FNAME "CalcContur"
  static bool Go(IPoint start, Freeman startdir, bool forward,
                 const ObjectFunctor& of, Contur& res)
  {
    IPoint pObject = start;
    IPoint pUnderground = startdir.step(pObject);
    Freeman adir = startdir;

    res.Reset();

    int dir2 = 2;
    int dir1 = 1;
    int dir6 = 6;

    if (!forward)
      {
        dir2 = 6; // search backward
        dir1 = 7;
        dir6 = 2;
      }

    res.SetStart(pObject);

    //    std::cout << "is Valid " << res.isValid() << std::endl;

    // Suche
    bool stop = false;

    do
      {
        IPoint test = (adir + dir2).step(pObject);
        object_rc obj = of(test);

        if (obj == isunderground)
          {
            // ?? ..
            // ?? ..
            //    --
            // ..|XX
            // ..|XX
            // rechts abbiegen
            pUnderground = test;
            adir = adir + dir2;
          }
        else
          {
            if (obj == isunknown)
              {
                // ?? uu
                // ?? uu
                //
                // ..|XX
                // ..|XX

                IPoint test2 = (adir + dir1).step(pObject);
                obj = of(test2);

                if (obj == isobject)
                  {
                    // links abbiegen
                    // XX uu
                    // XX uu
                    // --
                    // ..|XX
                    // ..|XX
                    // links abbiegen
                    res.Add((adir + dir1).Int());
                    pObject = test2;
                    adir = adir + dir6;
                  }
                else
                  {

                    // .. uu
                    // .. uu
                    //
                    // ..|XX
                    // ..|XX
                    // stop

                    // ODER

                    // uu uu
                    // uu uu
                    //
                    // ..|XX
                    // ..|XX
                    // stop
                    stop = true;
                  }
              }
            else
              {
                // ?? XX
                // ?? XX
                //
                // ..|XX
                // ..|XX
                IPoint test2 = (adir + dir1).step(pObject);
                obj = of(test2);

                if (obj == isobject)
                  {
                    // XX XX
                    // XX XX
                    // --
                    // ..|XX
                    // ..|XX
                    // links abbiegen
                    res.Add((adir + dir1).Int());
                    pObject = test2;
                    adir = adir + dir6;
                  }
                else
                  {
                    if (obj == isunderground)
                      {
                        // ..|XX
                        // ..|XX
                        //   |
                        // ..|XX
                        // ..|XX
                        // geradeaus
                        pObject = test;
                        pUnderground = test2;
                        res.Add((adir + dir2).Int());
                      }
                    else     /* obj == isunknown */
                      {
                        // uu XX
                        // uu XX
                        //
                        // ..|XX
                        // ..|XX
                        // Halt
                        stop = true;
                      }
                  }

              }
          }
      }
    while ((pObject != start || !(adir == startdir)) && !stop);

    //    std::cout << "Ende is Valid " << res.isValid() << std::endl;
    return stop;
  }

  Contur CalcContur(const ObjectFunctor& of, IPoint start)
  {
    Contur res;

    // ist startpunkt objektpunkt ?

    if (of(start) != isobject)
      {
        throw IceException(FNAME, M_WRONG_STARTPOINT, WRONG_STARTPOINT);
      }

    // Suche Untergrundpunkt in Nachbarschaft (links beginnend)

    Freeman startdir(4);

    int cr = 0;
    while ((cr < 4) &&
           (of(startdir.step(start)) != isunderground))
      {
        startdir = startdir + 2 ;
        cr++;
      }

    if (cr == 4)                    // Startpunkt liegt im Objekt
      {
        throw IceException(FNAME, M_WRONG_STARTPOINT3, WRONG_STARTPOINT);
      }

    if (Go(start, startdir, true, of, res))   // if true -> open contur
      {
        // from start in inverse direction
        Contur h;
        Go(start, startdir, false, of, h);
        // invert found contur
        h.InvDir();
        // add first contur
        h.Add(res);
        // -> this is result
        res = h;
      }

    return res;
  }

  bool SearchStart(const ObjectFunctor& of, IPoint& start)
  {
    object_rc last = of(start);
    bool ok = false;

    for (int i = 0; i < 1000000 && !ok; i++)
      {
        start.x++;
        object_rc h = of(start);

        if (last == isunderground && h == isobject)
          {
            ok = true;
          }
        else if (h == isunderground && last == isobject)
          {
            start.x--;
            ok = true;
          }
      }

    return ok;
  }

#undef FNAME
} // namespace

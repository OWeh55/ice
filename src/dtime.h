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
/****************************************************************/
/* Zeitmessung unter DOS          */
/* (c) OWeh Mai 93            */
/****************************************************************/

#ifndef _DTIME_H
#define _DTIME_H

#define TM_WORLD 1
#define TM_PROCESS 2
#define TM_USER 3
#define TM_CHILD 4
#define TM_CHILDUSER 5

namespace ice
{
  void Delay(double t);
  double TimeD(int mode = TM_WORLD);
}
#endif

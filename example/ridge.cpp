/*****************************************************
 *  Konturfolge und Kantendetektion                  *
 *                                                   *
 *                                                   *
 *                                                   *
 *  (C) m.schubert 9.93                              *
 *****************************************************/
#include <stdlib.h>
#include <stddef.h>  /* notwendig fuer Unix !!! */
#include <math.h>

#include <image.h>
#include <message.h>
#include <defs.h>
#include <macro.h>
#include <numbase.h>
#include <contools.h>
#include <contfeat.h>
#include <Contur.h>
#include <threshld.h>

#define PGL 10
#define BOFF 2

#define IMGNAME "pic.tif"

/************************************************/
int main(int argc, char* argv[])
{
  Image img, imgm;
  Contur c;
  int ps[2] = {0, 0}, pgl, pgl1;

  if (argc > 1)
    img = ReadImg(argv[1]);
  else
    {
      img = ReadImg(IMGNAME);
    }

  imgm = NewImg(img->xsize, img->ysize, 63);
  SetImg(imgm, 0);
  Display(ON);
  Show(OVERLAY, img, imgm);
  /*   SmearImg(img,img); */
  pgl = RidgeThreshold(img);
  pgl1 = ice::max(pgl / 2, 3);
  printf("pgl = %d  pgl1 = %d\n", pgl, pgl1);

  while (SearchRidgeStart(img, imgm, pgl, 5, ps, HORZ) == OK)
    {
      c = CalcRidgeContur(img, imgm, pgl1, ps, 0);

      if (c.isValid())
        {
          if (c.Number() > 8) MarkContur(c, 2, imgm);
        }

      ps[0]++;
    }

  ps[0] = 0;
  ps[1] = 0;

  while (SearchRidgeStart(img, imgm, pgl, 5, ps, VERT) == OK)
    {
      c = CalcRidgeContur(img, imgm, pgl1, ps, 0);

      if (c.isValid())
        {
          if (c.Number() > 8) MarkContur(c, 2, imgm);
        }

      ps[1]++;
    }

  SelPoint(0, img, ps);
  Display(OFF);

  return 0;
}




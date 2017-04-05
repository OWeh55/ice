/* Liniendetektion (ridge) */
#include <cstdlib>
#include <math.h>

#include <image.h>
#include <defs.h>
#include <macro.h>
#include <numbase.h>
#include <contools.h>
#include <contfeat.h>
#include <Contur.h>
//#include <robvis.h>

using namespace ice;

/************************************************/
int main(int argc, char* argv[])
{
  Image img, imgm;
  Contur c;
  int ps[2] = {0, 0}, pgl, pgl1;
  char name[100];

  if (argc > 1)
    img = ReadImg(argv[1]);
  else
    {
      printf("Bildfile: ");
      fflush(stdout);
      scanf("%s", name);
      img = ReadImg(name);
    }

  imgm = NewImg(img->xsize, img->ysize, 63);
  setImg(imgm, 0);
  Display(ON);
  Show(OVERLAY, img, imgm);
  /*   SmearImg(img,img); */

  pgl = RidgeThreshold(img(Window(img->xsize / 5, img->ysize / 5,
                                  img->xsize * 4 / 5, img->ysize * 4 / 5)));

  pgl = pgl * 9 / 10;
  pgl1 = ice::max(pgl / 2, 3);
  printf("pgl = %d  pgl1 = %d\n", pgl, pgl1);

  while (SearchRidgeStart(img, imgm, pgl, 5, ps, HORZ) == OK)
    {
      c = CalcRidgeContur(img, imgm, pgl1, ps, 0);

      if (c.isValid())
        {
          if (c.Length() > 8)
            {
              MarkContur(c, 2, imgm);
            }
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
          if (c.Length() > 8)
            {
              MarkContur(c, 2, imgm);
            }
        }

      ps[1]++;
    }

  SelPoint(0, img, ps);

  Display(OFF);

  return 0;
}




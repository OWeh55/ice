/* Kantendetektion im Bild argv[1] */
#include <stdio.h>
#include <math.h>
#include <image.h>
#include <Contur.h>
#include <threshld.h>

using namespace ice;

#define PGL 10
#define IMG "~/Bilder/rob1.tif"

/************************************************/
int main(int argc, char* argv[])
{
  Image img, imgm;
  Contur c;
  int pgl = 30, pgl1;
  int ps[2] = {0, 0};
  char name[100];

  if (argc > 1)
    img = ReadImg(argv[1]);
  else
    {
      printf("Bildname: ");
      fflush(stdout);
      scanf("%s", name);
      img = ReadImg(name);
    }

  pgl1 = pgl / 3;
  imgm = NewImg(img->xsize, img->ysize, 63);
  clearImg(imgm);
  Display(ON);
  Show(OVERLAY, img, imgm);

  pgl = GradThreshold(img);
  pgl1 = pgl / 3;


  while (SearchGradStart(img, imgm, pgl, 5, ps, HORZ) == OK)
    {
      c = CalcGradContur(img, imgm, pgl1, 40, ps, 0);
      if (c.isValid())
        {
          if (c.Number() > 8)
            {
              if (c.isClosed()) MarkContur(c, 2, imgm);
              else MarkContur(c, 1, imgm);
            }
        }

      ps[0]++;
    }

  SelPoint(0, imgm, ps);
  Display(OFF);

  return 0;
}



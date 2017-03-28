/* Test für Sortieren von Punktlisten */
#include <stdio.h>
#include <stdlib.h>
#include <image_nonvis.h>


#define LNG 10000

int main(int argc, char* argv[])
{
  PointList pl, pls;
  int i;
  double time;
  pl = NewPointList(LNG);

  for (i = 0; i < LNG; i++)
    {
      PutPoint(pl, i, 3 * LNG * RandomD(), RandomD(), 1);
    }

  time = TimeD();
  pls = SortPointList(pl, 0, -1);
  printf("Zeit für %d Elemente: %f\n", LNG, TimeD() - time);

  for (i = 0; i < 100; i++)
    {
      printf("%f  %f    %f  %f\n", pl->xptr[i], pl->yptr[i], pls->xptr[i], pls->yptr[i]);
    }

  return 0;
}
